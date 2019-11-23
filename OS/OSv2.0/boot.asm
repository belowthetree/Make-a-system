org 0x7c00

BaseOfStack     equ     0x7c00
BaseOfLoader    equ     0x1000
OffsetOfLoader  equ     0x00

;   根目录簇长度，根目录起始簇，FAT表项起始簇，起始扇区 -2

    jmp	short Label_Start
    nop
	%include "./include/FAT_head.inc"

Label_Start:
    mov ax, cs
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov sp, BaseOfStack

;   清屏
    mov ax, 0600h
    mov bx, 0700h
    mov cx, 0
    mov dx, 0184fh
    int 10h

;   设置焦点
    mov ax, 0200h
    mov bx, 0000h
    mov dx, 0000h
    int 10h
;   显示文字：Start booting...
    mov ax, 1301h
    mov bx, 000fh
    mov dx, 0000h
    mov cx, 10
    push ax
    mov ax, ds
    mov es, ax
    pop ax
    mov bp, StartBootMessage
    int 10h

;=======	reset floppy

	xor	ah,	ah
	xor	dl,	dl
	int	13h

;   搜索并加载内核文件
    %include "./include/search_loader.inc"

Stop:
    hlt
    jmp Stop

StartBootMessage:	db	"Start Boot...."
NoLoaderMessage:	db	"ERROR:No LOADER Found"
LoaderFileName:		db	"LOADER  BIN",0

;   临时变量位置
SectorNumber        dw  0
RootDirSizeForLoop	dw	RootDirSectors
Odd                 db 0

;=======	fill zero until whole sector

	times	510 - ($ - $$)	db	0
	dw	0xaa55
