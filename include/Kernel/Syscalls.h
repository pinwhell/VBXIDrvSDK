#pragma once

bool SyscallReplace(int syscallId, const void* replace, void** origp);
bool SyscallRestore(int syscallId);

template<typename TReplace, typename TBackup>
class SyscallReplaceScoped {
	inline SyscallReplaceScoped(int syscallId, TReplace replace, TBackup* orig = nullptr)
		: mSyscallId(syscallId)
	{
		SyscallReplace(mSyscallId, (const void*)replace, (void**)orig);
	}

	inline ~SyscallReplaceScoped()
	{
		SyscallRestore(mSyscallId);
	}

	int mSyscallId;
};

#define SYSCALL_REPLACE(nr, with, orig) SyscallReplace((nr), (const void*)(with), (void**)(orig))
#define SYSCALL_RESTORE(nr) SyscallRestore(nr)
