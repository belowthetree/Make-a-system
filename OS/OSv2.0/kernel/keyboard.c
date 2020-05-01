#include "interrupt.h"
#include "keyboard.h"
#include "io.h"
#include "lib.h"

static int shift_l, shift_r, ctrl_l, ctrl_r, alt_l, alt_r;
static struct keyboard_inputbuffer * kbcode = 0;

void keyboard_init(){
    printf("init keyboard\n");

    kbcode = (struct keyboard_inputbuffer*)kmalloc((unsigned long)sizeof(struct keyboard_inputbuffer)
        , 0);
    kbcode->head = kbcode->buf;
    kbcode->tail = kbcode->buf;
    kbcode->count = 0;
    kbcode->end = kbcode->buf + KB_BUFF_SIZE;

    memset(kbcode->buf, 0, KB_BUFF_SIZE);

    /* 欠缺 APIC 高级中断*/

    wait_KB_write();
    io_out8(PORT_KB_CMD, KBCMD_WRITE_CMD);
    wait_KB_write();
    io_out8(PORT_KB_DATA, KB_INIT_MODE);


    unsigned long i, j;
    for (i = 0;i < 1000;i++)
        for (j = 0;j < 1000;j++)
            nop();
    
    register_irq(0x21, keyboard_handler);

    printf("finish keyboard init\n");
}


void keyboard_handler(){
    int input = io_in8(PORT_KB_DATA);

    *kbcode->head = input;
    kbcode->head++;
    kbcode->count++;
    if (kbcode->count > KB_BUFF_SIZE)
        kbcode->count = KB_BUFF_SIZE;
    if (kbcode->head == kbcode->end)
        kbcode->head = kbcode->buf;
    if (kbcode->head == kbcode->tail)
        kbcode->tail++;
    if (kbcode->tail == kbcode->end)
        kbcode->tail = kbcode->buf;
}

unsigned char get_scancode(){
    if (kbcode->count == 0)
        return NONEDOWN;

    unsigned char ret = *kbcode->tail;
    kbcode->tail++;
    if (kbcode->tail == kbcode->end)
        kbcode->tail = kbcode->buf;
    kbcode->count--;

    return ret;
}

void decode_keyboard(){
    unsigned char input = get_scancode();
    char c;
    switch(input){
        case ADOWN:
            c = 'a';
            break;
        case BDOWN:
            c = 'b';
            break;
        case CDOWN:
            c = 'c';
            break;
        case DDOWN:
            c = 'd';
            break;
        case EDOWN:
            c = 'e';
            break;
        case FDOWN:
            c = 'f';
            break;
        case GDOWN:
            c = 'g';
            break;
        case HDOWN:
            c = 'h';
            break;
        case IDOWN:
            c = 'i';
            break;
        case JDOWN:
            c = 'j';
            break;
        case KDOWN:
            c = 'k';
            break;
        case LDOWN:
            c = 'l';
            break;
        case MDOWN:
            c = 'm';
            break;
        case NDOWN:
            c = 'n';
            break;
        case ODOWN:
            c = 'o';
            break;
        case PDOWN:
            c = 'p';
            break;
        case QDOWN:
            c = 'q';
            break;
        case RDOWN:
            c = 'r';
            break;
        case SDOWN:
            c = 's';
            break;
        case TDOWN:
            c = 't';
            break;
        case UDOWN:
            c = 'u';
            break;
        case VDOWN:
            c = 'v';
            break;
        case WDOWN:
            c = 'w';
            break;
        case XDOWN:
            c = 'x';
            break;
        case YDOWN:
            c = 'y';
            break;
        case ZDOWN:
            c = 'z';
            break;
        default:
            return;
    }
    putchar(c);
}
