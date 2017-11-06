//
// Created by sunqi on 17-11-6.
//

#ifndef SYSTEM_MEMORY_H
#define SYSTEM_MEMORY_H

#include "stdint.h"
#include "bitmap.h"



extern struct pool kernel_pool,user_pool;

extern struct virtual_addr kernel_vaddr;

void mem_init();

#endif //SYSTEM_MEMORY_H
