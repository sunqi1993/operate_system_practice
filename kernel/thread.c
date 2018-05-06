//
// Created by sunqi on 18-5-1.
//

#include "thread.h"
#include "string.h"
#include "global.h"
#include "memory.h"
#include "interrupt.h"
#include "debug.h"

#define PG_SIZE 4096
//由kernel_thread去执行function(func_arg)

extern void switch_to(struct task_struct* cur,struct task_struct* next);

struct task_struct* main_thread;
list thread_ready_list;  //就绪队列
list thread_all_list;   //所有任务队列
static  struct  list_elem* thread_tag;  //用于保存队列中的线程节点


static void kernel_thread(thread_func *function,void* func_arg)
{
    function(func_arg);
}

//初始化线程栈
void thread_create(struct task_struct *pthread,thread_func function,void* func_arg)
{
    pthread->self_kstack-= sizeof(struct intr_stack);
    pthread->self_kstack-= sizeof(struct thread_stack);
    struct thread_stack* kthread_stack=(struct thread_stack*)pthread->self_kstack;

    kthread_stack->eip=kernel_thread;
    kthread_stack->function=function;
    kthread_stack->func_arg=func_arg;
    kthread_stack->ebp=kthread_stack->ebx=\
    kthread_stack->esi=kthread_stack->edi=0;
}

//初始化线程的基本信息
void init_thread(struct task_struct *pthread, char* name, int prio)
{
    memset(pthread,0, sizeof(*pthread));
    strcpy(pthread->name,name);

    if(pthread==main_thread)
        pthread->status=TASK_RUNING;
    else
        pthread->status=TASK_READY;

    pthread->priority=prio;
    pthread->self_kstack=(uint32_t*)((uint32_t)&pthread+PG_SIZE);
    pthread->stack_magic=MagicNumber;
    pthread->pgdir=NULL;
    pthread->ticks=prio;
}


struct task_struct* thread_start(char* name,int prio,thread_func function,void* func_arg)
{
    struct task_struct *thread=(struct task_struct*)get_kernel_pages(1);
    init_thread(thread,name,prio);
    thread_create(thread,function,func_arg);

//    asm volatile("movl %0,%%esp;pop %%ebp;pop %%ebx;pop %%edi;pop %%esi;"::"g"(thread->self_kstack):"memory");

    //确保之前就不在就绪队列之中
    ASSERT(!elem_find(&thread_ready_list,&thread->general_tag));
    list_append(&thread_ready_list,&thread->general_tag);
    ASSERT(!list_empty(&thread_ready_list));

    ASSERT(!elem_find(&thread_all_list,&thread->all_list_tag));
    list_append(&thread_all_list,&thread->all_list_tag);
    ASSERT(!list_empty(&thread_all_list));
    return thread;

}

struct task_struct* running_thread()
{
    uint32_t  esp;
    asm("mov %%esp,%0":"=g"(esp));
    //这段代码还需要深究
    return (struct task_struct*)(esp&0xfffff000);
}


//将kernel中的main函数化为主进程
static void make_main_thread()
{
    /*
     * 因为loader.S进入内核的时候mov esp,0xc009f000
     * 就是为其预留PCB的 所以pcb 地址为oxc009e000
     */
    main_thread=running_thread();

    init_thread(main_thread,"main_thread",31);

    //main thread 是当前进程 所以只需要将它加入运行list 而不需要加入ready_list
    ASSERT(!elem_find(&thread_all_list,&main_thread->all_list_tag));
    list_append(&thread_all_list,&main_thread->all_list_tag);
}

void schedule()
{
    //进入中断函数以后CPU自动会关闭中断
    ASSERT(intr_get_status()==INTR_OFF);

    struct task_struct* cur=running_thread();
    ASSERT(cur->stack_magic==MagicNumber);
    if(cur->status==TASK_RUNING)
    {
        //若此线程只是cpu时间到了，就将此线程加入到就绪队列尾部
        ASSERT(!elem_find(&thread_ready_list,&cur->general_tag))
        list_append(&thread_ready_list,&cur->general_tag);
        cur->ticks=cur->priority;
        cur->status=TASK_READY;
    } else{
        //此线程需要某些事情发生后才能继续上CPU
    }

    ASSERT(!list_empty(&thread_ready_list));
    thread_tag=NULL;

    /*
     * 将就绪队列中的第一个元素弹出
     */
#ifdef Schedule
    volatile static int num_cout=0;
    num_cout++;
    put_str("\nnum_count=");
    put_int(num_cout);
    put_str("\n");
#endif
    thread_tag=list_pop(&thread_ready_list);
#ifdef Schedule
    put_str(" 0x");
    put_int(thread_tag);
    put_str(" ");
#endif
    struct  task_struct* next=elem2entry(struct task_struct, general_tag, thread_tag);   //获取线程的PCB地址
#ifdef Schedule
    put_int(next->stack_magic);
    put_str(" ");
    put_int(next);
#endif
    ASSERT(next->stack_magic==MagicNumber);
//    while(1);
    switch_to(cur,next);
}


void thread_init()
{

    list_init(&thread_ready_list);
    list_init(&thread_all_list);
    /*将当前main函数创建为线程*/
    make_main_thread();

}