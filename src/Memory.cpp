#include <Kernel/Memory.h>
#include <Kernel/Syms.h>
#include <fslc_string.h>

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
    CR0Write(CR0Read() & 0xFFFEFFFF);
}

void MemoryPatch(void* dst, const void* src, size_t len)
{
    CR0WPDisableScoped wpDisable;

    fslc_memcpy(dst, src, len);
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