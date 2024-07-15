#include <Kernel/Syms.h>
#include <Kernel/Syscalls.h>
#include <Kernel/Memory.h>
#include <etl/unordered_map.h>
#include <etl/unordered_set.h>

DECL(const void*) sys_call_table;

etl::unordered_map<int, void*, 256> gSyscallBackup;
etl::unordered_set<int, 256> gSyscallReplaced;

bool SyscallReplace(int syscallId, const void* replace, void** origp)
{
    if (gSyscallReplaced.count(syscallId))
        return false; // Already Replaced

    gSyscallReplaced.insert(syscallId);

    ScopedCR0WPDisable wpDisable;
    void* orig = const_cast<void*>((&sys_call_table)[syscallId]);

    gSyscallBackup[syscallId] = orig;
    (&sys_call_table)[syscallId] = replace;

    if (origp)
        *origp = orig;

    return true;
}

bool SyscallRestore(int syscallId)
{
    if (gSyscallBackup.find(syscallId) == gSyscallBackup.end())
        return false;

    ScopedCR0WPDisable wpDisable;

    (&sys_call_table)[syscallId] = gSyscallBackup[syscallId];
    gSyscallReplaced.erase(syscallId);

    return true;
}