//
// Created by sunqi on 17-11-2.
//

#ifndef SYSTEM_INTERRUPT_H
#define SYSTEM_INTERRUPT_H
#include "stdint.h"
#include "interrupt.h"

#define IDT_DESC_CNT 0X21

#define  PIC_M_CTRL 0X20
#define  PIC_M_DATA 0X21
#define  PIC_S_CTRL 0XA0
#define  PIC_S_DATA 0XA1

#define EFLAGS_IF 0X00000200    //eflags_if位为1

/*
 * push{wl} 将标志寄存器压入栈 w:2byte l:4byte
 *
 */
#define GET_EFLAGS(EFLAGS_VAR) asm volatile("pushfl;popl %0":"=g"(EFLAGS_VAR))

//中断门描述符结构体
struct gate_desc{
    uint16_t func_offset_low_word;
    uint16_t selector;
    uint8_t dcount;     //此项为双字计数字段，是门描述符中的第四个字节
    //此项为固定值，不用考虑
    uint8_t attribute;
    uint16_t func_offset_high_word;
};


/*中断状态枚举类型*/

enum intr_status{
    INTR_OFF,        //中断关闭
    INTR_ON          //中断打开
};



char * intr_name[IDT_DESC_CNT]; //用于保存异常的名字

typedef void* intr_handler;
intr_handler interruptFunction_table[IDT_DESC_CNT];




void idt_init();
static void pic_init();
static void idt_desc_init();
static void make_idt_desc(struct gate_desc* p_gdesc,uint8_t attr,intr_handler function);
extern intr_handler intr_entry_table[IDT_DESC_CNT];  //中断入口函数的地址
static void general_intr_handler(uint32_t irq_num);

enum intr_status intr_get_status();
enum intr_status intr_enable();
enum intr_status intr_disable();
enum intr_status intr_set_status(enum intr_status status);
void register_handler(uint8_t irq_num,intr_handler function);
#endif //SYSTEM_INTERRUPT_H
