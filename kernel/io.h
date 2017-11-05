//
// Created by sunqi on 17-11-4.
//

#ifndef SYSTEM_IO_H
#define SYSTEM_IO_H

#include "stdint.h"
static  inline  void outb(uint16_t port,uint8_t data);
static inline void outsw(uint16_t port, const void* addr,uint32_t word_cnt);
static  inline uint8_t intb(uint16_t port);
static inline void insw(uint16_t port,void* addr,uint32_t word_cnt);
#endif //SYSTEM_IO_H
