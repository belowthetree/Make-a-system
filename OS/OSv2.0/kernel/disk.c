#include "disk.h"
#include "interrupt.h"
#include "lib.h"
#include "io.h"
#include "graph.h"

void disk_init(){
	register_irq(0x2f, disk_handler);
	// 使能中断请求
	io_out8(PORT_DISK1_ALT_STA_CTL, 0);

	io_out8(PORT_DISK1_ERR_FEATURE, 0);
	printf_color(BLACK, RED, "%x\n", io_in8(PORT_DISK1_ERR_FEATURE));
	io_out8(PORT_DISK1_SECTOR_CNT, 1);
	io_out8(PORT_DISK1_SECTOR_LOW, 0);
	io_out8(PORT_DISK1_SECTOR_MID, 0);
	io_out8(PORT_DISK1_SECTOR_HIGH, 0);
	io_out8(PORT_DISK1_DEVICE, 0xe0);

	printf_color(BLACK, RED, "start disk\n");
	
	while (!(io_in8(PORT_DISK1_STATUS_CMD) & DISK_STATUS_READY));
	printf_color(BLACK, RED, "%x\n", io_in8(PORT_DISK1_STATUS_CMD));

	io_out8(PORT_DISK1_STATUS_CMD, 0x20);
}



void disk_handler(){
	int i;
	unsigned char cx[512];
	port_insw(PORT_DISK1_DATA, &cx, 256);
	printf_color(ORANGE, WHITE, "Read one sector finish", io_in8(PORT_DISK1_STATUS_CMD));
	for (i = 0;i < 512;i++)
		printf_color(ORANGE, WHITE, "%x ", cx[i]);
}
















