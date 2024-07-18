#include <Kernel/File.h>
#include <Kernel/Syms.h>
#include <Kernel/Decls.h>
#include <Kernel/Libc.h>
#include <Kernel/Offsets.h>
#include <Kernel/KAllsyms.h>
#include <fslc_string.h>
#include <stdarg.h>

#define AT_FDCWD		-100
#define LOOKUP_FOLLOW		0x0001
#define O_RDONLY    00
#define O_WRONLY    01
#define O_RDWR      02
#define O_CREAT     0100
#define O_EXCL      0200
#define O_NOCTTY    0400
#define O_TRUNC     01000
#define O_APPEND    02000
#define O_NONBLOCK  04000
#define O_NDELAY    O_NONBLOCK
#define O_SYNC      010000
#define O_FSYNC     O_SYNC
#define O_ASYNC     020000
#define O_ACCMODE   (O_RDONLY|O_WRONLY|O_RDWR)
#define O_DSYNC     040000
#define O_RSYNC     O_SYNC
#define O_DIRECTORY 0200000
#define O_NOFOLLOW  0400000
#define O_CLOEXEC   02000000
#define O_DIRECT    040000
#define O_LARGEFILE 0100000
#define SEEK_SET    0   // Set file offset to offset
#define SEEK_CUR    1   // Set file offset to current plus offset
#define SEEK_END    2   // Set file offset to EOF plus offset

REGPARAMDECL(uintptr_t) fget(unsigned int fd);
REGPARAMDECL(void) fput(uintptr_t file);

ScopedFileFromFd::ScopedFileFromFd(int fd)
{
    mFile = fget(fd);
}

ScopedFileFromFd::~ScopedFileFromFd()
{
    if (*this)
        fput(mFile);
}

ScopedFileFromFd::operator bool() {
    return mFile != 0;
}

struct File {
    uintptr_t file;
    bool append;
};

int FileModeFlagsFromModeString(const char* mode)
{
    size_t modeLen = fslc_strlen(mode);

    if(modeLen == 0)
        return 0;

    modeLen = modeLen > 2 ? 2 : modeLen;

    if(modeLen == 1 && mode[0] == 'r')
        return O_RDONLY;

    if(modeLen == 1 && mode[0] == 'w')
        return O_WRONLY | O_APPEND;

    if(mode[0] == 'r' && mode[1] == 'w')
        return O_RDWR | O_APPEND;

    if(mode[0] == 'r')
        return O_RDONLY;

    if(mode[0] == 'w')
        return O_WRONLY | O_APPEND;

    return 0;    
}

File* FileOpen(const char* path, const char* mode, bool append, bool truncate)
{
    if(path == nullptr || mode == nullptr)
        return nullptr;

    int modeFlags = FileModeFlagsFromModeString(mode) | O_LARGEFILE;

    if(truncate == true)
        modeFlags |= O_TRUNC;


    uintptr_t file = (uintptr_t) filp_open(path, modeFlags, 0777);


    if(IS_ERR_VALUE(file) == true)
    {
        file = (uintptr_t) filp_open(path, modeFlags | O_CREAT, 0777);

        if(IS_ERR_VALUE(file) == true)
            return nullptr;
    }

    if(File* result = (File*)kmalloc(sizeof(File), GFP_KERNEL))
    {
        result->file = file;
        result->append = append;
        return result;
    }

    // At this point, 
    // we have no memory

    filp_close(file, 0);
    return nullptr;
}

size_t FileWrite(File* stream, const void *ptr, size_t size)
{
    if(stream->append == true)
        FileSeek(stream, 0, SEEK_END);

    uint64_t seek = KernelFileSeekGet(stream->file);
    uintptr_t writedBytes = kernel_write(stream->file, ptr, size, (uintptr_t) & seek);

    if(IS_ERR_VALUE(writedBytes))
        return 0;
    
    return writedBytes;
}

size_t FileRead(File* stream, void *ptr, size_t size){
    return 0; // Unimplemented
} 

void FileSeek(File* file, size_t offset, int whence)
{
    vfs_llseek(file->file, offset, whence);
}

