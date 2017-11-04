//
// Created by sunqi on 17-11-4.
//

#ifndef SYSTEM_TIMER_H
#define SYSTEM_TIMER_H

#include "stdint.h"
static  void freq_set(uint8_t counter_port,uint8_t counter_no,uint8_t rwl,uint8_t  counter_mode,uint8_t counter_value);
void timer_init();
#endif //SYSTEM_TIMER_H
