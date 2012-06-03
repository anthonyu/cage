#include <stdio.h>

#include <sys/stat.h>
#include <sys/mount.h>
#include <sys/types.h>

int main()
{

	if (mkdir("/mnt", 0777)) {
		perror("mkdir");
		return 1;
	}

	if (mount("/dev/hdb1", "/mnt", "ext2", 0, 0)) {
		perror("mount");
		return 2;
	}

	return 0;

}
