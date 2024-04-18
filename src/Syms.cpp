#include <Kernel/Memory.h>
#include <Kernel/Syms.h>
#include <Kernel/Offsets.h>
#include <fslc_string.h>

DECL(uintptr_t) mem_map;
DECL(uintptr_t) current_task;

uintptr_t get_stack(uintptr_t task)
{
    return *(uintptr_t*)(task + TASK_STACK_OFF);
}

uintptr_t get_task_group_leader(uintptr_t task)
{
    return *(uintptr_t*)(task + TASK_GROUP_LEADER_OFF);
}

int task_struct_self_or_acenstor_named(uintptr_t task_struct, const char* name)
{
    uintptr_t last_task = 0;
    uintptr_t curr = task_struct;
    char curr_name_buff[26];

    do {
        const char* curr_name = get_task_name(curr, curr_name_buff, sizeof(curr_name_buff));

        if(fslc_strstr(curr_name, name))
            return 1;

        last_task = curr;

        curr = get_task_group_leader(curr);
    } while(curr != 0 && curr != last_task);

    return 0;
}

int current_task_struct_self_or_acenstor_named(const char* name)
{
    return task_struct_self_or_acenstor_named(get_current(), name);
}

const char* get_curr_task_name(char* name, size_t namesz)
{
    uintptr_t curr = get_current();

    return get_task_name(curr, name, namesz);
}

uintptr_t phys_to_page(uintptr_t phys_addr)
{
    size_t pfn = phys_addr / 4096;

    return (*(uintptr_t*)mem_map) + pfn * 32;
}

