//
// Created by sunqi on 17-11-6.
//

#ifndef SYSTEM_BITMAP_H
#define SYSTEM_BITMAP_H

#include "global.h"

#define BITMAP_MASK 1

struct bitmap{
    /*遍历位图的时候整体上是以字节为单位 细节上是以位为单位*/
    uint32_t bitmap_bytes_len;
    uint8_t *bits;
};

void bitmap_init(struct bitmap* btmp);
bool bitmap_check_bit_idx(struct bitmap* btmp,uint32_t bit_idx);
int bitmap_scan(struct bitmap* btmp,uint32_t cnt);
void bitmap_set(struct bitmap* btmp,uint32_t bit_idx,int8_t value);


#endif //SYSTEM_BITMAP_H
