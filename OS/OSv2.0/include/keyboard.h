#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KB_BUFF_SIZE 10
#define PORT_KB_DATA 0x60
#define PORT_KB_STATUS 0x64
#define PORT_KB_CMD 0x64
#define KBCMD_WRITE_CMD 0x60
#define KBCMD_READ_CMD 0x20

#define KB_INIT_MODE 0x47

struct keyboard_inputbuffer
{
    unsigned char* head;
    unsigned char* tail;
    int count;
    unsigned char buf[KB_BUFF_SIZE];
    unsigned char* end;
};

#define KBSTATUS_IBF 0x02
#define KBSTATUS_OBF 0x01

#define wait_KB_write() while(io_in8(PORT_KB_STATUS) & KBSTATUS_IBF)
#define wait_KB_read() while(io_in8(PORT_KB_STATUS) & KBSTATUS_OBF)

void keyboard_init();
void keyboard_handler();
void mouse_handler();
unsigned char get_scancode();
void decode_keyboard();





// 键盘编码
enum keyboardmap
{
	NONEDOWN,
	ESCDOWN,
	ONEDOWN,
	TWODOWN,
	THREEDOWN,
	FOURDOWN,
	FIVEDOWN,
	SIXDOWN,
	SEVENDOWN,
	EIGHTDOWN,
	NINEDOWN,
	ZERODOWN,
	MINUSDOWN,
	EQUALDOWN,
	BACKSPACDOWNE,
	TABDOWN,
	QDOWN,
	WDOWN,
	EDOWN,
	RDOWN,
	TDOWN,
	YDOWN,
	UDOWN,
	IDOWN,
	ODOWN,
	PDOWN,
	LSBDOWN,
	RSBDOWN,
	ENTERDOWN,
	LCTRLDOWN,
	ADOWN,
	SDOWN,
	DDOWN,
	FDOWN,
	GDOWN,
	HDOWN,
	JDOWN,
	KDOWN,
	LDOWN,
	COLONDOWN,
	SINGLEPDOWN,
	POINTDOWN,
	LSHIFTDOWN,
	BACKSLASHDOWN,
	ZDOWN,
	XDOWN,
	CDOWN,
	VDOWN,
	BDOWN,
	NDOWN,
	MDOWN,
};






#endif
