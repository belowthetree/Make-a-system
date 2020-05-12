#ifndef FS_H
#define FS_H

struct DIR{
	char filename[11];
	unsigned char attr;
	unsigned char rev[10];
	unsigned short wtime;
	unsigned short wdate;
	unsigned short stcluster;
	unsigned int filesize;
}__attribute__((packed));

struct BMP{
	unsigned char type[2];
	unsigned int size;
	unsigned short rev1;
	unsigned short rev2;
	unsigned int offset;
	unsigned int struct_size;
	int width;
	int height;
	short rev3;
	short bpp;
	unsigned int zip;
	unsigned int imgsize;
	int ver_px;
	int hor_px;
	unsigned int quote_cnt;
	unsigned int main_cnt;
}__attribute__((packed));

struct FILE{
	unsigned char* context;
	struct FILE *next;
};

struct COLOR{
	unsigned char B;
	unsigned char G;
	unsigned char R;
}__attribute__((packed));

struct DIR dirs[16];

void scanf_files();
void ls();
struct FILE* read_file(const char* filename);
struct FILE* store_file_cluster(unsigned short entry);


























#endif