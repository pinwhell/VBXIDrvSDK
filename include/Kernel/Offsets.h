#pragma once

using Offset = unsigned long long;

extern Offset KERNEL_TEXT;
extern Offset TASK_STACK_OFF;
extern Offset TASK_PARENT_OFF;
extern Offset TASK_COMM_OFF;
extern Offset TASK_GROUP_LEADER_OFF;
extern Offset TASK_STACK_REGS_BASE;
extern Offset FILE_FOFFSET;
extern Offset DENTRY_PARENT;
extern Offset DENTRY_INODE;

//#define TASK_STACK_OFF 0x8
//#define STACK_REG_ENTRY 0x1fb4 //(((1 << 13) << 1) - 0x10)
//#define TASK_STRUCT_PARENT_OFF 0x39C
//#define TASK_STRUCT_GROUP_LEADER_OFF 0x3B0
//#define FILE_FOFFSET 0x40
//#define DENTRY_PARENT 0x10
//#define DENTRY_INODE 0x20