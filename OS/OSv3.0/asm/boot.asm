	org 0x7c00

	BaseOfLoader    equ	0x7e00
	BaseOfStack 	equ 0x7c00

	jmp Label_Start
	nop
	BS_OEMName		db	'TISUboot'
	BPB_BytesPerSec	dw	512
	BPB_SecPerClus	db	1
	BPB_RsvdSecCnt	dw	1
	BPB_NumFATs		db	2
	BPB_RootEntCnt	dw	224
	BPB_TotSec16	dw	2880
	BPB_Media		db	0xf8
	BPB_FATSz16		dw	9
	BPB_SecPerTrk	dw	18
	BPB_NumHeads	dw	2
	BPB_HiddSec		dd	0
	BPB_TotSec32	dd	0
	BS_DrvNum		db	0x80
	BS_Reserved1	db	1
	BS_BootSig		db	0x29
	BS_VolID		dw	0
	BS_VolLab		db	'boot loader'
	BS_FileSysType	db	'FAT16   '
	

Label_Start:
	mov ax, cs
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov sp, 0x7c00

	; 清屏
	mov ax, 0600h
    mov bx, 0700h
    mov cx, 0
    mov dx, 0184fh
    int 10h
    mov dx, 0
    mov bx, 0
    mov ah, 2
    int 10h

    mov si, LoaderName
    mov di, BaseOfLoader

    call Search_File

    jmp BaseOfLoader

Stop:
	hlt
	jmp Stop

LoaderName db "LOADER  BIN"

[BITS 16]
%include "./asm_header/readdisk.asm"
%include "./asm_header/searchfile.asm"

times	510 - ($ - $$)	db	0
dw 0xaa55