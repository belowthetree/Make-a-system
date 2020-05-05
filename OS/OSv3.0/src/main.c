#include "io.h"
#include "keyboard.h"
#include "graph.h"
#include "descriptor.h"
#include "trap.h"
#include "interrupt.h"
#include "lib.h"
#include "memory.h"
#include "mouse.h"
#include "disk.h"

void test();

void main(){
	InitGraph((unsigned int*)0xffff800000a00000);
	InitDescripter();
	InitTrap();
	InitInterrupt();
	InitMemory();
	keyboard_init();
	mouse_init();
	disk_init();

	unsigned char* cx = (unsigned char*)kmalloc(512);
	read_one_sector(0, cx);
	int i;
	while (!finish);
	for (i = 0;i < 512;i++)
		printf("%c", cx[i]);

	printf("into C's kernel\n");
	printf("handler at %ux\n", (unsigned long)keyboard_handler);
	printf("decode_keyboard at %ux\n", (unsigned long)decode_keyboard);
	printf("get_scancode at %ux\n", (unsigned long)get_scancode);
	// register_irq(0x21, test);

	set_tss_at(2, 0xffff800000008e00);

	while(1){
		hlt();
		decode_keyboard();
	}
}

void test(){
	printf("Hello\n");
}