//
// Created by 3Wsunqi on 17-11-5.
//

#ifndef SYSTEM_DEBUG_H
#define SYSTEM_DEBUG_H


#include "interrupt.h"
#include "print.h"

void panic_pin(char* filename,int line,const char* func,const char* condition);




/*************************  _VA_ARGS__ *****************************
 * _VA_ARGS__
 * 代表所有与省略号相对应的参数
 * “...”表示定义的宏参数可变
 * */

#define PANIC(...)  panic_spin (__FILE__,__LINE__,__func__,__VA_ARGS__)

#ifdef NDEBUG
    #define ASSERT(CONDITION)   ((void)0)
#else
#define ASSERT(CONDITION) \
    if(CONDITION){} else{  \
     /* 符号#让编译器将宏的参数转化为字符串字面量*/   \
    PANIC(#CONDITION); \
    }
#endif //NDEBUG


void panic_spin(char* filename,int line, const char* func, const char* condition)
{
    intr_disable();   //因为有时候会单独调用panic_spin 所以在此处关闭中断
    put_str("*************************************\n");
    put_str("*              error                *\n");
    put_str("*************************************\n");
    put_str("filename: ");put_str(filename);put_str("\n");
    put_str("line: 0x");put_int(line);put_str("\n");
    put_str("function: ");put_str((char*)func);put_str("\n");
    put_str("condition: ");put_str((char*)condition);put_str("\n");
    while(1);

}


#endif //SYSTEM_DEBUG_H

