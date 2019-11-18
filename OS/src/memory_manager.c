#include "memory_manager.h"

// 物理内存管理的栈指针
static uint32 phy_memory_esp;

void InitMemory()
{
    MAX_PAGE_NUM = MAX_PHY_SIZE * 1024 / 4;
    memory_manager = (char *)PAGE_ADDR;
    phy_memory_esp = 0;
    int i;
    
    for (i = 0;i < MAX_PAGE_NUM;i++) {
        memory_manager[i] = PHYSIC_PAGE_SIZE * i + PHYSIC_ADDR_START;
    }
}

uint32 MallocPage()
{
    if (phy_memory_esp >= MAX_PAGE_NUM)
        return -1;
    
    return memory_manager[phy_memory_esp++];
}

void FreePage(uint32 page)
{
    if (phy_memory_esp <= 0)
        return ;
    memory_manager[phy_memory_esp--] = page;
}
