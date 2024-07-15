#pragma once

#include <etl/string.h>

constexpr size_t MAX_FILE_PATH = 4096;

struct path {
	uintptr_t mnt;
	uintptr_t dentry;
};

struct File;
using KernelFile = uintptr_t;
using string_path = typename etl::string<MAX_FILE_PATH>;

struct ScopedFileFromFd {

	ScopedFileFromFd(int fd);
	~ScopedFileFromFd();

	operator bool();

	KernelFile mFile;
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

etl::unique_ptr<string_path> KernelFilePathGet(KernelFile file);
etl::unique_ptr<string_path> FdPathGet(int fd);
etl::unique_ptr<string_path> FilenameFromPathGet(const char* path);
size_t KernelFileSeekGet(KernelFile file);
void KernelFileSeekSet(KernelFile file, size_t pos);
int KernelFileDeleteByName(const char* name);
bool KernelFileExist(const char __user * userPath);
