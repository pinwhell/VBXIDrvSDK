#include <Kernel/Syms.h>

bool DriverMain(uintptr_t entry)
{
    printk(KERN_INFO "Loaded!");
    return false;
}

void DriverShootdown()
{
    printk(KERN_INFO "Unloaded!");
}