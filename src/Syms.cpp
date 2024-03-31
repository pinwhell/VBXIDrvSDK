#include <Kernel/Memory.h>
#include <Kernel/Syms.h>
#include <Kernel/Offs.h>
#include <fslc_string.h>

DECL(unsigned int) mem_map;
DECL(unsigned long) current_task;

__attribute__((always_inline)) unsigned long readfsdword(unsigned long Offset) {
    unsigned long value;
    asm volatile("mov %%fs:%1, %0" : "=r" (value) : "m" (*(unsigned long*)Offset));
    return value;
}

unsigned int get_task_parent(unsigned int task)
{
    return *(unsigned int*)(task + TASK_STRUCT_PARENT_OFF);
}

unsigned long get_current()
{
    return readfsdword(current_task);
}

unsigned long get_stack(unsigned int task)
{
    return *(unsigned long*)(task + TASK_STACK_OFF);
}

// #define MAX_CALL_STACK 10

// void traverse_call_stack(int(*callback)(unsigned int ret_at, void* param), void* param)
// {
//     struct pt_regs* regs = get_curr_pt_regs();

//     unsigned int frame_ptr = regs->bp;
//     unsigned int ret_addr = regs->ip;

//     if(callback(ret_addr, param) == 0)
//         return;

//     if(frame_ptr == 0)
//         return;

//     int count = 0;

//     do {
//         if(frame_ptr == 0)
//             break;

//         ret_addr = read_int_from_user(frame_ptr + 4);

//         if(ret_addr == 0)
//             break;

//         if(callback(ret_addr, param) == 0)
//             return;

//     } while((frame_ptr = read_int_from_user(frame_ptr)) != 0 && count++ < MAX_CALL_STACK);
// }

unsigned int get_task_group_leader(unsigned int task)
{
    return *(unsigned int*)(task + TASK_STRUCT_GROUP_LEADER_OFF);
}

const char* get_task_name(unsigned int task, char* name, size_t namesz)
{
    name[0] = '\0';

    if(task == 0)
        goto RET;

    if(namesz < 16)
        goto RET;

    get_task_comm(name, (void*)task);
    
    RET:
    return name;
}

int task_struct_self_or_acenstor_named(unsigned int task_struct, const char* name)
{
    unsigned int last_task = 0;
    unsigned int curr = task_struct;
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
    unsigned int curr = get_current();

    return get_task_name(curr, name, namesz);
}

unsigned int phys_to_page(unsigned int phys_addr)
{
    unsigned int pfn = phys_addr / 4096;

    return (*(unsigned int*)mem_map) + pfn * 32;
}

