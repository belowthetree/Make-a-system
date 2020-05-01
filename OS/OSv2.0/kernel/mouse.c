#include "mouse.h"
#include "keyboard.h"
#include "lib.h"
#include "io.h"
#include "graph.h"

static struct keyboard_inputbuffer * mcode = 0;
static struct MOUSE_BUF * mouse = 0;
static unsigned long pointer_cache[32][16];

static unsigned long pointer_point[32][16] = {
	{0x0,0x0,EDGECOLOR,EDGECOLOR,EDGECOLOR,0x0,0x0,0x0, 0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,},
	{0x0,0x0,EDGECOLOR,EDGECOLOR,EDGECOLOR,EDGECOLOR,0x0,0x0, 0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,},
	{0x0,0x0,EDGECOLOR,EDGECOLOR,0x0,EDGECOLOR,EDGECOLOR,0x0, 0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,},
	{0x0,0x0,EDGECOLOR,EDGECOLOR,0x0,0x0,EDGECOLOR,EDGECOLOR, 0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,},
	{0x0,0x0,EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,EDGECOLOR, EDGECOLOR,0x0,0x0,0x0,0x0,0x0,0x0,0x0,},
	{0x0,0x0,EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,EDGECOLOR, EDGECOLOR,0x0,0x0,0x0,0x0,0x0,0x0,0x0,},
	{0x0,0x0,EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,0x0, EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,0x0,0x0,0x0,},
	{0x0,0x0,EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,0x0, EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,0x0,0x0,0x0,},
	{0x0,0x0,EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,0x0, 0x0,EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,0x0,0x0,},
	{0x0,EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,0x0,0x0, 0x0,EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,0x0,0x0,},
	{0x0,EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,0x0,0x0, 0x0,0x0,EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,0x0,},
	{0x0,EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,0x0,0x0, 0x0,0x0,EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,0x0,},
	{0x0,EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,0x0,0x0, 0x0,0x0,0x0,EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,},
	{0x0,EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,0x0,0x0, 0x0,0x0,0x0,EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,},
	{0x0,EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,0x0,0x0, 0x0,0x0,0x0,0x0,EDGECOLOR,EDGECOLOR,0x0,0x0,},
	{0x0,EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,0x0,0x0, 0x0,0x0,0x0,0x0,EDGECOLOR,EDGECOLOR,0x0,0x0,},

	{0x0,EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,0x0,0x0, 0x0,0x0,0x0,0x0,EDGECOLOR,EDGECOLOR,0x0,0x0,},
	{0x0,EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,0x0,0x0, 0x0,0x0,0x0,0x0,0x0,EDGECOLOR,EDGECOLOR,0x0,},
	{EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,0x0,0x0,0x0, 0x0,0x0,0x0,0x0,0x0,EDGECOLOR,EDGECOLOR,0x0,},
	{EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,0x0,0x0,0x0, 0x0,0x0,0x0,0x0,0x0,EDGECOLOR,EDGECOLOR,EDGECOLOR,},
	{EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,0x0,0x0,0x0, 0x0,0x0,0x0,0x0,0x0,EDGECOLOR,EDGECOLOR,EDGECOLOR,},
	{EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,0x0,0x0,0x0, 0x0,0x0,0x0,0x0,0x0,EDGECOLOR,EDGECOLOR,EDGECOLOR,},
	{EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,0x0,0x0,0x0, 0x0,0x0,0x0,0x0,0x0,EDGECOLOR,EDGECOLOR,EDGECOLOR,},
	{EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,0x0,0x0,0x0, 0x0,0x0,0x0,0x0,0x0,EDGECOLOR,EDGECOLOR,0x0,},
	{EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,0x0,0x0,0x0, 0x0,0x0,0x0,0x0,0x0,EDGECOLOR,EDGECOLOR,0x0,},
	{0x0,EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,0x0,0x0, 0x0,0x0,0x0,0x0,0x0,EDGECOLOR,EDGECOLOR,0x0,},
	{0x0,0x0,EDGECOLOR,EDGECOLOR,EDGECOLOR,0x0,0x0,0x0, 0x0,0x0,0x0,EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,},
	{0x0,0x0,0x0,EDGECOLOR,EDGECOLOR,EDGECOLOR,0x0,0x0, 0x0,0x0,EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,0x0,},
	{0x0,0x0,0x0,0x0,0x0,EDGECOLOR,EDGECOLOR,EDGECOLOR, 0x0,0x0,EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,0x0,},
	{0x0,0x0,0x0,0x0,0x0,0x0,EDGECOLOR,EDGECOLOR, EDGECOLOR,0x0,EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,0x0,},
	{0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0, EDGECOLOR,EDGECOLOR,EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,0x0,},
	{0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0, 0x0,0x0,EDGECOLOR,EDGECOLOR,EDGECOLOR,0x0,0x0,0x0,}
};

