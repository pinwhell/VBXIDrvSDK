#pragma once

#ifdef BITS32
#include <Kernel/UniSTD32.h>
#else
#include <Kernel/UniSTD64.h>
#endif

void replace_syscall(int syscall_id, const void* _new, void** out_old);