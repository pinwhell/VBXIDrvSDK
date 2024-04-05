#include <Kernel/Memory.h>
#include <Kernel/Syms.h>
#include <fslc_string.h>
#include <nmd_assembly.h>

#ifdef BITS32
constexpr size_t JMP_SZ = 5;
constexpr auto NMD_MODE = NMD_X86_MODE_32;
#elif defined(BITS64)
constexpr size_t JMP_SZ = 14;
constexpr auto NMD_MODE = NMD_X86_MODE_64;
#endif

struct ScopedDisableInterupts {
    ScopedDisableInterupts()
    {
        asm("cli");
    }

    ~ScopedDisableInterupts()
    {
        asm("sti");
    }
};

CR0ScopedBackup::CR0ScopedBackup()
{
    mPrevCR0 = CR0Read();
}

CR0ScopedBackup::~CR0ScopedBackup()
{
    CR0Write(mPrevCR0);
}

CR0WPDisableScoped::CR0WPDisableScoped()
{
    uintptr_t disableWpMask = 0x00010000;
    disableWpMask = ~disableWpMask;
    CR0Write(CR0Read() & disableWpMask);
}

void MemoryPatch(void* dst, const void* src, size_t len)
{
    CR0WPDisableScoped wpDisable;
    ScopedDisableInterupts iruptsDisable;

    fslc_memcpy(dst, src, len);
}

void MemoryFromUserRead(void* dst, const void __user* src, size_t len)
{
    copy_from_user(dst, src, len);
}

void MemoryToUserWrite(void __user* dst, const void* src, size_t len)
{
    copy_to_user(dst, src, len);
}

size_t X86HookBackupLengthGet(void* at)
{
    size_t result = 0;

    for (char* i = (char*)at;;)
    {
        size_t currLen = nmd_x86_ldisasm(i, 0x100, NMD_MODE);

        if (!currLen)
            break;

        result += currLen;
        i += currLen;

        if (result < JMP_SZ)
            continue;

        break;
    }

    return result;
}

void* MemoryAlloc(size_t size, bool bExecutable)
{
#ifdef BITS64
    constexpr auto KMALLOC_FLAGS = 0xD0;
#else
    constexpr auto KMALLOC_FLAGS = GFP_KERNEL;
#endif

    void* buff = kmalloc(size, KMALLOC_FLAGS);

    if (bExecutable)
    {
        size += (unsigned long long)buff & 0xFFFull;
        int nPages = ((size + 0xFFFull) & ~0xFFFull) >> 12;
        void* buffPgAlign = (void*)(((unsigned long long)buff + 0xFFFull) & ~0xFFFull);
        set_memory_x(buffPgAlign, nPages);
    }

    return buff;
}

void* operator new(size_t size)
{
    return MemoryAlloc(size);
}

void operator delete(void* ptr) noexcept {
    // Must Implement!.
    return;
}

size_t HookBackupLengthGet(void* at)
{
    return X86HookBackupLengthGet(at);
}

/*
* Computes a JMP Direct/Indirect from src to dst
* outJmp Expected to have JMP_SIZE bytes available
*/
void X86JMPCompute(void* outJmp, void* dst, void* src = nullptr)
{
    unsigned char jmpInst[]{
#ifdef BITS32
        0xE9, 0x00, 0x00, 0x00, 0x00,
#else 
        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, // jmp qword ptr [$+6]
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // ptr
#endif
    };

#ifdef BITS32
    *(int32_t*)(jmpInst + 1) = ((int32_t)dst - (int32_t)src) - 5;
#else
    *(void**)(jmpInst + 6) = dst;
#endif

    fslc_memcpy(outJmp, jmpInst, sizeof(jmpInst));
}

void X86HookDetourInstall(void* at, void* replace)
{
    char jmpDetour[JMP_SZ];
    X86JMPCompute(jmpDetour, replace, at);
    MemoryPatch(at, jmpDetour, sizeof(jmpDetour));
}

size_t HookReplaceBackupCreate(void* at, void** outBackup)
{
    if (outBackup == nullptr)
        return 0;

    size_t backupLen = HookBackupLengthGet(at);

    if (!backupLen)
        return backupLen;

    void* backup = MemoryAlloc(backupLen + JMP_SZ);
    fslc_memcpy(backup, at, backupLen);

    char* jmpBackStart = (char*)backup + backupLen;
    char* atResume = (char*)at + backupLen;
    X86JMPCompute(jmpBackStart, atResume, jmpBackStart);

    *outBackup = backup;

    return backupLen + JMP_SZ;
}

void HookDetourInstall(void* at, void* replace)
{
    X86HookDetourInstall(at, replace);
}

size_t HookTrampInstall(void* at, void* replace, void** backup)
{
    size_t backupLen = HookReplaceBackupCreate(at, backup);

    if (!backupLen)
        return backupLen;

    HookDetourInstall(at, replace);

    return backupLen;
}

bool HookTrampRestore(void* at, void* backup, size_t backupLen)
{
    if (backup == nullptr)
        return false;

    if(!backupLen)
        backupLen = HookBackupLengthGet(backup);

    if (!backupLen)
        return false;

    MemoryPatch(at, backup, backupLen);

    return true;
}

// int detour32(void* from, void* to, size_t len)
// {
//     if(len < 5)
//         return 0;

//     unsigned char jmp_dst[] = {
//         0xE9, 0x00, 0x00, 0x00, 0x00
//     };

//     int32_t rel_jmp_disp = (int32_t)to - (int32_t)from - 5;

//     *(int32_t*)&(jmp_dst[1]) = rel_jmp_disp;

//     patch(from, jmp_dst, 5);

//     return 1;
// }

// int trampoline32(void* from, void* to, size_t len, void** out_original)
// {
//     if(len < 5)
//         return 0;

//     if(out_original != 0)
//     {
//         unsigned char* gateway = (unsigned char*)kmalloc(len + 5, 0xD0);

//         if(gateway == 0)
//             return 0;

//         fslc_memcpy(gateway, from, len);

//         unsigned char jmp_back[] = {
//             0xE9, 0x00, 0x00, 0x00, 0x00
//         };

//         int32_t rel_jmp_back_disp = (int32_t)from - (int32_t)gateway - 5;

//         *(int32_t*)&(jmp_back[1]) = (int32_t)rel_jmp_back_disp;

//         fslc_memcpy(gateway + len, jmp_back, 5);

//         *out_original = gateway;
//     }

//     return detour32(from, to, len);
// }