#ifndef _MEMORY_MANAGER_H
#define _MEMORY_MANAGER_H
#include "main.h"

// 支持的最大物理内存 (512MB)
int MAX_PHY_SIZE;
// 支持的物理页的个数
int MAX_PAGE_NUM;
// 物理内存页框大小
#define PHYSIC_PAGE_SIZE    0x1000
// 页掩码按照4096对齐地址
#define PHYSIC_PAGE_MASK    0xfffff000
// 内存分配开始地址
#define PHYSIC_ADDR_START   0x300000
// 页管理器开始地址
#define PAGE_ADDR   0x150000
// 动态分配物理页的总页数
extern uint32   phy_page_count;

uint32 * memory_manager;

// 分配一个页
uint32 MallocPage();
// 释放一个页
void FreePage(uint32 page);

typedef struct PDE {
    char present;           // 存在位，是否存在于物理地址中
    char read_write;        // 读写权限，0 只读，1可读可写
    char user_supervisor;   // 特权级， 0 系统级（即 CPL=0,1,2），1 用户级（CPL=3）
    char write_through;     // 控制缓冲策略，0 使用 write-back 策略，1 使用 write-through
    char cache_disabled;
    char accessed;
    char reserved_zero;
    char page_size;
    char global_page;
    char avail[3];
    char page_table_base;
}pde;

typedef struct PTE
{
    
}pte;


#endif
