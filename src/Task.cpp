#include <Kernel/Task.h>
#include <Kernel/Syms.h>
#include <fslc_string.h>

#ifdef BITS64
__attribute__((always_inline)) uintptr_t __readgsqword(uintptr_t offset) {
    uintptr_t value;
    asm volatile(
        "movq %1, %%rax\n"          // Move the offset to %rdi (first argument register)
        "movq %%gs:(%%rax), %0"     // Access the value at gs:offset and move it to value
        : "=r" (value)              // Output: value is written to a register
        : "r" (offset)              // Input: offset is read from a register
        : "%rax"                    // Clobbered registers
        );
    return value;
}
#endif

#ifdef BITS32
__attribute__((always_inline)) uintptr_t __readfsdword(uintptr_t offset) {
    uintptr_t value;
    asm volatile(
        "mov %1, %%eax\n"           // Move the offset to %rdi (first argument register)
        "mov %%fs:(%%eax), %0"      // Access the value at fs:offset and move it to value
        : "=r" (value)              // Output: value is written to a register
        : "r" (offset)              // Input: offset is read from a register
        : "%eax"                    // Clobbered registers
        );
    return value;
}
#endif

uintptr_t gCurrentTaskOff;

Task TaskCurrentGet()
{
    if(!gCurrentTaskOff)
        gCurrentTaskOff = uintptr_t(kallsyms_lookup_name("current_task"));

#ifdef BITS64
    return __readgsqword(gCurrentTaskOff);
#else
    return __readfsdword(gCurrentTaskOff);
#endif
}

REGPARAMDECL(char*) get_task_comm(char* comm, Task tsk);
REGPARAMDECL(char*) __get_task_comm(char* comm, size_t len, Task tsk);

TaskComm TaskCommGet(Task tsk)
{
    char result[18]{};

    if (get_task_comm)
       get_task_comm(result, tsk);
    else if (__get_task_comm)
       __get_task_comm(result, sizeof(result), tsk);

    return TaskComm(result);
}