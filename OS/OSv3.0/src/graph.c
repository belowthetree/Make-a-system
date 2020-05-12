#include "graph.h"
#include "io.h"
#include "fs.h"
#include "memory.h"

void SetPixel(int x, int y, unsigned int color){
    if (y >= Pos.YResolution || x >= Pos.XResolution || x < 0 || y < 0)
        return;
    int yy = Pos.XResolution * y;
    Pos.FB_addr[yy + x] = color;
}

unsigned long GetPixel(int x, int y){
    return Pos.FB_addr[y * Pos.XResolution + x];
}

void InitGraph(unsigned int *addr)
{
    Pos.FB_addr = addr;
    Pos.XPosition = Pos.YPosition = 0;
    Pos.XResolution = 1440;
    Pos.YResolution = 900;
    Pos.YCharSize = 16;
    Pos.XCharSize = 8;
}

void TranslateX(int x)
{
    Pos.XPosition += x;
    if (Pos.XPosition < 0){
        Pos.XPosition = Pos.XResolution + Pos.XPosition;
        Pos.YPosition -= Pos.YCharSize;
        if (Pos.YPosition < 0)
            Pos.YPosition = 0;
    }
    else if (Pos.XPosition >= Pos.XResolution){
        Pos.YPosition += Pos.YCharSize;
        Pos.XPosition %= Pos.XResolution;
    }
    Pos.YPosition %= Pos.YResolution;
}

void cls(){
    Pos.XPosition = Pos.YPosition = 0;
    memset(Pos.FB_addr, 0, Pos.XResolution * Pos.YResolution * sizeof(int));
}

void backspace(int n){
    TranslateX(-n * Pos.XCharSize);
    int i;
    for (i = 0; i < n;i++)
        printf(" ");
    TranslateX(-n * Pos.XCharSize);
}

void set_background(){
    char* picname = (char *)curcmd + 7;
    struct FILE* img = read_file(picname);

    if (img == 0){
        printf_color(BLACK, RED, "filename err %s\n", picname);
        return;
    }
    printf("%ux\n", img->next);

    struct BMP* bmp = (struct BMP*)(img->context);
    printf_color(BLACK, GREEN, "%c%c %d %d width %d height %d bpp %d\n", 
        bmp->type[0], bmp->type[1], bmp->imgsize, bmp->offset, bmp->width, bmp->height, bmp->bpp);

    int width = bmp->width;
    int height = bmp->height;
    long x, y, idx = bmp->offset-1, cnt = 0, xx = 0, yy = Pos.YResolution - 1;

    int i, j;
    unsigned char color[3];
    unsigned char* tmp = (unsigned char*)bmp;

    for (y = height - 1; y >= 0; y--) {
        long t = y * Pos.YResolution / height;
        for (x = 0; x < width; x++) {
            for (i = 0; i < 3; i++){
                color[cnt++ % 3] = tmp[idx++];
                if (idx % 512 == 0) {
                    img = img->next;
                    tmp = img->context;
                    idx = 0;
                }
            }
            long tmp = x * Pos.XResolution / width;
            for (;xx < tmp && xx < Pos.XResolution;xx++){
                for (i = yy;i >= t;i--){
                    SetPixel(xx, i, (color[2] << 16) + (color[1] << 8) + (color[0]));
                }
            }
        }
        yy = y * Pos.YResolution / height;
        xx = 0;
        cnt = 0;
        idx += idx % 4;
    }
    Pos.XPosition = Pos.YPosition = 0;
}