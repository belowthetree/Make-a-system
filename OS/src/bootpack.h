struct BOOTINFO{
    char cyls;
    char leds;
    char vmode;
    char reserve;
    short scrnx, scrny;
    char *vram;
};
#define ADR_BOOTINFO    0x00000ff0

void io_hlt();
void io_cli();
void io_sti();
void io_stihlt();
int io_in8(int port);
void io_out8(int port, int data);
int io_load_eflags();
