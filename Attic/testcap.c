#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#undef _POSIX_SOURCE
#include <sys/capability.h>

int main(int argc, char *argv[])
{

	cap_user_header_t head;
	cap_user_data_t data;
	char *cmd[] = { "showcap", NULL };

	
	if ((head = malloc(8)) == NULL)
		perror("malloc");

	if ((data = malloc(3 * 4)) == NULL)
		perror("malloc");

	head->pid = 0;
	head->version = _LINUX_CAPABILITY_VERSION;

	data->inheritable = 0;

	if (capset(head, data))
		perror("capset");

	execve("/usr/bin/showcap", cmd, NULL);
	perror("execve");

	return -1;

}
