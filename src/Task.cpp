#include <Kernel/Task.h>
#include <Kernel/Syms.h>
#include <fslc_string.h>
#include <Kernel/KAllsyms.h>
#include <Kernel/Offsets.h>

#ifdef BITS32
#define AX_REG "eax"
#define TLS_REG "fs"
#elif BITS64
#define AX_REG "rax"
#define TLS_REG "gs"
#endif

__attribute__((always_inline)) uintptr_t __readtlsword(uintptr_t offset) {
    uintptr_t value;
    asm volatile(
        "movq %1, %%" AX_REG "\n"          // Move the offset to %rdi (first argument register)
        "movq %%" TLS_REG ":(%%" AX_REG "), %0"     // Access the value at gs:offset and move it to value
        : "=r" (value)              // Output: value is written to a register
        : "r" (offset)              // Input: offset is read from a register
        : "%" AX_REG                    // Clobbered registers
        );
    return value;
}



Task TaskCurrentGet()
{
    static uintptr_t gCurrentTaskOff = KallsymLookupName<uintptr_t>("current_task");

    return __readtlsword(gCurrentTaskOff);
}

REGPARAMDECL(Task) find_task_by_vpid(size_t vpid);

Task TaskByPidGet(size_t vpid)
{
    if (find_task_by_vpid)
        return find_task_by_vpid(vpid);

    return 0;
}

REGPARAMDECL(size_t)  __x64_sys_getppid();

Task TaskCurrentGetParent()
{
    if (__x64_sys_getppid)
    {
        size_t ppid = __x64_sys_getppid();
        return TaskByPidGet(ppid);
    }

    return 0;
}

//void TaskForEachParent(Task tsk, etl::delegate<bool(Task tsk)> callback)
//{
//    for(Task t = tsk;; tsk = )
//}

REGPARAMDECL(char*) get_task_comm(char* comm, Task tsk);
REGPARAMDECL(char*) __get_task_comm(char* comm, size_t len, Task tsk);

TaskComm TaskCommGet(Task tsk)
{
    char result[17]{};

    memcpy(result, (char*)tsk + TASK_COMM_OFF, sizeof(result) - 1);

    //if (get_task_comm)
    //   get_task_comm(result, tsk);
    //else if (__get_task_comm)
    //   __get_task_comm(result, sizeof(result), tsk);

    return TaskComm(result);
}