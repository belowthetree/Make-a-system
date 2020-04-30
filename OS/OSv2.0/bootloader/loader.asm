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


GdtLen	equ	$ - LABEL_GDT
GdtPtr	dw	GdtLen - 1
	    dd	LABEL_GDT	;be carefull the address(after use org)!!!!!!

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
;   加载 gdt 并进入保护模式
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
    ;=======	设置 SVGA 模式(VESA VBE)
Setup_SVGA:
	mov	ax,	4F02h
	mov	bx,	4180h	;========================mode : 0x180 or 0x143
	int 	10h

	cmp	ax,	004Fh
	jnz	Label_SET_SVGA_Mode_VESA_VBE_FAIL

;   开始进入保护模式
Start_To_Protect:
    cli

    db 0x66
    lgdt    [GdtPtr]
    mov     eax, cr0
    or      eax, 1
    mov     cr0, eax

    jmp     dword SelectorCode32:Label_Protect

Stop:
    hlt
    jmp Stop

[SECTION .s32]
[BITS 32]
Label_Protect:
    mov     ax, SelectorData32
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     ss, ax
    mov     esp, 0x7e00
    
    call    support_long_mode
    test    eax, eax

    jz      no_support
;   配置分页 1 0000 0000
	mov	dword	[0x90000],	0x91007
	mov	dword	[0x90800],	0x91007		

	mov	dword	[0x91000],	0x92007

	mov	dword	[0x92000],	0x000083
	mov	dword	[0x92008],	0x200083
	mov	dword	[0x92010],	0x400083
	mov	dword	[0x92018],	0x600083
	mov	dword	[0x92020],	0x800083
	mov	dword	[0x92028],	0xa00083
    
;   加载 64 位 GDT

	db	0x66
	lgdt	[GdtPtr64]
	mov	ax,	0x10
	mov	ds,	ax
	mov	es,	ax
	mov	fs,	ax
	mov	gs,	ax
	mov	ss,	ax

	mov	esp,	7E00h

;=======	开启PAE
	mov	eax,	cr4
	bts	eax,	5
	mov	cr4,	eax

;=======	加载页目录
	mov	eax,	0x90000
	mov	cr3,	eax

;=======	开启长模式
	mov	ecx,	0C0000080h		;IA32_EFER
	rdmsr

	bts	eax,	8
	wrmsr

;=======	使第 1 位和 32 位置 1，开启分页和保护模式
	mov	eax,	cr0
	bts	eax,	0
	bts	eax,	31
	mov	cr0,	eax

	jmp	SelectorCode64:OffsetOfKernelFile

support_long_mode:
    mov     eax, 0x80000000
    cpuid
    cmp     eax, 0x80000001
    setnb   al
    jb  support_long_mode_done
    mov     eax, 0x80000001
    cpuid
    bt      edx, 29
    setc    al

support_long_mode_done:
    movzx   eax, al
    ret

no_support:
    jmp $


[SECTION idt]
IDT:
    times   0x50    dq  0
IDT_END:

IDT_POINTER:
        dw  IDT_END - IDT - 1
        dd  IDT

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
