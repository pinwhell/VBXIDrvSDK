#include <Kernel/Dirent.h>
#include <fslc_string.h>

void LinuxDirentRemove(linux_dirent* dentsp, linux_dirent* d, int* cnt_dents_bytes)
{
    uintptr_t dirent_end = (uintptr_t)dentsp + *cnt_dents_bytes;
    uintptr_t next_dirnet = (uintptr_t)d + d->d_reclen;
    
    *cnt_dents_bytes -= d->d_reclen;
    
    fslc_memcpy((void*)d, (void*)next_dirnet, (dirent_end - next_dirnet));
}

void LinuxDirent64Remove(linux_dirent64* dentsp, linux_dirent64* d, int* cnt_dents_bytes)
{
    uintptr_t dirent_end = (uintptr_t)dentsp + *cnt_dents_bytes;
    uintptr_t next_dirnet = (uintptr_t)d + d->d_reclen;
    
    *cnt_dents_bytes -= d->d_reclen;
    
    fslc_memcpy((void*)d, (void*)next_dirnet, (dirent_end - next_dirnet));
}
