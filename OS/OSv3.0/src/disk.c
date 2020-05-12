#include "disk.h"
#include "interrupt.h"
#include "lib.h"
#include "io.h"
#include "graph.h"

unsigned char* reader;

void InitDisk(){
	// register_irq(0x2E, disk_handler);
	// register_irq(0x2F, disk_handler);
	// 使能中断请求
	// io_out8(PORT_DISK0_ALT_STA_CTL, 0);
	finish = 0;
}

void read_one_sector(int n, unsigned char* cx){
	finish = 0;
	reader = cx;

	io_out8(PORT_DISK0_ALT_STA_CTL, 0);
	io_out8(PORT_DISK0_SECTOR_CNT, 1);
	io_out8(PORT_DISK1_ERR_FEATURE, 0);
	io_out8(PORT_DISK0_SECTOR_LOW, (unsigned char)(n & 0xff));
	io_out8(PORT_DISK0_SECTOR_MID, (unsigned char)((n >> 8) & 0xff));
	io_out8(PORT_DISK0_SECTOR_HIGH, (unsigned char)((n >> 16) & 0xff));
	io_out8(PORT_DISK0_DEVICE, 0xe0 | (unsigned char)((n >> 24) & 0x0f));
	io_out8(PORT_DISK0_STATUS_CMD, 0x20);

	// 直接检测数据是否抵达端口
	while (!(io_in8(PORT_DISK0_STATUS_CMD) & DISK_STATUS_REQ));

	port_insw(PORT_DISK0_DATA, reader, 256);
	finish = 1;
}

void disk_handler(){
	printf_color(BLACK, GREEN, "st acc\n");
	port_insw(PORT_DISK0_DATA, reader, 256);
	printf_color(BLACK, GREEN, "ed acc\n");
	finish = 1;
	printf_color(BLACK, YELLOW, "disk read finish\n");
}
















