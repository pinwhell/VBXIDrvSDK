#pragma once

#include <Kernel/Decls.h>

void LinuxDirentRemove(linux_dirent* dentsp, linux_dirent* d, int* cnt_dents_bytes);
void LinuxDirent64Remove(linux_dirent64* dentsp, linux_dirent64* d, int* cnt_dents_bytes);