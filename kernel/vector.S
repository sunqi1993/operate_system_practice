[bits 32]
%define ERROR_CODE NOP
%define ZERO push 0

extern put_str   ;声明外部函数
extern interruptFunction_table ;引用外部定义的中断函数向量表

section .data
intr_str db "interrupt occur",0xa,0;
global intr_entry_table
intr_entry_table:
%macro VECTOR 2
section .text
intr%1entry:       ;每个中断都要压入中断向量号自己知道自己的中断向量号是多少
    %2             ;有错误码就压入错误码 没有就压人0填充

    ;以下是保存上下文环境
    push ds
    push es
    push gs
    push fs

    ;pushad指令压人32位寄存器，其入栈的顺序是 eax ecx edx ebx esp ebp esi edi , eax最先入栈
    pushad



    ;如果是从从片上进入的中断，除了往从片上发EOI,还要往主片上发送EOI
    mov al,0x20
    out 0xa0,al
    out 0x20,al

    push %1;不管中断函数表的目标程序是否需要参数都一律压人中断向量号，调试的时候很方便
    call [interruptFunction_table+%1*4] ;调用interruptFunction_table中的c版本中断处理函数 [a+b] 是基质加变址寻址方式特有的

    jmp intr_exit ;恢复现场

section .data
    dd intr%1entry   ;存储入口函数的地址

%endmacro

section .text
global intr_exit
intr_exit:
;以下是恢复上下文环境
    add esp,4 ;跳过中断向量号
    popad
    pop fs
    pop gs
    pop es
    pop ds
    add esp,4 ;跳过错误码
    iretd   ;从栈顶弹出32bit 放入EIP


VECTOR 0x00,ZERO
VECTOR 0x01,ZERO
VECTOR 0x02,ZERO
VECTOR 0x03,ZERO
VECTOR 0x04,ZERO
VECTOR 0x05,ZERO
VECTOR 0x06,ZERO
VECTOR 0x07,ZERO
VECTOR 0x08,ERROR_CODE
VECTOR 0x09,ZERO
VECTOR 0x0a,ERROR_CODE
VECTOR 0x0b,ERROR_CODE
VECTOR 0x0c,ZERO
VECTOR 0x0d,ERROR_CODE
VECTOR 0x0e,ERROR_CODE
VECTOR 0x0f,ZERO
VECTOR 0x10,ZERO
VECTOR 0x11,ERROR_CODE
VECTOR 0x12,ZERO
VECTOR 0x13,ZERO
VECTOR 0x14,ZERO
VECTOR 0x15,ZERO
VECTOR 0x16,ZERO
VECTOR 0x17,ZERO
VECTOR 0x18,ERROR_CODE
VECTOR 0x19,ZERO
VECTOR 0x1a,ERROR_CODE
VECTOR 0x1b,ERROR_CODE
VECTOR 0x1c,ZERO
VECTOR 0x1d,ERROR_CODE
VECTOR 0x1e,ERROR_CODE
VECTOR 0x1f,ZERO
VECTOR 0x20,ZERO