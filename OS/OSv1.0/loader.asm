org     0x8000
    jmp LABEL_BEGIN
    
%include    "pm.inc"

;GDT
;                               段基址           段界限              段属性
LABEL_GDT:          Descriptor      0,          0,                  0               ;空描述符
LABEL_DESC_CODE32:  Descriptor      0,          0xfffff,            DA_CR | DA_DPL0 | DA_32 | DA_LIMIT_4K ;非一致性代码
LABEL_DESC_VIDEO:   Descriptor      0xb8000,    0xffff,             DA_DRW            ;显存
KERNELDATA:         Descriptor      0,          0xfffff,            DA_DRW | DA_DPL0 | DA_32 | DA_LIMIT_4K
KERNEL:             Descriptor   BaseOfKernel,  0xfffff,            DA_CR | DA_32 | DA_LIMIT_4K

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
TmpStack                equ 0x03ff
BaseOfStack             equ 0x03ff
BaseOfKernel            equ 0x30000
KernelEntry             equ 0x30400

[bits 16]
LABEL_BEGIN:
    mov sp, BaseOfStack

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

    ;跳转进入保护模式
    jmp dword SelectorCode32:LABEL_SEG_CODE32

[BITS   32]
LABEL_SEG_CODE32:
    xor ax, ax
    mov ax, SelectorVideo
    mov gs, ax
    xor ax, ax
    mov ax, SelectorKernelData
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax


    mov edi, (80 * 1 + 20)
    mov ah, 0Ch
    mov al, 'P'
    mov [gs:edi], ax



    call InitKernel
    
    jmp KernelEntry

;   从 0x8000 开始到 0x30000 共占用320个扇区
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