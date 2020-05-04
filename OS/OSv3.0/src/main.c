#include "io.h"
#include "graph.h"
#include "descriptor.h"
#include "trap.h"
#include "interrupt.h"
#include "memory.h"

void main(){
	InitGraph((unsigned int*)0xffff800000a00000);
	InitDescripter();
	InitTrap();
	InitInterrupt();
	InitMemory();

	

	printf("into C's kernel\n");
	while(1)
		;
}