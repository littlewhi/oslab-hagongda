INITSEG=0X9000
entry _start
_start:
	mov ah, #0x03
	xor bh,bh
	int 0x10

	mov cx, #22
	mov bx, #0x0007
	mov bp, #msg2
	mov ax, cs		!the address of printing
	mov es, ax
	mov ax, #0x1301
	int 0x10
	
	!set stack
	mov ax, #INITSEG
	mov ss, ax
	mov bp, 0xff00


!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

get_info:	!get the information of a disk
	mov ax, #INITSEG
	mov ds, ax		!set ds 0x9000

	!cursor
	mov ah, #0x03
	xor bh, bh
	int 0x10		!use int 0x10 to get cursor's place
	mov [0],dx		!write the place to 0x90000

	!get memory size
	mov ah, #0x88
	int 0x15
	mov [2], ax		!the size is in 0x9000:2

	!get 16 byte from 0x41 (a disk parameter table)
	mov ax, #0x0000
	mov ds, ax
	lds si, [4*0x41] ! mov ds, [4*0x41+2],mov si, [4*0x41+2]
	mov ax, #INITSEG
	mov es, ax
	mov di, #0x4
	mov cx, #0x10		!16
	cld
	rep
	movsb

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!SHOW PART!

readywork:
	mov ax, #INITSEG
	mov ds, ax
	mov ax, cs		!the address of printing
	mov es, ax

show_cur:
	call get_cur
	
	mov cx, #13
	mov bp, #msg_cursor
	call print_str

	mov dx, [0]
	call print_start

show_mem:
	call get_cur

	mov cx, #14
	mov bp, #msg_mem
	call print_str
	
	mov dx, [2]
	call print_start

show_kb:
	call get_cur

	mov cx, #2
	mov bp, #msg_kb
	call print_str

show_cyls:
	call get_cur

	mov cx, #7
	mov bp, #msg_cyls
	call print_str

	mov dx, [4]
	call print_start

show_heads:
	call get_cur

	mov cx, #8
	mov bp, #msg_heads
	call print_str
	
	mov dx, [6]
	call print_start

show_sector:
	call get_cur

	mov cx, #10
	mov bp, #msg_sector
	call print_str

	mov dx, [18]
	call print_start
	
inf_loop:
	jmp inf_loop

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!function part!

get_cur:
	mov ah, #0x03
	xor bh, bh
	int 0x10	
	ret

print_str:
	!bp is msg, cx is number of char
	mov bx, #0x0007
	mov ax, #0x1301
	int 0x10
	ret	

print_start:		!call the 'print_start' to print
	mov cx, #4		!set the number of times of loop

move_bit:
	rol dx, #4		!the high bit to low bit

print_hex:
	!print a hex number from dl's 4 low bits
	mov ah, #0xe 
	mov al, dl
	and al, #0x0f
	add al, #0x30
	cmp al, #0x3a
	jl outp		! a number less than 10
	add al, #0x07

outp:
	int 0x10
	loop move_bit
	ret

print_endl:
	!CR
	mov ax, 0xe0d
	int 0x10
	!LF
	mov al, 0xa
	int 0x10
	ret

msg2:
	.byte 13,10
	.ascii "NOW is SETUP ..."
	.byte 13,10
msg_cursor:!13
	.byte 13,10
	.ascii "Cursor POS:"
msg_mem:!14
	.byte 13,10
	.ascii "Memory SIZE:"
msg_cyls:!7
	.byte 13,10
	.ascii "Cyls:"
msg_heads:!8
	.byte 13,10
	.ascii "Heads:"
msg_sector:!10
	.byte 13,10
	.ascii "Sectors:"
msg_kb:!2
	.ascii "KB"

.org 510
boot_flag:
	.word 0xAA55
