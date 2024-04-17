#pragma once

#include <Kernel/Decls.h>

struct path;

REGPARAMDECL(int) set_memory_x(void* addr, size_t numPages);
REGPARAMDECL(void*) __kmalloc(uintptr_t sz, uintptr_t flags);
REGPARAMDECL(long) strncpy_from_user(char *dest, const char __user *src, long count);
REGPARAMDECL(void) kfree(const void*);
REGPARAMDECL(uintptr_t)  _copy_from_user(void * to, const void __user * from, uintptr_t n);
REGPARAMDECL(uintptr_t)  copy_to_user(void __user * to,const void * from, uintptr_t n);
REGPARAMDECL(char *)  d_path(uintptr_t path, char *buf, int buflen);
REGPARAMDECL(void)  seq_puts(uintptr_t m, const char* s);
REGPARAMDECL(uintptr_t)  __fdget(uintptr_t fd);
REGPARAMDECL(void)  fput(uintptr_t file);
REGPARAMDECL(const char*)  file_path(uintptr_t filp, char *buf, int buflen);
REGPARAMDECL(uintptr_t)  filp_open(const char *filename, int flags, int mode);
REGPARAMDECL(void)  filp_close(uintptr_t filp, int id);
REGPARAMDECL(long long)  vfs_llseek(uintptr_t file, long long offset, int whence);
REGPARAMDECL(int)  vfs_unlink(uintptr_t dir, uintptr_t dentry, uintptr_t delegated_inode);
REGPARAMDECL(uintptr_t)  kernel_write(uintptr_t file, const void *buf, uintptr_t count,  uint64_t pos);
REGPARAMDECL(uintptr_t)  kernel_read(uintptr_t file, void *buf, uintptr_t count,  uint64_t pos);
REGPARAMDECL(int)  kern_path(const char *name, uintptr_t flags, path *path);
REGPARAMDECL(uintptr_t)  dget_parent(uintptr_t dentry);
REGPARAMDECL(void)  dput(uintptr_t dentry);
REGPARAMDECL(void)  path_put(uintptr_t path);
REGPARAMDECL(int)  user_path_at_empty(int dfd, const char __user *name, unsigned flags, void* path, int *empty);
DECL(int) printk(const char * format, ...);
DECL(void) dump_stack();
DECL(int) sys_getpid();
DECL(void**) sys_call_table;

#define kmalloc __kmalloc
#define copy_from_user _copy_from_user

uintptr_t get_current();
uintptr_t get_stack(uintptr_t task);

void traverse_call_stack(int(*callback)(uintptr_t ret_at, void* param), void* param);

uintptr_t get_task_parent(uintptr_t task);
uintptr_t get_task_group_leader(uintptr_t task);
const char* get_task_name(uintptr_t task, char* name, size_t namesz);
int task_struct_self_or_acenstor_named(uintptr_t task_struct, const char* name);
int current_task_struct_self_or_acenstor_named(const char* name);
const char* get_curr_task_name(char* name, size_t namesz);
uintptr_t phys_to_page(uintptr_t phys_addr);


#define KLOG_PRINT(...) printk(KERN_INFO __VA_ARGS__)
