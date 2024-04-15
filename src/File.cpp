#include <Kernel/File.h>
#include <Kernel/Syms.h>
#include <Kernel/Decls.h>
#include <Kernel/Libc.h>
#include <Kernel/Offs.h>
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

fd __to_fd(uintptr_t v)
{
    return { (uintptr_t)(v & ~3), v & 3 };
}

fd fdget(uintptr_t fd)
{
    return __to_fd(__fdget(fd));
}

ScopedFd::ScopedFd(int fd)
{
    mFd = fdget(fd);
}

ScopedFd::~ScopedFd()
{
    if (*this)
        fput(mFd.file);
}

ScopedFd::operator bool() {
    return mFd.file != 0;
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

    static int (KERN_CALL *_vsnprintf)(char* str, size_t size, const char* format, va_list ap) = (decltype(_vsnprintf)) kallsyms_lookup_name("vsnprintf");

    if (!stream || !format) {
        return -1; // Error: invalid stream or format string
    }

    va_list args;
    va_start(args, format);

    int formatSize = _vsnprintf(NULL, 0, format, args) + 1;

    if(formatSize == 0)
        return 0;

    char* formatBuff = (char*)__builtin_alloca(formatSize);
    size_t formattedCount = _vsnprintf(formatBuff, formatSize, format, args);

    if(formattedCount < 1)
    {
        va_end(args);
        return formattedCount;
    }

    if(FileWrite(stream, formatBuff, formattedCount) < 1)
    {
        va_end(args);
        return -1;
    }

    va_end(args);

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

string_path KernelFilePathGet(KernelFile file)
{
    string_path result;
    file_path(file, result.data(), result.size());
    return result;
}

string_path FdPathGet(int fd)
{
    ScopedFd sfd(fd);

    if (!sfd) return 0;

    return KernelFilePathGet(sfd.mFd.file);
}

string_path FilenameFromPathGet(const char* path)
{
    const char* lastSlash = fslc_strrstr(path, "/");

    if (lastSlash == nullptr)
        return path;

    return lastSlash + 1;
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

bool KernelFileExist(const char* userPath)
{
    struct path path;

    int result = user_path_at_empty(AT_FDCWD, userPath, LOOKUP_FOLLOW, &path, NULL);

    if (result)
        return false;

    path_put((uintptr_t)&path);

    return true;
}