#include <Kernel/Syms.h>
#include <Kernel/Syscalls.h>
#include <Kernel/Memory.h>

// void replace_syscall(int syscall_id, const void* _new, void** out_old)
// {
//     disable_write_prot();

//     if( out_old != 0)
//         *out_old = sys_call_table[syscall_id];

//     sys_call_table[syscall_id] = (void*)_new;    

//     enable_write_prot();
// }