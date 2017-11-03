//
// Created by sunqi on 17-11-2.
//

#ifndef SYSTEM_INTERRUPT_H
#define SYSTEM_INTERRUPT_H

#include "stdint.h"


#define IDT_DESC_CNT 0X21

#define  PIC_M_CTRL 0X20
#define  PIC_M_DATA 0X21
#define  PIC_S_CTRL 0XA0
#define  PIC_S_DATA 0XA1

typedef void* intr_handler;
intr_handler interruptFunction_table[IDT_DESC_CNT];
void idt_init();
//static void pic_init();
//static void idt_desc_init();
//static void make_idt_desc(struct gate_desc* p_gdesc,uint8_t attr,intr_handler function);
//extern intr_handler intr_entry_table[IDT_DESC_CNT];  //中断入口函数的地址

#endif //SYSTEM_INTERRUPT_H
