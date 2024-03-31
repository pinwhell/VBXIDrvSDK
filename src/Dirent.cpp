#include <Kernel/Dirent.h>
#include <fslc_string.h>

void LinuxDirentRemove(linux_dirent* dentsp, linux_dirent* d, int* cnt_dents_bytes)
{
    unsigned int dirent_end = (unsigned int)dentsp + *cnt_dents_bytes;
    unsigned int next_dirnet = (unsigned int)d + d->d_reclen;
    
    *cnt_dents_bytes -= d->d_reclen;
    
    fslc_memcpy((void*)d, (void*)next_dirnet, (dirent_end - next_dirnet));
}

void LinuxDirent64Remove(linux_dirent64* dentsp, linux_dirent64* d, int* cnt_dents_bytes)
{
    unsigned int dirent_end = (unsigned int)dentsp + *cnt_dents_bytes;
    unsigned int next_dirnet = (unsigned int)d + d->d_reclen;
    
    *cnt_dents_bytes -= d->d_reclen;
    
    fslc_memcpy((void*)d, (void*)next_dirnet, (dirent_end - next_dirnet));
}
