global  _start
global  io_hlt

extern  main

[section .text]
_start:
    mov esi, welcome
    cld
    call display
    call main

loop:
    hlt
    jmp loop
display:
    mov edi, 80 * 4
.begin:
    lodsb
    cmp al, 0
    jz  .enddis
    add edi, 2
    mov ah, 0Ch
    mov [gs:edi], ax
    
    jmp .begin
.enddis:
    ret

io_hlt:
    hlt
    ret


[section .data]
welcome:
    db "Here in kernel", 0