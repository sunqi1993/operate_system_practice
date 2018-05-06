#include "asm_print.h"
#include "init.h"
#include "interrupt.h"
#include "memory.h"
#include "thread.h"
#include "debug.h"
#include "thread.h"
void thread_a(void* arg)
{
    char* str=arg;
    while (1)
    {
        put_str(str);

    }
}

void thread_b(void* arg)
{
    char* str=arg;
    while (1)
    {
        put_str(str);
    }
}

void check_intr_status()
{
    enum intr_status a=intr_get_status();
    if(a==INTR_ON) put_str("the intr is on\n");
    else put_str("the intr is off\n");
}

int main(void) {


    put_str("I am kernel\n");
    init_all();
    intr_disable();

    put_str("\n");
    thread_init();   //初始化内核的两个页表这个是必须进行的否则会报错
    thread_start("thread_a",21,thread_a," A ");
    thread_start("thread_b",21,thread_b," B ");
    intr_enale();

    while(1)
    {
//        put_str("main_thread");
    }
   return 0;
}

