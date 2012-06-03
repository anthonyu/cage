/* 
 * cage - makes a nice padded cell for untrusted applications.
 *
 * Copyright (C) 2001 Anthony D. Urso
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  
 *
 * $Id: cage.c,v 1.8 2001/11/19 08:08:45 anthonyu Exp $
 *
 */


#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"


#ifdef HAVE_LIBCAP
#  include <sys/capability.h>
#else
/* Thanks to Vadasz Laszlo for correcting this: */
#  include <linux/capability.h>
#  include <linux/unistd.h>
_syscall2(int, capget, cap_user_header_t, header, cap_user_data_t, data)
_syscall2(int, capset, cap_user_header_t, header, const cap_user_data_t, data)
#endif /* HAVE_LIBCAP */


static int decode_switches(int, char **);
static void die(char *);
static void usage(int);

#ifdef HAVE_LIBCAP
char *captxt;
#endif
uid_t uid, gid;


static int decode_switches(int argc, char *argv[])
{

	struct option long_options[] = {
#ifdef HAVE_LIBCAP
		{"caps", required_argument, 0, 'c'},
#endif
		{"uid", required_argument, 0, 'u'},
		{"gid", required_argument, 0, 'g'},
#if 0
		{"euid", required_argument, 0, 'U'},
		{"egid", required_argument, 0, 'G'},
#endif
		{"help", no_argument, 0, 'h'},
		{"version", no_argument, 0, 'v'},
		{NULL, 0, NULL, 0}
	};

	int c;
	unsigned long tmp;


	while ((c = getopt_long(argc, argv,
#ifdef HAVE_LIBCAP
				"c:"	/* caps to run under */
#endif
				"u:"	/* uid to run under */
				"g:"	/* gid to run under */
#if 0
				"U:"	/* euid to run under */
				"G:"	/* egid to run under */
#endif
				"h"	/* help */
				"v",	/* version */
				long_options,
				NULL)) != EOF) {
		switch(c) {
#ifdef HAVE_LIBCAP
			case 'c':
				if ((captxt = strdup(optarg)) == NULL)
					die("strdup");
				break;
#endif
			case 'u':
				tmp = strtoul(optarg, NULL, 0);
				uid = (tmp > 65535) ? 0 : (unsigned short)tmp;
				if (!uid)
					usage(-1);
				break;
			case 'g':
				tmp = strtoul(optarg, NULL, 0);
				gid = (tmp > 65535) ? 0 : (unsigned short)tmp;
				if (!gid)
					usage(-1);
				break;
#if 0
			case 'U':
				tmp = strtoul(optarg, NULL, 0);
				euid = (tmp > 65535) ? 0 : (unsigned short)tmp;
				if (!euid)
					usage(-1);
				break;
			case 'G':
				tmp = strtoul(optarg, NULL, 0);
				egid = (tmp > 65535) ? 0 : (unsigned short)tmp;
				if (!egid)
					usage(-1);
				break;
#endif
			case 'v':
				printf("cage %s\n", VERSION);
				exit(EXIT_SUCCESS);
			case 'h':
				usage(EXIT_SUCCESS);
			default:
				usage(EXIT_FAILURE);
		}
	}

	return optind;

}


static void die(char *errmsg)
{

	if (errno) {
		perror(errmsg);
		exit(errno);
	} else {
		fprintf(stderr, "%s", errmsg);
		exit(EXIT_FAILURE);
	}

}


static void usage(int status)
{

#ifdef HAVE_LIBCAP
	fprintf(stderr, "Usage: cage [-ug id] [-c <caps>] newroot command [args...]\n\n");
	fprintf(stderr, "\t-c, --caps\t\tcaps to use, instead of default\n");	
#else
	fprintf(stderr, "Usage: cage [-ug id] newroot command [args...]\n\n");
#endif
	fprintf(stderr, "\t-u, --uid\t\tuid to run under\n");	
	fprintf(stderr, "\t-g, --gid\t\tgid to run under\n");	
#if 0
	fprintf(stderr, "\t-U, --euid\t\teuid to run under\n");	
	fprintf(stderr, "\t-G, --egid\t\tegid to run under\n");	
#endif
	fprintf(stderr, "\t-h, --help\t\tdisplay this help and exit\n");
	fprintf(stderr, "\t-v, --version\t\toutput version and exit\n\n");
	
	exit(status);

}


