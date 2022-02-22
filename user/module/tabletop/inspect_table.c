#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/printk.h>
#include <linux/syscalls.h>
#include <linux/fdtable.h>
#include <linux/rcupdate.h>
#include <linux/fs.h>
#include <linux/fs_struct.h>
#include <linux/dcache.h>
#include <linux/slab.h>

#define MODULE_NAME "tabletop"
#define TABLETOP_MAX_PATH_LENGTH 256

struct fd_info {
    int fd;
    unsigned int flags;
    long long pos;
    char path[TABLETOP_MAX_PATH_LENGTH];
};

extern void * fun_ptr;

long print_fd(struct task_struct *task, pid_t pid){
	struct files_struct *target_files;
	struct fdtable *files_table;
	int i=0;
	unsigned int max;

	target_files = task->files;
	if (target_files == NULL)
		return 0;

//	count = target_files->count;
//	printk(KERN_INFO "Count is %d", count.counter);

        files_table = files_fdtable(target_files);

	max = files_table->max_fds;
//	printk(KERN_INFO "max_fds is %d", max);

	printk(KERN_INFO "Open fds for %d:", pid);
	while(i<max){
		if(files_table->fd[i] != NULL)
			printk(KERN_INFO "%d", i);
		i++;
	}

	//while(files_table->fd[i] != NULL) {
	//	printk(KERN_INFO "%d", i);
	//	i++;
	//}
	return 0;

}
long fun(pid_t pid, struct fd_info *entries, int max_entries){
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
		print_fd(task, pid);
	}

	return 0;
}

//long (*fun_ptr)(pid_t, struct fd_info *, int) = &fun;

//EXPORT_SYMBOL(fun_ptr);

int simple_init(void)
{
	pr_info("Loading %s\n", MODULE_NAME);
	fun_ptr = fun;
	return 0;
}

void simple_exit(void)
{
	pr_info("Removing %s\n", MODULE_NAME);
	fun_ptr = NULL;
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION(MODULE_NAME);
MODULE_AUTHOR("cs4118");
