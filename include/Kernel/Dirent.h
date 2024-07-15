#pragma once

#include <cstdint>
#include <fslc_string.h>

typedef struct _linux_dirent {
    unsigned long  d_ino;     /* Inode number */
    unsigned long  d_off;     /* Offset to next linux_dirent */
    unsigned short d_reclen;  /* Length of this linux_dirent */
    char           d_name[1];  /* Filename (null-terminated) */

} linux_dirent;

typedef struct _linux_dirent64 {
    unsigned long long		d_ino;
    long long		d_off;
    unsigned short	d_reclen;
    unsigned char	d_type;
    char		d_name[1];
} linux_dirent64;

template<typename TDirent>
unsigned int LinuxDirentRemove(TDirent* pDentsStart, TDirent* pDentsEnd, TDirent* toRemove)
{
    void* nextToRemove = (char*)toRemove + toRemove->d_reclen;

    if (nextToRemove >= pDentsEnd)
        return toRemove->d_reclen;

    size_t remainingLen = ((size_t)pDentsEnd - (size_t)nextToRemove);
    size_t removedLen = toRemove->d_reclen;

    fslc_memcpy((void*)toRemove, (void*)nextToRemove, remainingLen);

    return removedLen;
}