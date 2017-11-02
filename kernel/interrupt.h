//
// Created by sunqi on 17-11-2.
//

#ifndef SYSTEM_INTERRUPT_H
#define SYSTEM_INTERRUPT_H

#include "stdint.h"
typedef void* intr_handler;
void idt_init();
//static void pic_init();
//static void idt_desc_init();
//static void make_idt_desc(struct gate_desc* p_gdesc,uint8_t attr,intr_handler function);
//extern intr_handler intr_entry_table[IDT_DESC_CNT];  //中断入口函数的地址

#endif //SYSTEM_INTERRUPT_H
