#pragma once

#ifdef BITS32
#include <Kernel/UniSTD_x86_32.h>
#elif defined(BITS64)
#include <Kernel/UniSTD_x86_64.h>
#endif