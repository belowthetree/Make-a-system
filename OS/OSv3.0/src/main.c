#include "io.h"
#include "graph.h"
#include "descripter.h"

void main(){
	InitGraph((unsigned int*)0xffff800000a00000);
	InitDescripter();

	printf("into c's kernel\n");
	// set_tss64(0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00,
 //     0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 
 //     0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 
 //     0xffff800000007c00);
	// InitAbnormal();
	// int i = 1 / 0;
	while(1)
		;
}