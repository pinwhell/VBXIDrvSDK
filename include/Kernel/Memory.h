#pragma once

#include <Kernel/Syms.h>
#include <etl/string.h>

REGPARAMDECL(uintptr_t) native_read_cr0();
REGPARAMDECL(void) native_write_cr0(uintptr_t newCr0);

#define CR0Read native_read_cr0
#define CR0Write(x) native_write_cr0(x)

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

void MemoryFromUserRead(void* dst, const void __user * src, size_t len);
void MemoryToUserWrite(void __user * dst, const void* src, size_t len);

template <typename T>
T MemoryFromUserReadObject(const void __user * at)
{
    T obj;

    MemoryFromUserRead(&obj, at, sizeof(obj));

    return obj;
}

template <typename T>
void MemoryToUserWriteObject(void __user * at, T what)
{
    MemoryToUserWrite(at, &what, sizeof(what));
}

void* MemoryAlloc(size_t size, bool bExecutable = true);
size_t HookBackupLengthGet(void* at);
void HookDetourInstall(void* at, void* replace);
size_t HookReplaceBackupCreate(void* at, void** outBackup);
bool HookTrampInstall(void* at, void* replace, void** backup);
bool HookTrampRestore(void* at, void* backup);

template<size_t maxCapacity>
etl::string<maxCapacity> MemoryFromUserString(const char __user* at)
{
    etl::string<maxCapacity> result;
    strncpy_from_user(result.data(), at, maxCapacity);
    return result;
}