size_t FileSeekTell(File* file)
{
    return vfs_llseek(file->file, 0, SEEK_CUR);
}

void FileRewind(File* file)
{
    vfs_llseek(file->file, 0, SEEK_SET);
}

int FilePrint(File* stream, const char *format, ...) {

    if (!stream || !format) 
        return -1; // Error: invalid stream or format string

    static auto _vsnprintf = KallsymLookupName<int (KERN_CALL*)(char*, size_t, const char*, va_list)>("vsnprintf");
    
    va_list args1;
    va_start(args1, format);
    int formatSize = _vsnprintf(NULL, 0, format, args1) + 1;
    va_end(args1);


    if (formatSize == 0)
        return 0;

    char* formatBuff = (char*)__builtin_alloca(formatSize);
    va_list args2;
    va_start(args2, format);
    size_t formattedCount = _vsnprintf(formatBuff, formatSize, format, args2);
    va_end(args2);

    if(formattedCount < 1)
        return formattedCount;

    if(FileWrite(stream, formatBuff, formattedCount) < 1)
        return -1;

    return formattedCount;
}

void FileClose(File* file){
    
    if(file == nullptr)
        return;

    filp_close(file->file, 0);

    kfree(file);
}

size_t FileSizeGet(File* file)
{
    if(file == nullptr)
        return EINVAL;

    size_t old_seek = FileSeekTell(file);

    if(IS_ERR_VALUE(old_seek))
        return old_seek;

    FileSeek(file, 0, SEEK_END);

    size_t file_size = FileSeekTell(file);
    
    if(IS_ERR_VALUE(file_size))
    {
        FileSeek(file, old_seek, SEEK_SET);
        return file_size;
    }

    FileSeek(file, old_seek, SEEK_SET);

    return file_size;
}

etl::unique_ptr<string_path> KernelFilePathGet(KernelFile file)
{
    etl::unique_ptr<char[]> pathStrg(new char[MAX_FILE_PATH]);
    memset(pathStrg.get(), 0x0, MAX_FILE_PATH);
    const char* path = file_path(file, pathStrg.get(), MAX_FILE_PATH);
    pathStrg.get()[MAX_FILE_PATH - 1] = 0;
    return etl::unique_ptr<string_path>(new string_path(path));
}

etl::unique_ptr<string_path> FdPathGet(int fd)
{
    ScopedFileFromFd sfd(fd);

    if (!sfd) return etl::unique_ptr<string_path>(new string_path(""));

    return KernelFilePathGet(sfd.mFile);
}

etl::unique_ptr<string_path> FilenameFromPathGet(const char* path)
{
    const char* lastSlash = fslc_strrstr(path, "/");

    if (lastSlash == nullptr)
        return etl::unique_ptr<string_path>(new string_path(path));

    return etl::unique_ptr<string_path>(new string_path(lastSlash + 1));
}

size_t KernelFileSeekGet(KernelFile file) {
    return *(size_t*)(file + FILE_FOFFSET);
}

void KernelFileSeekSet(KernelFile file, size_t pos) {
    *(size_t*)(file + FILE_FOFFSET) = pos;
}

int KernelFileDeleteByName(const char* name)
{
    path _path;
    int err;

    err = kern_path(name, 0, &_path);

    if (err)
        return err;

    uintptr_t dentry = _path.dentry;
    uintptr_t parent_dentry = *(uintptr_t*)(_path.dentry + DENTRY_PARENT);
    uintptr_t parent_inode = *(uintptr_t*)(parent_dentry + DENTRY_INODE);

    // Unlink (delete) the file
    err = vfs_unlink(parent_inode, _path.dentry, 0);
    path_put((uintptr_t)&_path);

    if (err)
        return err;

    return 0;
}

bool KernelFileExist(const char __user * userPath)
{
    struct path path;

    int result = user_path_at_empty(AT_FDCWD, userPath, LOOKUP_FOLLOW, &path, NULL);

    if (result)
        return false;

    path_put((uintptr_t)&path);

    return true;
}