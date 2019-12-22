#ifndef _CPU_H
#define _CPU_H

#define NR_CPUS 8
// Mop 主功能号，子功能号
void get_cpuid(unsigned int Mop, unsigned int Sop,
						unsigned int *a, unsigned int *b, 
						unsigned int *c, unsigned int *d);

void init_cpu(void);









#endif