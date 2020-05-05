#include "memory.h"
#include "io.h"

extern char _text;
extern char _etext;
extern char _edata;
extern char _end;

void search_memory();
void init_page();
void free_page(struct PAGE*);
struct PAGE* alloc_page(unsigned long attr);
struct PAGE* use_page(int n, unsigned long attr);

void init_slab();
unsigned long alloc_slab(int i, int size);
struct SLAB* create_slab(int size);

void append_slab(struct SLAB* slab, int size);
void delete_slab(struct SLAB* slab, int size);

void InitMemory(){
	search_memory();
	init_page();
	init_slab();

	// int i;
	// for (i = 0;i < 16;i++){
	// 	struct PAGE* page = alloc_page(PAGE_KERNEL_PAGE);
	// 	printf("phy: %ux virt: %ux attr: %ux\n", page->physical_addr, page->virtual_addr, page->attr);
	// 	// free_page(page);
	// }

	// while(1);
	// int size = 15;
	// for (i = 0;i < 16;i++){
	// 	unsigned long addr = kmalloc(size);
	// 	size += 30;
	// 	printf("addr : %ux\n", addr);
	// 	kfree(addr);
	// }
}

void init_page(){
	unsigned char* bit_start = memory_manager.bitmap;
	struct PAGE* page = (struct PAGE*)(bit_start + memory_manager.map_len);
	memory_manager.pages = page;
	unsigned long i;
	printf_color(BLACK, INDIGO, "bit_start at %ux\n", (unsigned long)bit_start);

	// 内核代码部分标志为已用
	memset(memory_manager.bitmap, 0, memory_manager.map_len);
	for (i = 0;i * PAGE_2M_SIZE < Virt_To_Phy(memory_manager.kernel_end);i++){
		use_page(i, PAGE_KERNEL_PAGE);
	}
	printf_color(BLACK, INDIGO, "free page start at num %d\n", i);

	unsigned long pre = i;
	for (i = 0;i < AREANUM;i++){
		if (memory_manager.area[i]->free_page <= 0)
			continue;
		unsigned long start = memory_manager.area[i]->physical_addr;
		unsigned long idx = start / PAGE_2M_SIZE;

		if (idx < pre)
			continue;
		for (;pre < idx;pre++){
			printf("set %ld\n", pre);
			use_page(pre, PAGE_KERNEL_PAGE);
		}
		for (;idx <= memory_manager.area[i]->free_page;idx++){
			memory_manager.pages[idx].physical_addr = idx * PAGE_2M_SIZE;
			memory_manager.pages[idx].virtual_addr = Phy_To_Virt(memory_manager.pages[idx].physical_addr);
			memory_manager.pages[idx].attr = 0;
		}
		pre = memory_manager.area[i]->end & PAGE_2M_MASK;
	}
	printf_color(BLACK, GREEN, "page end at %ux\n", &memory_manager.pages[memory_manager.free_page]);
}

// 记录 BIOS 查找到的内存信息
void search_memory(){
	AREANUM = 0;
	struct MEMORY_E820* p = 0;
	struct AREA* area = (struct AREA*)AREABASE;
	memory_manager.kernel_start = (unsigned long) &_text;
	memory_manager.kernel_end = (unsigned long) &_end;
	unsigned long end = 0;
	int i;

	p = (struct MEMORY_E820*)0xffff800000007c00;

	for (i = 0; i < 32;i++){
		printf_color(BLACK, GREEN, "address: %ux\tlength: %ux\ttype: %ux\n", 
			p->addr, p->len, p->type);
		if (p->type == 1){
			// 记录可用区域信息
			area->physical_addr = PAGE_2M_ALIGN(p->addr);
			area->virtual_addr = Phy_To_Virt(area->physical_addr);
			area->free_page = (p->len + PAGE_2M_ALIGN(p->addr) - p->addr) / PAGE_2M_SIZE;
			area->end = p->addr + p->len;

			printf_color(BLACK, GREEN, "Area start at physic: %ux, virtual: %ux, free: %ux, total_page: %d\n",
				area->physical_addr, area->virtual_addr, area->free_page, area->free_page);

			memory_manager.area[AREANUM++] = area;
			if (end < p->addr + p->len)
				end = p->addr + p->len;
		}
		else if (p->type > 4 || p->type < 1)
			break;
		p++;
		area++;
	}
	// printf_color(BLACK,GREEN, "area end at %ux\n", area);
	// 计算位图长度，页表起始地址
	memory_manager.map_len = (memory_manager.total_page + 63) / 8;
	memory_manager.bitmap = (unsigned char *)MAPBASE;
	memory_manager.pages = (struct PAGE*)(MAPBASE + memory_manager.map_len);
	memory_manager.total_page = end / PAGE_2M_SIZE;
	memory_manager.free_page = memory_manager.total_page;
	printf("total memory is %uldMB\n", end / 1024 / 1024);
}

