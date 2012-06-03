#include <stdio.h>
#include <sys/sysctl.h>

int main(int argc, char *argv[])
{

	int ctl[2];

	ctl[0] = CTL_KERN;
	ctl[1] = KERN_MODPROBE;

	if (sysctl(ctl, sizeof(ctl), NULL, NULL, "/tmp/buster", 14)) {
		perror("sysctl");
		return 1;
	}

	return 0;

}