int main(int argc, char *argv[])
{

	int num;
	char *dir, **cmd;

#ifdef HAVE_LIBCAP
	cap_t caps;
	cap_value_t capv[7];
	int flags;
#else
	cap_user_header_t head;
	cap_user_data_t data;
	__u32 mask;
#endif


	uid = 0;
	gid = 0;
#if 0
	euid = geteuid();
	egid = getegid();
#endif

	num = decode_switches(argc, argv);

	dir = argv[num++];
	cmd = argv + num;

	if (dir == NULL)
		usage(EXIT_FAILURE);

	if (chdir(dir))
		die(dir);

	if (chroot(dir))
		die("chroot");

#ifdef HAVE_LIBCAP
	if (captxt != NULL) {
		if ((caps = cap_from_text(captxt)) == NULL)
		       die("cap_from_text");
	} else {
		if ((caps = cap_get_proc()) == NULL)
			die("cap_get_proc");
 
		capv[0] = CAP_SETPCAP;	  /* give caps to non-chrooted tasks */
		capv[1] = CAP_SYS_MODULE; /* hijack syscalls like chroot() */
		capv[2] = CAP_SYS_RAWIO;  /* raw disk access (needed?) */
		capv[3] = CAP_SYS_CHROOT; /* can't have this, obviously... */
		capv[4] = CAP_SYS_PTRACE; /* control other, non-caged tasks */
		capv[5] = CAP_SYS_ADMIN;  /* mount proc and mess with kmem */
#ifdef CAP_MKNOD
		capv[6] = CAP_MKNOD;      /* create block devices */

		flags = 7;
#else
		flags = 6;
#endif /* CAP_MKNOD */

		if (cap_set_flag(caps, CAP_EFFECTIVE, flags, capv, CAP_CLEAR))
		 	die("cap_set_flag");
	
		if (cap_set_flag(caps, CAP_PERMITTED, flags, capv, CAP_CLEAR))
			die("cap_set_flag");
	
		if (cap_set_flag(caps, CAP_INHERITABLE, flags, capv, CAP_CLEAR))
			die("cap_set_flag");

	}

	if (cap_set_proc(caps))
		die("cap_set_proc");
	
	cap_free(&caps);
#else 
	if ((head = malloc(8)) == NULL)
		die("malloc()");

	if ((data = malloc(3 * 4)) == NULL)
		die("malloc()");

	head->pid = 0;
	head->version = _LINUX_CAPABILITY_VERSION;

	if (capget(head, data))
		die("capget");

 	mask  = (1 << CAP_SETPCAP);	/* give caps to non-chrooted tasks */
 	mask |= (1 << CAP_SYS_MODULE);	/* hijack syscalls such as chroot() */
 	mask |= (1 << CAP_SYS_RAWIO);	/* raw disk access (needed?) */
 	mask |= (1 << CAP_SYS_CHROOT);  /* can't have this, obviously... */
 	mask |= (1 << CAP_SYS_PTRACE);	/* control other, non-caged tasks */
 	mask |= (1 << CAP_SYS_ADMIN);   /* mount proc and mess with kmem */
#ifdef CAP_MKNOD
 	mask |= (1 << CAP_MKNOD);	/* create block devices */
#endif /* CAP_MKNOD */

	data->permitted &= ~mask;
	data->effective &= ~mask;
	data->inheritable &= ~mask;

	if (capset(head, data))
		die("capset()");

#endif /* HAVE_LIBCAP */

	if ((gid) && (setgid(gid)))
		die("setgid");

	if ((uid) && (setuid(uid)))
		die("setuid");

	execvp(cmd[0], cmd);
	
	die(cmd[0]);
}
