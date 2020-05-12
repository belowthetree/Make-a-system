#include "fs.h"
#include "disk.h"
#include "io.h"

#define STARTOFDATA 31

struct DIR files[100];
int filecnt;
unsigned char* fat;

void scanf_files(){
	filecnt = 0;

	int i, j;
	for (j = 0;j < 14;j++){
		read_one_sector(19 + j, dirs);
		while (!finish);

		for (i = 0; i < 0x10; i++){
			if (dirs[i].attr != 0x20)
				continue;
			files[filecnt++] = dirs[i];
			printf_color(BLACK, YELLOW, "filename %s wtime %ux\n", 
				dirs[i].filename, dirs[i].wtime);
		}
	}

	fat = kmalloc(512 * 9);
	for (i = 0;i < 9;i++){
		read_one_sector(i + 1, fat + i * 512);
	}
	printf_color(BLACK, RED, "scanf finish\n");
}

void ls(){
	int i;
	for (i = 0;i < filecnt; i++){
		printf_color(BLACK, INDIGO, "%s\t", files[i].filename);
	}
	printf("\n");
}

struct FILE* read_file(const char* filename){
	int i;
	for (i = 0;i < filecnt; i++){
		if (strcmpns(files[i].filename, filename)){
			printf("size %d\n", files[i].filesize);
			return store_file_cluster(files[i].stcluster);
		}
	}

	return 0;
}

struct FILE* store_file_cluster(unsigned short entry){
	printf("%d\t", entry);

	struct FILE* file = kmalloc(sizeof(struct FILE));
	struct FILE* tail = file;

	while (entry < 0xff8 && entry >= 2){
		// printf_color(BLACK, RED, "%d\t", entry);
		tail->context = kmalloc(512);
		read_one_sector(entry + STARTOFDATA, tail->context);
		tail->next = kmalloc(sizeof(struct FILE));
		tail = tail->next;
		if (entry % 2)
		entry = *(unsigned short*)(fat + entry * 3 / 2) >> 4;
		else
			entry = *(unsigned short*)(fat + entry * 3 / 2);
		entry &= 0xfff;
	}
	return file;
}