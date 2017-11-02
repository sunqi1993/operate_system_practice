//
// Created by sunqi on 17-11-2.
//

#include "interrupt.h"
#include "stdint.h"
#include "global.h"

#define IDT_DESC_CNT 0X21


//中断门描述符结构体
struct gate_desc{
    uint16_t func_offset_low_word;
    uint16_t selector;
    uint8_t dcount;     //此项为双字计数字段，是门描述符中的第四个字节
                        //此项为固定值，不用考虑
    uint8_t attribute;
    uint16_t func_offset_high_word;
};

static void make_idt_desc(struct gate_desc* p_gdesc,uint8_t attr,intr_handler function);

static struct gate_desc idt[IDT_DESC_CNT];   //中断描述符表IDT  本质上是一个中断描述符表的数组

extern intr_handler intr_entry_table[IDT_DESC_CNT];  //中断入口函数的地址


//将函数和相关参数转化为一个IDT表结构
static void make_idt_desc(struct gate_desc* p_gdesc,uint8_t attr,intr_handler function)
{
    p_gdesc->func_offset_low_word=(uint32_t)function & 0x0000ffff;
    p_gdesc->selector=SELECTOR_K_CODE;
    p_gdesc->dcount=0;
    p_gdesc->attribute=attr;
    p_gdesc->func_offset_high_word=((uint32_t)function&0xffff0000)>>16;

}

//初始化中断描述符表
static void idt_desc_init()
{
    for (int i = 0; i <IDT_DESC_CNT ; ++i) {
        make_idt_desc(&idt[i],IDT_DESC_ATTR_DPL0,intr_entry_table[i]);
    }
    put_str("idt_desc_init done\n");
}

//完成有关中断的所有初始化工作
void idt_init()
{
    put_str("idt_init start\n");
    idt_desc_init();   //初始化中断描述表
    pic_init(); //初始化8259A
    //加载idt
    uint64_t idt_operand=((sizeof(idt)-1)|((uint64_t)(uint32_t)idt<<16));
    asm volatile("lidt %0"::"m" (idt_operand));
    put_str("idt_init done");
}


