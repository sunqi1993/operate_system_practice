//
// Created by sunqi on 17-11-6.
//

#ifndef SYSTEM_MEMORY_H
#define SYSTEM_MEMORY_H

#include "stdint.h"
#include "bitmap.h"

#define	 PG_P_1	  1	// 页表项或页目录项存在属性位
#define	 PG_P_0	  0	// 页表项或页目录项存在属性位
#define PG_RW_R	 0
#define PG_RW_W	 2
#define PG_US_S	 0
#define PG_US_U	 4

enum pool_flags{
    PF_KERNEL=1,
    PF_USER=2
};


extern struct pool kernel_pool,user_pool;

extern struct virtual_addr kernel_vaddr;

void mem_init();

void* get_kernel_pages(uint32_t pg_cnt);

#endif //SYSTEM_MEMORY_H
