global  _start
global  io_hlt, io_sti, io_out8, io_cli, io_delay, io_load_eflags, io_store_eflags
global  load_idtr, load_gdtr
global  memcpy, register_clock, create_int

extern  main, Clock, printi, prints

[section .text]
_start:
    call main

    jmp $

io_hlt:
    hlt
    ret

memcpy:     ; void memcpy(char *str, short value, int len)
    xor eax, eax
    xor edi, edi
    xor ecx, ecx
    mov edi, [esp + 4]
    mov ax,  word [esp + 8]
    mov ecx, [esp + 12]
.membg:
    mov [edi], ax
    inc edi
    dec ecx
    jnz .membg
    ret

io_out8:      ; void io_out8(int port, int data);
    mov edx, [esp + 4]
    mov al, [esp + 8]
    out dx, al
    ret

io_cli:      ; void io_cli()
    cli
    ret

io_sti:
    sti
    ret

io_load_eflags:      ; int io_load_eflags()
    pushfd
    pop eax
    ret

io_store_eflags:      ; void io_store_eflags()
    mov     eax, [esp + 4]
    push    eax
    popfd
    ret

io_delay:
    nop
    nop
    nop
    nop
    ret

load_idtr:  ; void load_idtr(int limit, int addr)
    mov     ax, [esp + 4]
    mov     [esp + 6], ax
    lidt    [esp + 6]
    ret

load_gdtr:      ; void load_gdtr(int limit, int addr)
    mov     ax, [esp + 4]
    mov     [esp + 6], ax
    lgdt    [eax + 6]
    ret

create_int:
    int 0x80
    ret

register_clock:
    call Clock
    iretd


[section .data]
welcome:
    db "Here in kernel", 0