struct PAGE* use_page(int n, unsigned long attr){
	if (n >= memory_manager.total_page){
		printf_color(BLACK, RED, "%d is out of page num\n", n);
		return 0;
	}
	if (memory_manager.bitmap[n / 8] & (unsigned char)(1 << (n % 8))){
		printf_color(BLACK, RED, "page %d is using\n", n);
		return 0;
	}

	memory_manager.pages[n].attr = attr;
	memory_manager.bitmap[n / 8] |= (unsigned char)(1 << (n % 8));

	printf("use page %d, start at %ux bitmap %ux\n", n, memory_manager.pages[n].virtual_addr, 
		memory_manager.bitmap[n / 8]);
	return &memory_manager.pages[n];
}

struct PAGE* alloc_page(unsigned long attr){
	if (memory_manager.free_page <= 0){
		printf_color(BLACK, RED, "No free page\n");
		return 0;
	}

	int i;
	for (i = 0;i < memory_manager.map_len;i++){
		if (memory_manager.bitmap[i] >= (unsigned char)(~0))
			continue;
		int j;
		for (j = 0;j < 8;j++){
			// printf("%ud i:%d j:%d\n", (unsigned int)(memory_manager.bitmap[i] & (1 << j)), i, j);
			if (!(memory_manager.bitmap[i] & (unsigned char)(1 << j))){
				memory_manager.used_page++;
				memory_manager.free_page--;
				return use_page(i * 8 + j, attr);
			}
		}
	}
	printf_color(BLACK, RED, "alloc failed\n");
	return 0;
}

void free_page(struct PAGE* page){
	if (page == 0){
		printf_color(BLACK, RED, "Page is null\n");
		return;
	}

	int i;
	for (i = 0;i < memory_manager.total_page;i++){
		if (&memory_manager.pages[i] == page){
			memory_manager.bitmap[i / 8] &= (~(unsigned char)(1 << (i % 8)));
			// printf_color(BLACK, ORANGE, "Free page %d at %ux\n", i, page->virtual_addr);
			return;
		}
	}

	printf_color(BLACK, RED, "free page fail\n");
}

void init_slab(){
	int i, size = 32;
	for (i = 0;i < 16;i++){
		Slab_cache[i].size = size;
		Slab_cache[i].free_slab = 0;
		Slab_cache[i].used_slab = 0;
		Slab_cache[i].cache_pool = 0;
		size *= 2;
	}
}

unsigned long kmalloc(int size){
	int i, level = 32;
	for (i = 0;i < 16;i++){
		if (level >= size){
			printf_color(BLACK, GREEN, "find level:%d, idx: %d\n", level, i);
			return alloc_slab(i, level);
		}
		level *= 2;
	}

	printf_color(BLACK, RED, "size too large %ux\n", size);
	return 0;
}

unsigned long alloc_slab(int i, int size){
	struct SLAB* slab = Slab_cache[i].cache_pool;

	while (slab != 0 && slab->free_count <= 0){
		slab = slab->next;
	}
	if (slab == 0){
		slab = create_slab(size);
	}
	if (slab == 0)
		return 0;

	int total = slab->free_count + slab->used_count;
	for (i = 0;i < total;i++){
		if (slab->bitmap[i / 8] == (unsigned char)(~0))
			continue;

		if (!(slab->bitmap[i / 8] & (1 << (i % 8)))){
			slab->free_count--;
			slab->used_count++;
			slab->bitmap[i / 8] |= (unsigned char)(1 << (i % 8));
			return (unsigned long)slab->page->virtual_addr + i * size;
		}
	}

	printf_color(BLACK, RED, "alloc slab error\n");
	return 0;
}

