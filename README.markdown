cage
====

Creator of padded cells for untrusted Linux applications.
---------------------------------------------------

Overview:
---------

cage is a creator of unbreakable padded cells for untrusted applications. Use it when you would like to chroot a program that wants to run as root. (e.g. sendmail, sshd, etc.)

Details:
--------

Ordinarily, applications run as root have the capabilities required to break out of their chroot jail; however, before executing your command, cage chroots into the directory you specify, then drops all privileges that would allow your process to bust out.

By default, the removed privileges currently include:

CAP_SETPCAP

  Set capabilities on arbitrary processes.

CAP_SYS_MODULE

  The ability to insert loadable kernel modules that hijack systems calls.

CAP_SYS_RAWIO

  Read/write access to kernel memory, keyboard controller, I/O devices, etc.

CAP_SYS_CHROOT

  Allows a process to change its root directory, the obvious way to break chroot.

CAP_SYS_PTRACE

  Attach to, and control the execution of, arbitrary processes.

CAP_SYS_ADMIN

  Allows mounting and unmounting of filesystems, among other things.

CAP_MKNOD

  Allows a process to create device nodes.

Installation:
-------------

Installation is straightforward:

1. Untar the sources:
    `$ tar -zxf cage-0.80.tar.gz`
2. Change into the source directory:
    `$ cd cage-0.80`
3. Configure it:
    `$ ./configure`
4. Make and install the binary and man page:
    `# make install`

Then, follow [these instructions] (http://killa.net/infosec/caps/) to set up POSIX 1003.1e capabilities.

Examples:
---------

To run sendmail under /home/sendmail the following command would be used:

    cage -- /home/sendmail /usr/sbin/sendmail -bd -q15m

The `--' is only required if the command to be run has dashed arguments.

To run slapd under /home/ldap with only the capabilities required to bind to a privileged port:

    cage -c cap_net_bind_service=eip /home/ldap slapd
