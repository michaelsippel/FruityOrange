
; fasm demonstration of writing simple ELF executable

format ELF executable 3
entry start

segment readable executable

start:

	mov	eax,12
	mov	ebx,1
	mov	ecx,msg
	mov	edx,msg_size
	int	0x30

	xor	eax,eax
	xor	ebx,ebx
	int	0x30

segment readable writeable

msg db 'Hello world!',0xA
msg_size = $-msg