struct SLAB* create_slab(int size){
	struct PAGE* page = alloc_page(PAGE_KERNEL_PAGE);
	struct SLAB* slab = 0;
	if (page == 0)
		return 0;

	switch (size){
		case 32:
		case 64:
		case 128:
		case 256:
		case 512:
			slab = (struct SLAB *)page->virtual_addr;
			slab->page = page;
			slab->bitmap = (unsigned char *)(page->virtual_addr + sizeof(struct SLAB));
			slab->free_count = (unsigned long)PAGE_2M_SIZE / size;
			slab->map_len = slab->free_count / 8;
			slab->used_count = (slab->map_len + size - 1 + sizeof(struct SLAB)) / size;
			slab->free_count -= slab->used_count;

			// printf_color(BLACK, GREEN, "slab used_count %d, map_len %d\n", slab->used_count, slab->map_len);
			memset(slab->bitmap, 0, slab->map_len);
			int i;
			for (i = 0;i < slab->used_count;i++){
				slab->bitmap[i / 8] |= (unsigned char)(1 << (i % 8));
			}
			slab->next = 0;
			// printf_color(BLACK, GREEN, "slab at %ux\n", slab->page->virtual_addr);

			break;

		case 1024:		//1KB
		case 2048:
		case 4096:		//4KB
		case 8192:
		case 16384:
		case 32768:
		case 65536:
		case 131072:		//128KB
		case 262144:
		case 524288:
		case 1048576:		//1MB
			slab = (struct SLAB *)kmalloc(sizeof(struct SLAB));
			if (slab == 0){
				printf_color(BLACK, RED, "create slab fail\n");
				return 0;
			}
			slab->map_len = slab->free_count / 8;
			slab->bitmap = (unsigned char*)kmalloc(slab->map_len);
			slab->page = page;
			slab->free_count = (unsigned long)PAGE_2M_SIZE / size;
			slab->used_count = 0;

			memset(slab->bitmap, 0, slab->map_len);

			break;

		default:
			printf("size error %d\n", size);
			break;
	}
	append_slab(slab, size);

	return slab;
}

void append_slab(struct SLAB* slab, int size){
	int i;
	int level = 32;
	for (i = 0;i < 16;i++){
		if (level == size)
			break;
		level *= 2;
	}
	if (i == 16){
		printf("end of slab size error %d\n", size);
		return;
	}

	struct SLAB* tail = Slab_cache[i].cache_pool;
	if (tail == 0){
		Slab_cache[i].cache_pool = slab;
		return;
	}

	while (tail->next != 0){
		tail = tail->next;
	}
	tail->next = slab;
	slab->prev = tail;
	Slab_cache[i].free_slab++;
}

void delete_slab(struct SLAB* slab, int size){
	if (slab->prev == 0)
		return;

	slab->prev->next = slab->next;
	slab->next->prev = slab->prev;

	switch(size){
		case 32:
		case 64:
		case 128:
		case 256:
		case 512:
			free_page(slab->page);
			break;

		default:
			kfree((unsigned long)slab);
			break;
	}
}

void kfree(unsigned long addr){
	unsigned long pagebase = addr & PAGE_2M_MASK;
	int i;

	for (i = 0; i < 16;i++){
		// printf_color(BLACK, GREEN, "search slab\n");
		struct SLAB * slab = Slab_cache[i].cache_pool;

		while (slab != NULL){
			if (slab->page->virtual_addr == pagebase){
				// printf_color(BLACK, GREEN, "find at %d\n", i);
				int idx = (addr - pagebase) / Slab_cache[i].size;
				slab->bitmap[idx / 8] &= (unsigned char)(~(1 << idx));
				slab->free_count++;
				slab->used_count--;

				if (slab->used_count == 0 && slab->free_count * 3 / 2 <= Slab_cache[i].free_slab
					&& Slab_cache[i].cache_pool != slab){
					delete_slab(slab, Slab_cache[i].size);
				}

				return;
			}
			slab = slab->next;
		}
	}

	printf_color(BLACK, RED, "free error\n");
}

