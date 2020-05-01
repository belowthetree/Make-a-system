#include "io.h"
#include "graph.h"
#include "interrupt.h"
#include "abnormal.h"

void main(){
	InitGraph(0xffff800000a00000);
	
	printf("into c's kernel\n");
	InitAbnormal();
	
	while(1)
		;
}