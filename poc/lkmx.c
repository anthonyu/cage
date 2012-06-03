#define MODULE
#define __KERNEL__

#include <linux/capability.h>
#include <linux/module.h>
#include <linux/sched.h>

#include <sys/syscall.h>



extern void *sys_call_table[];
int (*chroot_old)(char *);

int chroot_new(char *path)
{

	cap_raise(current->cap_permitted, CAP_SYS_CHROOT);
	cap_raise(current->cap_effective, CAP_SYS_CHROOT);
	return (*chroot_old)(path);

}


int init_module(void)
{

	chroot_old = sys_call_table[SYS_chroot];
	sys_call_table[SYS_chroot] = (void *)chroot_new;

	return 0;

}


void cleanup_module(void)
{

	sys_call_table[SYS_chroot] = chroot_old;

}