void mouse_init(){
	printf("init mouse\n");

	// 鼠标输入缓存
	mcode = (struct keyboard_inputbuffer*)kmalloc((unsigned long)sizeof(struct keyboard_inputbuffer),
		0);
	mcode->head = mcode->buf;
	mcode->tail = mcode->buf;
	mcode->count = 0;
	mcode->end = mcode->buf + KB_BUFF_SIZE;

	// 鼠标输入字节信息
	mouse = (struct MOUSE_BUF*)kmalloc((unsigned long)sizeof(struct MOUSE_BUF), 0);

	register_irq(0x2c, mouse_handler);

	wait_KB_write();
	io_out8(PORT_KB_CMD,KBCMD_EN_MOUSE_INTFACE);

	int i, j;
	for(i = 0;i<1000;i++)
		for(j = 0;j<1000;j++)
			nop();

	wait_KB_write();
	io_out8(PORT_KB_CMD,KBCMD_SENDTO_MOUSE);
	wait_KB_write();
	io_out8(PORT_KB_DATA,MOUSE_ENABLE);

	for(i = 0;i<1000;i++)
		for(j = 0;j<1000;j++)
			nop();

	wait_KB_write();
	io_out8(PORT_KB_CMD,KBCMD_WRITE_CMD);
	wait_KB_write();
	io_out8(PORT_KB_DATA,KB_INIT_MODE);

	pointer.xPos = pointer.yPos = 0;
}


void mouse_handler(){
    unsigned char input = io_in8(PORT_KB_DATA);
    *mcode->head = input;
    mcode->head++;
    if (mcode->count == KB_BUFF_SIZE){
    	mcode->count -= 5;
    	mcode->tail += 4;
    }
    mcode->count++;

    if (mcode->head == mcode->end)
    	mcode->head = mcode->buf;
    if (mcode->tail == mcode->end)
    	mcode->tail = mcode->buf;
    decode_mouse();
}

char get_mouse_code(){
	if (mcode->count == 0)
		return 0;

	char ret = *mcode->tail;
	mcode->tail++;
	// printf("%d %d;", mcode->head - mcode->buf, mcode->tail - mcode->buf);
	if (mcode->tail >= mcode->end)
		mcode->tail = mcode->buf;
	mcode->count--;

	return ret;
}

void decode_mouse(){
	static int count = 0;
	if (mcode->count == 0)
		return;
	int code = (int)get_mouse_code();
	
	switch(count){
		case 0:
			count++;
			int i, j;
			for (i = 0;i < 32;i++){
				for (j = 0;j < 16;j++)
					pointer_cache[i][j] = GetPixel(j, i);
			}
			break;
		case 1:
			count++;
			mouse->byte0 = code;
			mouse_left = mouse->byte0 & 0x1;
			mouse_right = mouse->byte0 & 0x2;
			mouse_mid = mouse->byte0 & 0x4;
			break;
		case 2:
			count++;
			mouse->byte1 = code;//((int)code * ((mouse->byte0 & 0x10) ? -1 : 1));
			break;
		case 3:
			count = 1;
			mouse->byte2 = code;//((int)code * ((mouse->byte0 & 0x20) ? -1 : 1));
			draw_pointer(mouse->byte1, mouse->byte2);
			// if (mouse_left)
			// 	printf("left\n");
			// if (mouse_right)
			// 	printf("right\n");
			// if (mouse_mid)
			// 	printf("mid\n");
			// printf_color(GREEN, BLACK, "%d %d %d\t", mouse->byte0, mouse->byte1, mouse->byte2);
			break;
		case 4:
			count = 1;
			break;
		default:
			break;
	}
}

void draw_pointer(int x, int y){
	y = -y;
	// if (y > 0)
	// 	y /= 100;
	// else
	// 	y *= 2;
	// if (x < 0)
	// 	x = 255 - x;
	// else
	// 	x *= 2;

	int tmpx = pointer.xPos + x;
	int tmpy = pointer.yPos + y;

	int i, j;
	for (i = 0;i < 32;i++){
		for (j = 0;j < 16;j++)
			SetPixel(pointer.xPos + j, pointer.yPos + i, pointer_cache[i][j]);
	}
	for (i = 0;i < 32;i++){
		for (j = 0;j < 16;j++){
			int xx = tmpx + j;
			int yy = tmpy + i;
			pointer_cache[i][j] = GetPixel(xx, yy);
			SetPixel(xx, yy, pointer_point[i][j]);
		}
	}

	pointer.xPos += x;
	pointer.yPos += y;
	// pointer.xPos %= Pos.XResolution;
	// pointer.yPos %= Pos.YResolution;

	if (pointer.xPos < 0)
		pointer.xPos = 0;
	if (pointer.yPos < 0)
		pointer.yPos = 0;
	if (pointer.xPos > Pos.XResolution)
		pointer.xPos = Pos.XResolution;
	if (pointer.yPos > Pos.YResolution)
		pointer.yPos = Pos.YResolution;
	// printf("x: %d  y: %d  ", pointer.xPos, pointer.yPos);
}