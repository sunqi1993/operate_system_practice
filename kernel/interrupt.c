//
// Created by sunqi on 17-11-2.
//

#include "interrupt.h"
#include "print.h"
#include "stdint.h"
#include "global.h"
#include "io.c"



//static intr_handler interruptFunction_table[IDT_DESC_CNT];  //定义中断处理程序数组

extern intr_handler intr_entry_table[IDT_DESC_CNT];  //中断入口函数的地址

static struct gate_desc idt[IDT_DESC_CNT];   //中断描述符表IDT  本质上是一个中断描述符表的数组

/*通用的中断处理函数 一般用在异常出现的时候处理*/
static void general_intr_handler(uint32_t irq_num)
{
    //IRQ7 15会产生伪中断 无需处理 0x3f是从片上的最后一个IRQ引脚，保留项
    if(irq_num==0x27 || irq_num==0x2f) return;
    put_str("iqr_num:0x");
    put_int(irq_num);
    put_str("  ");
    put_str(intr_name[irq_num]);
    put_char('\n');
}

/*一般中断处理函数的注册 以及异常名称的注册*/
static void exception_init()
{
    for (int i = 0; i < IDT_DESC_CNT; ++i)
    {
        interruptFunction_table[i]=general_intr_handler;
        intr_name[i]="unknown";
    }
    intr_name[0]="#DE Divide Error";
    intr_name[1]="#DB Debuger Exception";
    intr_name[2]="NMI Interrupt";
    intr_name[3]="#BP BreakPoint Exception";
    intr_name[4]="#OF Overflow Exception";
    intr_name[5]="#BR Bound Range Exceeded Exception";
    intr_name[6]="#UD Invliad Opcode Exception";
    intr_name[7]="#NM Device Not Avaliable Exception";
    intr_name[8]="#DF Double Fault Exception";
    intr_name[9]="Coprocessor Segment Overrun";
    intr_name[10]="#TS Invalid TSS Exception";
    intr_name[11]="#NP Segment Not Present";
    intr_name[12]="#SS Stack Fault Exception";
    intr_name[13]="#GP General Protection Exception";
    intr_name[14]="#PF Page Fault Exception";
    //15是intel 保留项 未使用
    intr_name[16]="#MF x87 FPU Float-Point Exception";
    intr_name[17]="#AC Alignment Check Exception";
    intr_name[18]="#MC Machine Check Exception";
    intr_name[19]="#XF SMID Floating-Point Exception";

}

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
    for (int i = 0; i <IDT_DESC_CNT ; i++) {
        make_idt_desc(&idt[i],IDT_DESC_ATTR_DPL0,intr_entry_table[i]);
    }
    put_str("idt_desc_init done\n");
}

/*8259中断芯片主片从片初始化的设置*/

static void pic_init()
{
    /*初始化主片*/
    outb(PIC_M_CTRL,0x11);    //ICW1 :边沿触发 需要ICW4
    outb(PIC_M_DATA,0x20);    //ICW2 :设置中断向量号起始数值32
    outb(PIC_M_DATA,0X04);    //icw3 :设置主片【主片IR2接从片
    outb(PIC_M_DATA,0x01);    //ICW4 :设置x86模式，正常结束EOI 手动

    /*初始化从片*/
    outb(PIC_S_CTRL,0x11);    //边沿触发，级联8259 需要ICW4
    outb(PIC_S_DATA,0x28);    //ICW2:设置从片的中断向量起始数值0x28=0x20+7+1
    outb(PIC_S_DATA,0x02);    //从片连接主片的IR2
    outb(PIC_S_DATA,0x01);    //ICW4 8086模式，正常EOI

    /*打开主片上的IR0,也就是目前只接受时种产生的中断*/
    outb(PIC_M_DATA,0xfe);    //主片屏蔽IR1-7 只开放了时钟中断
    outb(PIC_S_DATA,0xff);    //从片屏蔽所有中断

    put_str("pic_init done!\n");

}

//完成有关中断的所有初始化工作
void  idt_init()
{
    put_str("idt_init start\n");
    idt_desc_init();   //初始化中断描述表
    exception_init();  //异常名称初始化通常的中断处理函数
    pic_init(); //初始化8259A
    //加载idt idt的个数
    uint64_t idt_operand=((sizeof(idt)-1)|((uint64_t)((uint32_t)idt)<<16));
    asm volatile("lidt %0"::"m" (idt_operand));
    put_str("idt_init done\n");
}

/*获取当前的中断状态*/
enum intr_status intr_get_status()
{
    uint32_t  eflags=0;
    GET_EFLAGS(eflags);
    return (EFLAGS_IF & eflags)? INTR_ON:INTR_OFF;
}


/*开中断 并且返回中断前的状态*/
enum intr_status intr_enale()
{
    enum intr_status old_status;
    if(INTR_ON==intr_get_status())
    {
        old_status=INTR_ON;
        return  old_status;
    } else{
        old_status=INTR_OFF;
        asm volatile("sti");
        return old_status;
    }
}


/*关中断 并且返回关中断前的状态*/
enum intr_status intr_disable()
{
    enum intr_status old_status;
    if(INTR_ON==intr_get_status())
    {
        old_status=INTR_ON;
        asm volatile("cli":::"memory");
        return  old_status;
    } else{
        old_status=INTR_OFF;
        return old_status;
    }

}


/*设置状态为status*/
enum intr_status intr_set_status(enum intr_status status)
{
    return status & INTR_ON ? intr_enale():intr_disable();
}