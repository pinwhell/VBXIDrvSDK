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

Task ProcessCurrentGet()
{
    return TaskGroupLeaderGet(TaskCurrentGet());
}

REGPARAMDECL(Task) find_task_by_vpid(size_t vpid);

Task TaskByPidGet(size_t vpid)
{
    return find_task_by_vpid(vpid);
}

Task TaskGroupLeaderGet(Task tsk)
{
    return *(Task*)(tsk + TASK_GROUP_LEADER_OFF);
}

Task TaskParentGet(Task tsk)
{
    return *(Task*)(tsk + TASK_REAL_PARENT_OFF + sizeof(uintptr_t));
}

Task TaskGroupLeaderParentGet(Task tsk)
{
    tsk = TaskGroupLeaderGet(tsk); // Normalizing to group leader
    tsk = TaskParentGet(tsk); // Falling back to creator of this task
    return TaskGroupLeaderGet(tsk); // Finally, Normalizing to group leader of the creator task
}

void TaskForEachProcessAncestor(Task tsk, etl::delegate<bool(Task)> callback)
{
    tsk = TaskGroupLeaderGet(tsk); // Normalizing to group leader

    for (Task curr = TaskGroupLeaderParentGet(tsk), last = tsk; curr != last; last = curr, curr = TaskGroupLeaderParentGet(curr))
    {
        if (!callback(curr))
            break;
    }
}

void TaskForEachTaskAncestor(Task tsk, etl::delegate<bool(Task)> callback)
{
    for (Task curr = TaskParentGet(tsk), last = tsk; curr != last; last = curr, curr = TaskParentGet(curr))
    {
        if (!callback(curr))
            break;
    }
}

TaskComm TaskCommGet(Task tsk)
{
    char result[17]{};

    memcpy(result, (char*)tsk + TASK_COMM_OFF, sizeof(result) - 1);

    return TaskComm(result);
}