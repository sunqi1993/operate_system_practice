[bits 32]
%define ERROR_CODE NOP
%define ZERO push 0

extern put_str   ;声明外部函数

section .data
intr_str db "interrupt occur!",0xa,0
global intr_entry_table
intr_entry_table:


%macro VECTOR 2
section .text
intr%1entry:       ;每个中断都要压入中断向量号自己知道自己的中断向量号是多少
    %2             ;有错误码就压入错误码 没有就压人0填充
    push intr_str
    call put_str
    add esp,4     ;跳过函数的参数地址

    ;如果是从从片上进入的中断，除了往从片上发EOI,还要往主片上发送EOI
    mov al,0x20
    out 0xa0,al
    out 0x20,al

    add esp,4
    iret
section .data
    dd intr%1entry   ;存储入口函数的地址

%endmacro


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