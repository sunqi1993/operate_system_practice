//
// Created by 3Wsunqi on 17-11-5.
//

#ifndef SYSTEM_DEBUG_H
#define SYSTEM_DEBUG_H


#include "interrupt.h"
#include "asm_print.h"

void panic_spin(char* filename,int line,const char* func,const char* condition);




/*************************  _VA_ARGS__ *****************************
 * _VA_ARGS__
 * 代表所有与省略号相对应的参数
 * “...”表示定义的宏参数可变
 * */

#define PANIC(...)  panic_spin(__FILE__,__LINE__,__func__,__VA_ARGS__)

#ifdef NDEBUG
    #define ASSERT(CONDITION)   ((void)0)
#else
#define ASSERT(CONDITION) \
    if(CONDITION){} else{  \
     /* 符号#让编译器将宏的参数转化为字符串字面量*/   \
    PANIC(#CONDITION); \
    }
#endif //NDEBUG

#endif //SYSTEM_DEBUG_H

