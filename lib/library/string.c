//
// Created by sunqi on 17-11-4.
//

#include "debug.h"
#include "string.h"

#include "global.h"
#include "stdint.h"



/*将dst_起始的size个字节置为value*/
void memset(void* dst_,uint8_t value,uint32_t size)
{
#ifdef DEBUG
    put_str("memset params:");
    put_int(dst_);
    put_str(" ");
    put_int(value);
    put_str(" ");
    put_int(size);
    put_str("\n");
#endif
    ASSERT( dst_);  //判断指针是否为空
    uint8_t* dst=(uint8_t*)dst_;
    while (size-->0)
        *dst++=value;
}


/*将src开始的size个字节复制到dst*/
void memcpy(void* dst_, const void* src_,uint32_t size)
{
    ASSERT(dst_ &&  src_);
    uint8_t *dst=(uint8_t*)dst_;
    const uint8_t *src=src_;
    while (size-->0)
        *dst++=*src++;
}

/*连续比较以地址a b开始的size个字节，若相等则返回0 否则返回 -1 1*/
int memcmp(const void *a_, const void *b_,uint32_t size)
{
    const char* a=a_;
    const char* b=b_;
    ASSERT(a && b);
    while (size-->0)
    {
        if(*a!=*b)
            return *a>*b?1:-1;
        a++;
        b++;
    }
    return 0;

}


/*将字符串从src复制到dst*/

char* strcpy(char* dst_, const char* src_)
{
    ASSERT(dst_&&src_);
    char *dst=dst_;
    char *src=src_;
    while (*dst++=*src++);   //字符串 '\0' 的ASCii 是0
    return dst_;
}


uint32_t str_len(const char* str)
{
    ASSERT(str);
    const char *p=str;
    while (*p++);
    return (p-str-1);
}

int8_t strcmp(const char* a, const char *b)
{
    ASSERT(a&&b);
    while (*a!=0&& *a==*b)
    {
        a++;
        b++;
    }
    return *a<*b?-1:*a>*b;
}

/*返回str字符串第一次出现ch的地址*/
char* strchr(const char* str, const uint8_t ch)
{
    ASSERT(str);
    while (*str!=0)
    {
        if(*str==ch)
            return (char*)str;
        str++;
    }
    return  0;
}


/*从后往前查找第一次出现ch的地址*/

char* strrchr(const char* str, const uint8_t ch)
{
    ASSERT(str);
    const char *last_char=0;
    while (str!=0)
    {
        if(*str==ch)
            last_char=str;
        str++;
    }
    return (char*)last_char;
}

/* 将字符串src_拼接到dst_后,将回拼接的串地址 */
char* strcat(char* dst_, const char* src_) {
    ASSERT(dst_ != 0 && src_ != 0);
    char* str = dst_;
    while (*str++);
    --str;      // 别看错了，--str是独立的一句，并不是while的循环体
    while((*str++ = *src_++));	 // 当*str被赋值为0时,此时表达式不成立,正好添加了字符串结尾的0.
    return dst_;
}

/* 在字符串str中查找指定字符ch出现的次数 */
uint32_t strchrs(const char* str, uint8_t ch) {
    ASSERT(str != 0);
    uint32_t ch_cnt = 0;
    const char* p = str;
    while(*p != 0) {
        if (*p == ch) {
            ch_cnt++;
        }
        p++;
    }
    return ch_cnt;
}
