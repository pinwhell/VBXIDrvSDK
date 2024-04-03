#pragma once

#include <Kernel/Syms.h>

REGPARAMDECL(uintptr_t) read_cr0();
REGPARAMDECL(void) write_cr0(uintptr_t newCr0);

#define CR0Read read_cr0
#define CR0Write(x) write_cr0(x)

struct CR0ScopedBackup {
    CR0ScopedBackup();
    ~CR0ScopedBackup();

    uintptr_t mPrevCR0;
};

struct CR0WPDisableScoped {

    CR0WPDisableScoped();

    CR0ScopedBackup mBackup;
};

void MemoryPatch(void* dst, const void* src, size_t len);

template<typename T>
void MemoryPatchObject(void* dst, T obj)
{
    MemoryPatch(dst, (const void*)&obj, sizeof(obj));
}

void patch(void* dst, const void* src, size_t sz);
int detour32(void* from, void* to, size_t len);
int trampoline32(void* from, void* to, size_t len, void** out_original);

template <typename T>
T MemoryReadFromUser(uintptr_t at)
{
    T obj;

    copy_from_user(&obj, (const void __user *)at, sizeof(T));

    return obj;
}

template <typename T>
void MemoryWriteToUser(uintptr_t at, T what)
{
    copy_to_user((void __user *)at, &what, sizeof(T));
}