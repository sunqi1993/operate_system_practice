//
// Created by sunqi on 17-11-2.
//

#ifndef SYSTEM_IO_H
#define SYSTEM_IO_H

#include "stdint.h"
/**************************************
 * 机器模式
 * b  --输出蕲春器QImodd名称，即寄存器中的低8位[a-d]l
 * w  --输出寄存器HImode名称，即寄存器中两个字节的部分【a-d】x
 *
 * HImode "half-interger",表示一个两个字节的整数
 * QImode "Quarter-Interger",表示一个一个字节的整数
 **************************************/


/*向端口port写入一个字节*/
static  inline  void outb(uint16_t port,uint8_t data)
{
    /**************************************
     * 对端口指定N表示0-255，d表示用dx存储端口号
     * %b0 表示对应al %w0表示对应dx
     * ************************************/

    asm volatile("outb %b0,%w1"::"a" (data),"Nd" (port));

}


/*将addr起始处的word_count个字写入端口port*/
static inline void outsw(uint16_t port, const void* addr,uint32_t word_cnt)
{
    /*******************************************************
     * +表示此处限制，即作为输出又作为输出
     * outsw 是把 ds:esi处的16位内容写入port端口，在设置段描述符的时候
     * 已经将ds,es,ss段的选择子都设置为相同的数值了，不用担心数据错乱
     * *****************************************************/
    asm volatile("cld;rep outsw":"+S" (addr),"+c" (word_cnt):"d" (port));
}


/*从端口读入一个字节并返回*/
static  inline uint8_t intb(uint16_t port)
{
    uint8_t  data;
    asm  volatile("inb %w1,%b0":"=a"(data):"Nd"(port));
    return data;
}



/*将从端口port读入的word_cnt个字节写入addr*/
static inline void insw(uint16_t port,void* addr,uint32_t word_cnt)
{
    /**************************************************************
     * insw是将端口读入的16位内容写入es:edi指向的内存，我们在设置段描述符的时候已经将ds,es,ss段的选择子都设置为了相同的数值
     * 不用担心数据错乱 *
     **************************************************************/
    asm volatile("cld;rep insw":"+D" (addr),"+c" (word_cnt):"d" (port):"memory");
}
#endif //SYSTEM_IO_H
