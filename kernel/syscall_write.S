section .data
str_c_lib:
db "c ibrary says: hello world!",0xa  ;0xa为 LF的ASCII码
str_c_lib_len equ $-str_c_lib

str_syscall:
db "syscall says hello world!",0xa
str_syscall_len equ $-str_syscall

section .text

global _start
_start:
; 方式1 模拟c语言中的系统调用write
    push str_c_lib_len
    push str_c_lib
    push 1
    call simu_write
    add esp,12

; 方式2  跨过库函数，直接进行系统调用
    mov eax,4
    mov ebx,1
    mov ecx,str_syscall
    mov edx,str_syscall_len
    int 0x80  ;发起中断 通知Linux完成请求的功能

    mov eax,1 ;1号子功能是eixt
    int 0x80   ;

;----------------自定义simu_write来模拟c库中调用函数write---------------------------
simu_write:
    push ebp
    mov ebp,esp
    mov eax,4 ;4号子功能是write系统调用
    mov ebx,[ebp+8] ;第一个参数
    mov ecx,[ebp+12]
    mov edx,[ebp+16]
    int 0x80
    pop ebp
    ret 
