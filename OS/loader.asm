org     0x8000
    jmp LABEL_BEGIN

%include    "pm.inc"

;GDT
;                               段基址           段界限              段属性
LABEL_GDT:          Descriptor      0,          0,                  0               ;空描述符
LABEL_DESC_CODE32:  Descriptor      0,          0xfffff,            DA_CR  | DA_32 ;非一致性代码
LABEL_DESC_VIDEO:   Descriptor      0xb8000,    0xffff,             DA_DRW            ;显存
KERNELDATA:         Descriptor      0,          0xfffff,            DA_DRW | DA_32
KERNEL:             Descriptor      0,          0xfffff,            DA_CR | DA_32 

;LABEL_DESC_DATA:    Descriptor      0, SegDataLen - 1,     DA_DRW
; GDT结束

GdtLen:     equ $ - LABEL_GDT
GdtPtr:     dw GdtLen - 1
            dd 0

;选择子
SelectorCode32          equ LABEL_DESC_CODE32   - LABEL_GDT
SelectorVideo           equ LABEL_DESC_VIDEO    - LABEL_GDT
SelectorKernelData      equ KERNELDATA          - LABEL_GDT
SelectorKernel          equ KERNEL              - LABEL_GDT

BaseOfKernelFile        equ 0x8200
BaseOfStack             equ 0x0100
BaseOfKernel            equ 0x30000
KernelEntry             equ 0x30400

[bits 16]
LABEL_BEGIN:
    mov sp, BaseOfStack
    
;存入正确的 32 位段基址
    xor eax, eax
    mov ax, cs
    shl eax, 4
    add eax, LABEL_SEG_CODE32
    mov word [LABEL_DESC_CODE32 + 2], ax
    shr eax, 16
    mov byte [LABEL_DESC_CODE32 + 4], al
    mov byte [LABEL_DESC_CODE32 + 7], ah

    ;加载 GDT
    xor eax, eax
    mov ax, ds
    shl eax, 4
    add eax, LABEL_GDT
    mov dword [GdtPtr + 2], eax

    lgdt [GdtPtr]
    
    cli

    in al, 92h
    or al, 0x02
    out 92h, al

    ;进入保护模式
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ;跳转进入保护系统
    jmp dword SelectorCode32:0

[BITS   32]
LABEL_SEG_CODE32:
    mov ax, SelectorVideo
    mov gs, ax

    mov edi, (80 * 1 + 20)
    mov ah, 0Ch
    mov al, 'P'
    mov [gs:edi], ax

    ;打印 ELF 头部

printelf:
    mov ax, SelectorKernelData
    mov ds, ax
    mov es, ax
    mov al, byte [BaseOfKernelFile + 1]
    mov ah, 0x0c
    mov edi, (80 * 1 + 20)*2
    mov [gs:edi], ax
    mov al, byte [BaseOfKernelFile + 2]
    mov ah, 0x0c
    mov edi, (80 * 2 + 21)*2
    mov [gs:edi], ax
    mov al, byte [BaseOfKernelFile + 3]
    mov ah, 0x0c
    mov edi, (80 * 3 + 22)*2
    mov [gs:edi], ax

    call InitKernel
    
    jmp KernelEntry

InitKernel:
    xor   esi, esi
    mov   cx, word [BaseOfKernelFile+2Ch];`. ecx <- pELFHdr->e_phnum
    movzx ecx, cx                               ;/
    mov   esi, [BaseOfKernelFile + 1Ch]  ; esi <- pELFHdr->e_phoff
    add   esi, BaseOfKernelFile;esi<-OffsetOfKernel+pELFHdr->e_phoff
.Begin:
    mov   eax, [esi + 0]
    cmp   eax, 0                      ; PT_NULL
    jz    .NoAction
    push  dword [esi + 010h]    ;size ;`.
    mov   eax, [esi + 04h]            ; |
    add   eax, BaseOfKernelFile; | memcpy((void*)(pPHdr->p_vaddr),
    push  eax		    ;src  ; |      uchCode + pPHdr->p_offset,
    push  dword [esi + 08h]     ;dst  ; |      pPHdr->p_filesz;
    call  MemCpy                      ; |
    add   esp, 12                     ;/
.NoAction:
    add   esi, 020h                   ; esi += pELFHdr->e_phentsize
    dec   ecx
    jnz   .Begin

    ret

loop:
    hlt
    jmp loop

tes:
    ret
;MemCpy:     ; cx 大小， esi 源， edi 目标
MemCpy:
	push	ebp
	mov	ebp, esp

	push	esi
	push	edi
	push	ecx

	mov	edi, [ebp + 8]	; Destination
	mov	esi, [ebp + 12]	; Source
	mov	ecx, [ebp + 16]	; Counter
.1:
	cmp	ecx, 0		; 判断计数器
	jz	.2		; 计数器为零时跳出

	mov	al, [ds:esi]		; ┓
	inc	esi			; ┃
					; ┣ 逐字节移动
	mov	byte [es:edi], al	; ┃
	inc	edi			; ┛

	dec	ecx		; 计数器减一
	jmp	.1		; 循环
.2:
	mov	eax, [ebp + 8]	; 返回值

	pop	ecx
	pop	edi
	pop	esi
	mov	esp, ebp
	pop	ebp

	ret			; 函数结束，返回
; MemCpy 结束-------------------------------------------------------------


SegCode32Len:   equ $ - LABEL_SEG_CODE32