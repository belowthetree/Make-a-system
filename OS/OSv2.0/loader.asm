org     10000h
BaseOfKernelFile        equ     0x00
OffsetOfKernelFile      equ     0x100000

BaseTmpOfKernelAddr     equ     0x00
OffsetTmpOfKernelFile   equ     0x7e00

MemoryStructBufferAddr  equ     0x7e00
    
    
    jmp     Label_Start
%include "./asm_module/FAT_head.inc"
%include "./asm_module/pm.inc"
[SECTION gdt]
;                               段基址           段界限              段属性
LABEL_GDT:          Descriptor      0,          0,                  0               ;空描述符
LABEL_DESC_CODE32:  Descriptor      0,          0xfffff,            DA_CR | DA_DPL0 | DA_32 | DA_LIMIT_4K ;非一致性代码
LABEL_DESC_DATA32:  Descriptor      0,          0xfffff,            DA_DPL0 | DA_DRW | DA_LIMIT_4K
LABEL_DESC_VIDEO:   Descriptor      0xb8000,    0xffff,             DA_DRW            ;显存

[SECTION idt]
IDT:
    times   0x50    dq  0
IDT_END:

GdtLen	equ	$ - LABEL_GDT
GdtPtr	dw	GdtLen - 1
	    dd	LABEL_GDT	;be carefull the address(after use org)!!!!!!

IDT_POINTER:
        dw  IDT_END - IDT - 1
        dd  IDT

SelectorCode32	equ	LABEL_DESC_CODE32 - LABEL_GDT
SelectorData32	equ	LABEL_DESC_DATA32 - LABEL_GDT

[SECTION gdt64]

LABEL_GDT64:		dq	0x0000000000000000
LABEL_DESC_CODE64:	dq	0x0020980000000000
LABEL_DESC_DATA64:	dq	0x0000920000000000

GdtLen64	equ	$ - LABEL_GDT64
GdtPtr64	dw	GdtLen64 - 1
		    dd	LABEL_GDT64

SelectorCode64	equ	LABEL_DESC_CODE64 - LABEL_GDT64
SelectorData64	equ	LABEL_DESC_DATA64 - LABEL_GDT64

[SECTION .s16]
[BITS 16]

Label_Start:
    mov     ax, cs
    mov     ds, ax
    mov     es, ax
    mov     ax, 0x1000
    mov     ss, ax
    mov     sp, 0x7c00
;   显示欢迎
    mov     ax, 1301h
    mov     bx, 000fh
    mov     dx, 0200h
    mov     cx, 14
    push    ax
    mov     ax, ds
    mov     es, ax
    pop     ax
    mov     bp, StartLoaderMessage
    int     10h
;   打开 A20 地址线
    xor     eax, eax
    in      al, 92h
    or      al, 00000010b
    out     92h, al

    cli
;   加载 gdt 并进入实模式
    db  0x66
    lgdt    [GdtPtr]
    mov     eax, cr0
    or      eax, 1
    mov     cr0, eax

;   加载选择子, 然后跳回实模式
    mov     ax, SelectorData32
    mov     fs, ax
    mov     eax, cr0
    and     al, 11111110b
    mov     cr0, eax

    sti

;   搜索内核文件
    %include "./asm_module/search_kernel.inc"
;   获取获取内存信息
    %include "./asm_module/get_memory_info.inc"

;   开始进入保护模式
    cli

    db 0x66
    lgdt    [GdtPtr]
    mov     eax, cr0
    or      eax, 1
    mov     cr0, eax

    jmp     SelectorCode32:Label_Protect

Stop:
    hlt
    jmp Stop

[SECTION .s32]
[BITS 32]
Label_Protect:




;   临时变量
SectorNumber            dw  0
RootDirSizeForLoop	    dw	RootDirSectors
Odd                     db  0
KernelFileName          db  "KERNEL  BIN",0
OffsetOfKernelFileCount dd  OffsetOfKernelFile
DisplayPosition		    dd	0

NoKernelMessage:                db  "No Kernel File",0
StartLoaderMessage:             db  "Come in Loader"

StartGetMemStructMessage:	    db	"Start Get Memory Struct."
GetMemStructErrMessage:	        db	"Get Memory Struct ERROR"
GetMemStructOKMessage:	        db	"Get Memory Struct SUCCESSFUL!"

GetSVGAVBEInfoErrMessage:	db	"Get SVGA VBE Info ERROR"
GetSVGAVBEInfoOKMessage:	db	"Get SVGA VBE Info SUCCESSFUL!"
StartGetSVGAVBEInfoMessage:	    db	"Start Get SVGA VBE Info"

StartGetSVGAModeInfoMessage:	db	"Start Get SVGA Mode Info"
GetSVGAModeInfoErrMessage:	    db	"Get SVGA Mode Info ERROR"
GetSVGAModeInfoOKMessage:	    db	"Get SVGA Mode Info SUCCESSFUL!"
