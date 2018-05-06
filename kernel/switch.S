[bits 32]
section .text
global switch_to
switch_to:
    ;-------------------备份当前环境-------------------------
    ;此处是next_task地址
    ;此处是cur地址
    ;ret返回地址
    push esi;
    push edi;
    push ebx;
    push ebp;

    mov eax,[esp+20]  ;获得函数的返回地址
    mov [eax],esp     ;esp数据被考入task_struct中的kstack

    ;-------------------恢复下一个线程环境------------------------
    mov eax,[esp+24]
    mov esp,[eax]

    pop ebp
    pop ebx
    pop edi
    pop esi
    ret
