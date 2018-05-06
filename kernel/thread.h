//
// Created by sunqi on 18-5-1.
//

#ifndef SYSTEM_THREAD_H
#define SYSTEM_THREAD_H

#include "stdint.h"
#include "list.h"
#define MagicNumber 0x19870916
typedef void thread_func(void*);

enum task_status{
    TASK_RUNING,
    TASK_READY,
    TASK_BLOCKED,
    TASK_WAITING,
    TASK_HANGING,
    TASK_DIED,
};

/*
 * 中断栈 此结构用于保存中断发生时的上下文
 */

struct intr_stack{
    uint32_t vec_no;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp_dummy;
    //虽然pushad会把esp也压入但是esp是不断变化的所以会被popad忽略
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;
    //cpu从低特权级进入高特权级时候压入
    uint32_t error_code;
    void(*eip)(void);
    uint32_t cs;
    uint32_t eflags;
    void* esp;
    uint32_t ss;
};

struct thread_stack{
    uint32_t ebp;
    uint32_t ebx;
    uint32_t edi;
    uint32_t esi;

    //线程第一次执行的时候线程指向带调用的函数kernel_thread 其他时候eip指向switch_to的返回地址
    void(*eip)(thread_func* func,void* func_arg);

    //以下仅供c第一被调度上CPu时候使用
    void(*unused_ret_addr);   //返回地址基本用不到因为有切换程序
    thread_func *function;   //ret 跳到eip那个地址的时候 CPU默认这就是函数的参数
    void* func_arg;

};

//进程或者线程的PCB，程序控制块
struct task_struct{
    uint32_t *self_kstack;
    enum task_status  status;
    uint8_t priority;
    char name[16];
    uint8_t ticks;   //每次上CPU执行的时间
    uint8_t elapsed_ticks;   //此任务一共执行了多久

    list_elem general_tag;
    list_elem all_list_tag;

    uint32_t * pgdir;   //进程自己页表的虚拟地址
    uint32_t stack_magic;
};
extern list thread_ready_list;  //就绪队列
extern list thread_all_list;   //所有任务队列
struct task_struct* thread_start(char* name,int prio,thread_func function,void* func_arg);
struct task_struct* running_thread();
void schedule();

void thread_init();
#endif //SYSTEM_THREAD_H
