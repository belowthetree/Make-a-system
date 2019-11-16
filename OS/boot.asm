org 0x7c00
    jmp LABEL_BEGIN

BaseOfLoader        equ 0x8000
BaseOfStack         equ 0x0100

[BITS   16]
LABEL_BEGIN:
    mov ax, cs
    mov ds, ax
    mov es, ax
    mov sp, BaseOfStack

    xor cx, cx
    mov ax, 0x800
    mov es, ax
    mov ch, 0       ;柱面 0 
    mov dh, 0       ;磁头 0
    mov cl, 2       ;扇区 2

readloop:
    mov ah, 0
    mov dl, 0
    int 0x13
    mov ah, 0x02
    mov al, 1
    mov bx, 0
    mov dl, 0x00
    int 0x13
    jc readloop
    add cl, 1
    mov ax, es
    add ax, 0x20
    mov es, ax
    cmp cl, 18
    jae output
    jmp readloop

output:
    xor dh, 1
    cmp dh, 1
    jz noadd
    add ch, 1
noadd:
    mov cl, 1
    cmp ch, 10
    jl readloop
    mov esi, msg
    call putloop
putloop:
    mov al, [esi]
    cmp al, 0
    jz  0x8000
    mov ah, 0x0e
    int 0x10
    add esi, 1
    jmp putloop

msg:
    db "go", 0x0a, 0

times   510 - ($ - $$) db 0
dw  0xaa55