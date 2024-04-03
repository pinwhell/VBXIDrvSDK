#pragma once

#include <etl/string.h>

constexpr size_t MAX_FILE_PATH = 4096;

struct fd {
	uintptr_t file;
	uintptr_t flags;
};

struct path {
	uintptr_t mnt;
	uintptr_t dentry;
};

fd fdget(uintptr_t fd);

struct File;
using KernelFile = uintptr_t;
using string_path = etl::string<MAX_FILE_PATH>;

struct ScopedFd {

	ScopedFd(int fd);
	~ScopedFd();

	operator bool();

	struct fd mFd;
};

File* FileOpen(const char* path, const char* mode, bool append = false, bool truncate = false);
size_t FileWrite(File* stream, const void *ptr, size_t size);
size_t FileRead(File* stream, void *ptr, size_t size);
int FilePrint(File* stream, const char *format, ...);
void FileSeek(File* file, size_t offset, int whence);
size_t FileSeekTell(File* file);
void FileRewind(File* file);
void FileClose(File* file);
size_t FileSizeGet(File* file);

string_path KernelFilePathGet(KernelFile file);
string_path FdPathGet(int fd);
string_path FilenameFromPathGet(const char* path);
size_t KernelFileSeekGet(KernelFile file);
void KernelFileSeekSet(KernelFile file, size_t pos);
int KernelFileDeleteByName(const char* name);
bool KernelFileExist(const char* userPath);
