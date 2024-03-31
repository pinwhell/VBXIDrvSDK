#pragma once

#include <Kernel/Decls.h>

struct path;

REGPARAMDECL(void*) __kmalloc(unsigned int sz, unsigned int flags);
REGPARAMDECL(char*) get_task_comm(char* out_task_comm, void* task_struct);
REGPARAMDECL(long) strncpy_from_user(char *dest, const char __user *src, long count);
REGPARAMDECL(void) kfree(const void*);
REGPARAMDECL(unsigned long)  _copy_from_user(void * to, const void __user * from, unsigned long n);
REGPARAMDECL(unsigned long)  copy_to_user(void __user * to,const void * from, unsigned long n);
REGPARAMDECL(char *)  d_path(unsigned int path, char *buf, int buflen);
REGPARAMDECL(void)  seq_puts(unsigned int m, const char* s);
REGPARAMDECL(unsigned int)  __fdget(unsigned int fd);
REGPARAMDECL(void)  fput(unsigned int file);
REGPARAMDECL(const char*)  file_path(unsigned int filp, char *buf, int buflen);
REGPARAMDECL(unsigned int)  filp_open(const char *filename, int flags, int mode);
REGPARAMDECL(void)  filp_close(unsigned int filp, int id);
REGPARAMDECL(long long)  vfs_llseek(unsigned int file, long long offset, int whence);
REGPARAMDECL(int)  vfs_unlink(unsigned int dir, unsigned int dentry, unsigned int delegated_inode);
REGPARAMDECL(unsigned int)  kernel_write(unsigned int file, const void *buf, unsigned int count,  unsigned long long pos);
REGPARAMDECL(unsigned int)  kernel_read(unsigned int file, void *buf, unsigned int count,  unsigned long long pos);
REGPARAMDECL(int)  kern_path(const char *name, unsigned int flags, path *path);
REGPARAMDECL(unsigned int)  dget_parent(unsigned int dentry);
REGPARAMDECL(void)  dput(unsigned int dentry);
REGPARAMDECL(void)  path_put(unsigned int path);
REGPARAMDECL(int)  user_path_at_empty(int dfd, const char __user *name, unsigned flags, void* path, int *empty);
DECL(int) printk(const char * format, ...);
DECL(void) dump_stack();
DECL(int) sys_getpid();
DECL(void**) sys_call_table;

#define kmalloc __kmalloc
#define copy_from_user _copy_from_user

unsigned long get_current();
unsigned long get_stack(unsigned int task);

void traverse_call_stack(int(*callback)(unsigned int ret_at, void* param), void* param);

unsigned int get_task_parent(unsigned int task);
unsigned int get_task_group_leader(unsigned int task);
const char* get_task_name(unsigned int task, char* name, size_t namesz);
int task_struct_self_or_acenstor_named(unsigned int task_struct, const char* name);
int current_task_struct_self_or_acenstor_named(const char* name);
const char* get_curr_task_name(char* name, size_t namesz);
unsigned int phys_to_page(unsigned int phys_addr);
