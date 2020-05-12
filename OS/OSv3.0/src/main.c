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
#include "timer.h"
#include "process.h"
#include "string.h"
#include "fs.h"

#define INSNUM 4
#define DONUM  10

void help();

char *ins[] = {
	"ls",
	"cls",
	"help help me",
	"set bg"
};

char *helpinfo[] = {
	"list files",
	"clear screen",
	"list help info",
	"set the screen background image",
};

void (* doit[DONUM])();

void main(){
	InitGraph((unsigned int*)0xffff800000a00000);
	InitDescripter();
	InitTrap();
	InitInterrupt();
	InitMemory();
	InitKeyboard();
	InitMouse();
	InitDisk();

	// unsigned char* cx = (unsigned char*)kmalloc(512);
	// read_one_sector(0, cx);
	// while (!finish);
	// for (i = 0;i < 512;i++)
	// 	printf("%c", cx[i]);

	printf("into C's kernel\n");

	InitTimer();
	// InitProcess();
	scanf_files();

	int i = 0;
	doit[i++] = ls;
	doit[i++] = cls;
	doit[i++] = help;
	doit[i++] = set_background;
	// set_background();
	// while(1);
	while(1){
		hlt();
		decode_keyboard();
		if (cmd){
			cmd = 0;
			for (i = 0; i < INSNUM;i++){
				int l = strlen(ins[i]);
				if (strncmp(ins[i], curcmd, l)){
					doit[i]();
					break;
				}
			}
			if (i >= INSNUM)
				printf_color(BLACK, RED, "no cmd\n");
		}
	}
}


void help(){
	int i;
	for (i = 0; i < INSNUM;i++)
		printf_color(BLACK, GREEN, "%s: %s\n", ins[i], helpinfo[i]);
}