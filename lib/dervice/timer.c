//
// Created by sunqi on 17-11-4.
//

#include "timer.h"
#include "print.h"
#include "../../kernel/io.c"
#define IRQ0_FRQ 20
#define INPUT_FRQ 1193180

#define COUTER0_VALUE INPUT_FRQ/IRQ0_FRQ
#define COUNTER0_PORT 0X40
#define COUNTER0_NO 0
#define COUNTER_MODE 2
#define READ_WRITE_LATCH 3
#define PIT_CONTROL_PORT 0X43


/*
 * counter_port :计数器的计数值数据写入端口
 * counter_no   :计数器的数值 0-3
 * rwl          :控制字段的 readWriteFlag
 * counter_mode :计数器的运行模式
 * counter_value :计数器的写入的初始数值
 * */
static  void freq_set(uint8_t counter_port,uint8_t counter_no,uint8_t rwl,uint8_t  counter_mode,uint8_t counter_value)
{
    /*往控制寄存器的端口写入控制字段*/
    outb(PIT_CONTROL_PORT,(uint8_t)(counter_no<<6|rwl<<4|counter_mode<<1));
    /*先写入计数器的低八位*/
    outb(counter_port,(uint8_t)counter_value);
    /*写入计数值的高八位*/
    outb(counter_port,(uint8_t)(counter_value>>8));
}

void timer_init()
{
    put_str("timer init start.\n");
    /*初始化定时器的定时器0*/
    freq_set(COUNTER0_PORT,COUNTER0_NO,READ_WRITE_LATCH,COUNTER_MODE,COUTER0_VALUE);
}