//
// Created by sunqi on 17-11-6.
//

#include <string.h>
#include "bitmap.h"
#include "stdint.h"
#include "string.h"
#include "print.h"

#include "debug.h"


/*将问题btmp初始化*/
void bitmap_init(struct bitmap* btmp)
{
    memset(btmp->bits,0,btmp->bitmap_bytes_len);
}

/*判断bit_idx是否为1 若为1返回true 否则返回false*/
bool bitmap_check_bit_idx(struct bitmap* btmp,uint32_t bit_idx)
{
    uint32_t byte_idx=bit_idx/8;
    uint32_t bit_odd=bit_idx%8;
    return (btmp->bits[byte_idx]&(BITMAP_MASK<<bit_odd));
}

/*在位图中连续申请cnt个位，若成功则返回其起始下标，失败返回-1*/
int bitmap_scan(struct bitmap* btmp,uint32_t cnt)
{
    uint32_t idx_byte=0;
    /*bit为1表示占满*/
    while ((0xff==btmp->bits[idx_byte])&&idx_byte<btmp->bitmap_bytes_len)
    {
        idx_byte++;
    }

    ASSERT(idx_byte<cnt);
    if(idx_byte==btmp->bitmap_bytes_len)
        return -1;

    int idx_bit=0;
    /*若在位图范围内找到了某个空闲位 在该字节内诸位对比返回bit idx*/
    while ((uint8_t)BITMAP_MASK<<idx_bit & btmp->bits[idx_byte])
    {
        idx_bit++;
    }

    /*返回空闲位在数组内的下标*/
    int bit_idx_start=idx_byte*8+idx_bit;
    if(cnt==1)
        return bit_idx_start;

    //记录还有多少个位可以判断
    uint32_t bit_left=(btmp->bitmap_bytes_len*8-bit_idx_start);
    uint32_t next_bit=bit_idx_start+1;
    uint32_t count=1; //记录找到的空闲位的个数

    while (bit_left-->0)
    {
        if(!bitmap_check_bit_idx(btmp,next_bit))
        {
            count++;
        } else{
            count=0;
        }
        if(count==cnt)
        {
            bit_idx_start=next_bit-count+1;
            break;
        }
        next_bit++;
    }

    return bit_idx_start;
}

void bitmap_set(struct bitmap* btmp,uint32_t bit_idx,int8_t value)
{
    ASSERT(value==0 || value==1);
    uint32_t byte_idx=bit_idx/8;
    uint32_t bit_odd=bit_idx%8;
    if(value)
    {
        btmp->bits[byte_idx]|=((uint8_t)BITMAP_MASK<<bit_odd);
    } else{
        btmp->bits[byte_idx]&=~((uint8_t)BITMAP_MASK<<bit_odd);
    }
}