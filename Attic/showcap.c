#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>

#undef _POSIX_SOURCE
#include <sys/capability.h>

int main(int argc, char *argv[])
{

	pid_t pid;
	cap_t caps;
	char *text;

	if (argc > 1)
		pid = strtoul(argv[1], NULL, 0);
	else
		pid = 0;

	caps = cap_init();

	if (capgetp(pid, caps)) {
		perror("capgetp");
		return 1;
	}

	if ((text = cap_to_text(caps, NULL)) == NULL) {
		perror("cap_to_text");
		return 1;
	}

	printf("%s\n", text);

	return 0;

}
