BaseOfLoader equ 0x7e00
BaseOfKernel equ 0x100000
    org 0x7e00
	;   显示文字：Start booting...
    mov bx, 000fh
    mov dx, 0100h
    mov cx, 16
    mov ax, 1301h
    mov bp, LoaderMsg
    int 10h
    jmp Start_Loader
%include "./asm_header/pm.inc"

[SECTION gdt32]
GDTBASE32:    Descriptor    0,      0,          0
CODEBASE32:   Descriptor    0,      0xffff, DA_DPL0 | DA_CR | DA_32
DATABASE32:   Descriptor    0,      0xffff, DA_DPL0 | DA_DRW | DA_LIMIT_4K
VIDEOBASE32:  Descriptor    0xb8000,0xffff, DA_DPL0 | DA_DRW

GdtLen equ $ - GDTBASE32
GdtPtr  dw GdtLen - 1
        dd GDTBASE32

SelectorCode32 equ CODEBASE32 - GDTBASE32
SelectorData32 equ DATABASE32 - GDTBASE32

[SECTION gdt64]

GDTBASE64:      dq  0x0000000000000000
CODEBASE64:     dq  0x0020980000000000
DATABASE64:     dq  0x0000920000000000

GdtLen64    equ $ - GDTBASE64
GdtPtr64    dw GdtLen64 - 1
            dd GDTBASE64

SelectorCode64  equ CODEBASE64 - GDTBASE64
SelectorData64  equ DATABASE64 - GDTBASE64

Start_Loader:
    mov di, 0x7c00
    mov bx, 0
    mov ax, 0
    mov es, ax
    ; 获取内存信息
check_memory:
    mov ax, 0xe820
    mov cx, 20
    mov edx, 0x534d4150
    int 0x15

    mov ax, 20
    add di, ax
    cmp bx, 0
    jnz check_memory

    ; 设置显示模式
    mov ax, 0x4f02
    mov bx, 0x4180
    int 0x10
    ; 打开 A20 地址线
    xor eax, eax
    in  al, 92h
    or  al, 0x0002
    out 92, al

    cli
    ; 进入保护模式
    lgdt [GdtPtr]
    mov eax, cr0
    or  eax, 1
    mov cr0, eax

    jmp dword SelectorCode32:Start_Code32

[SECTION .s32]
[bits 32]
Start_Code32:
    mov ax, SelectorData32
    mov ds, ax
    mov fs, ax
    mov es, ax
    mov ss, ax
    mov esp, 0x7c00

    mov esi, KernelFile
    mov edi, BaseOfKernel
    call Search_File

    ; 页目录
    mov dword [0x90000], 0x91007
    mov dword [0x90800], 0x91007
    ; 二级页表
    mov dword [0x91000], 0x92007
    ; 三级，2M 页表
    mov dword [0x92000], 0x000083
    mov dword [0x92008], 0x200083
    mov dword [0x92010], 0x400083
    mov dword [0x92018], 0x600083
    mov dword [0x92020], 0x800083
    mov dword [0x92028], 0xe0000083
    mov dword [0x92030], 0xe0200083
    mov dword [0x92038], 0xe0400083

    ; 加载 64 位 GDT
    db 0x66
    lgdt [GdtPtr64]
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax
    mov esp, 0x7c00

    ; 开启 PAE
    mov eax, cr4
    bts eax, 5
    mov cr4, eax

    ; 加载页目录
    mov eax, 0x90000
    mov cr3, eax

    ; 开启长模式，ECX 选择寄存器组号，读取后放置在 EAX
    mov ecx, 0x0c0000080
    rdmsr
    bts eax, 8
    wrmsr

    ; 开启分页
    mov eax, cr0
    bts eax, 31
    mov cr0, eax


    jmp SelectorCode64:BaseOfKernel

.Stop:
    hlt
    jmp .Stop

StopLoader:
	hlt
	jmp StopLoader

LoaderMsg db "Start Loader....", 0
EnterPMMsg db "Enter Protection", 0
KernelFile db "KERNEL  BIN"

[BITS 32]
%include "./asm_header/readdisk.asm"
%include "./asm_header/searchfile2.asm"
