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

SYSCALL_DEFINE3(inspect_table, pid_t, pid, struct fd_info *, entries, int, max_entries)
{
	struct task_struct *task;
	struct pid *pid_struct;
	uid_t t_uid;
	uid_t c_euid;

	if (pid < -1){
		return -EINVAL;
	} else if (pid == -1){
	//	pid_struct = find_get_pid(task_pid_nr(current));
	//	task = get_pid_task(pid_struct, PIDTYPE_PID);
		task = current;
	} else{
		if(!(pid_struct = find_get_pid(pid))){
				return -ESRCH;
		}
	//	if (!pid_has_task(pid_struct, PIDTYPE_PID)){
	//		return -ESRCH;
	//	}
		task = get_pid_task(pid_struct, PIDTYPE_PID);
	       	t_uid = task->cred->uid.val;		// uid of target process
		c_euid = current->cred->euid.val;	// euid of calling process
		if (c_euid != 0 && c_euid != t_uid){
			return -EPERM;
		}
	}

	return 0;

}
