#include "cpu.h"
#include "io.h"
#include "graph.h"

void init_cpu(void)
{
	int i,j;
	unsigned int CpuFacName[4] = {0,0,0,0};
	char	FactoryName[17] = {0};

	//vendor_string
	get_cpuid(0,0,&CpuFacName[0],&CpuFacName[1],&CpuFacName[2],&CpuFacName[3]);

	*(unsigned int*)&FactoryName[0] = CpuFacName[1];

	*(unsigned int*)&FactoryName[4] = CpuFacName[3];

	*(unsigned int*)&FactoryName[8] = CpuFacName[2];	

	FactoryName[12] = '\0';
	printf_color(BLACK, YELLOW, "%s\t%010X\t%010X\t%010X\n",FactoryName,CpuFacName[1],CpuFacName[3],CpuFacName[2]);
	
	//brand_string
	for(i = 0x80000002;i < 0x80000005;i++)
	{
		get_cpuid(i,0,&CpuFacName[0],&CpuFacName[1],&CpuFacName[2],&CpuFacName[3]);

		*(unsigned int*)&FactoryName[0] = CpuFacName[0];

		*(unsigned int*)&FactoryName[4] = CpuFacName[1];

		*(unsigned int*)&FactoryName[8] = CpuFacName[2];

		*(unsigned int*)&FactoryName[12] = CpuFacName[3];

		FactoryName[16] = '\0';
		printf_color(BLACK, YELLOW, "%s",FactoryName);
	}
	printf_color(BLACK, YELLOW, "\n");

	//Version Informatin Type,Family,Model,and Stepping ID
	get_cpuid(1,0,&CpuFacName[0],&CpuFacName[1],&CpuFacName[2],&CpuFacName[3]);
	printf_color(BLACK, YELLOW, "Family Code:%010X,Extended Family:%010X,Model Number:%010X,Extended Model:%010X,Processor Type:%010X,Stepping ID:%010X\n",(CpuFacName[0] >> 8 & 0xf),(CpuFacName[0] >> 20 & 0xff),(CpuFacName[0] >> 4 & 0xf),(CpuFacName[0] >> 16 & 0xf),(CpuFacName[0] >> 12 & 0x3),(CpuFacName[0] & 0xf));

	//get Linear/Physical Address size
	get_cpuid(0x80000008,0,&CpuFacName[0],&CpuFacName[1],&CpuFacName[2],&CpuFacName[3]);
	printf_color(BLACK, YELLOW, "Physical Address size:%08d,Linear Address size:%08d\n",(CpuFacName[0] & 0xff),(CpuFacName[0] >> 8 & 0xff));

	//max cpuid operation code
	get_cpuid(0,0,&CpuFacName[0],&CpuFacName[1],&CpuFacName[2],&CpuFacName[3]);
	printf_color(BLACK, YELLOW, "MAX Basic Operation Code :%010X\t",CpuFacName[0]);

	get_cpuid(0x80000000,0,&CpuFacName[0],&CpuFacName[1],&CpuFacName[2],&CpuFacName[3]);
	printf_color(BLACK, YELLOW, "MAX Extended Operation Code :%010X\n",CpuFacName[0]);
}

void get_cpuid(unsigned int Mop, unsigned int Sop,
						unsigned int *a, unsigned int *b, 
						unsigned int *c, unsigned int *d)
{
	__asm__ __volatile__ (
		"cpuid \n\t"
		:"=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d)
		:"0"(Mop), "2"(Sop)
		);
}