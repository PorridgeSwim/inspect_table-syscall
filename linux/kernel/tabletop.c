#include <linux/pid.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/fs.h> 
#include <linux/cdev.h>
#include <linux/proc_fs.h>
#include <linux/pid_namespace.h>
#include <linux/syscalls.h>
#include <linux/uidgid.h>
#include <linux/cred.h>
#include <linux/types.h>
#include <linux/unistd.h>
#include <linux/sched.h>

#define TABLETOP_MAX_PATH_LENGTH 256

struct fd_info {
    int fd;
    unsigned int flags;
    long long pos;
    char path[TABLETOP_MAX_PATH_LENGTH];
};

long (*fun_ptr)(pid_t pid, struct fd_info *entries, int max_entries) = NULL;
EXPORT_SYMBOL(fun_ptr);

//extern long fun(pid_t pid, struct fd_info *entries, int max_entries);
//extern long (*fun_ptr)(pid_t pid, struct fd_info *entries, int max_entries);
//extern void* fun_ptr;

SYSCALL_DEFINE3(inspect_table, pid_t, pid, struct fd_info *, entries, int, max_entries)
{
	//int ret = fun(pid, entries, max_entries);
	//int ret = (*fun_ptr)(pid, entries, max_entries);
	//return ret;
	if(!fun_ptr){
		return -ENOSYS;
	}else{
		return (*fun_ptr)(pid, entries, max_entries);
	}
}
