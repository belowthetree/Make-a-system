#include "memory.h"
#include "type.h"
#include "graph.h"
#include "io.h"

void init_memory()
{
	int i,j;
	unsigned long TotalMem = 0 ;
	struct Memory_E820_Format *p = NULL;
	
	printf_color(BLACK, BLUE, "Display Physics Address MAP,Type(1:RAM,2:ROM or Reserved,3:ACPI Reclaim Memory,4:ACPI NVS Memory,Others:Undefine)\n");
	p = (struct Memory_E820_Formate *)0xffff800000007e00;

	for(i = 0;i < 32;i++)
	{
		printf_color(BLACK, ORANGE, "Address:%10uX,%10uX\t\
			Length:%10uX,%10uX\t Type:%10uX\n",
			p->address2,p->address1,p->length2,p->length1,p->type);
		unsigned long tmp = 0;
		if(p->type == 1)
		{
			tmp = p->length2;
			TotalMem +=  p->length1;
			TotalMem +=  tmp  << 32;
		}

		p++;
		if(p->type > 4)
			break;		
	}

	printf_color(BLACK, ORANGE, "OS Can Used Total RAM:%#018lx\n",TotalMem);
}