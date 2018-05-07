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

    ;mov ebx,[esp+16]  ;保存ret返回地址


    mov eax,[esp+20]  ;获得cur_task地址
    mov [eax],esp     ;esp数据被考入task_struct中的self_kstack中

    ;-------------------恢复下一个线程环境------------------------
    mov eax,[esp+24]  ;eax 保存的是next_task的地址
    mov esp,[eax]     ;将 self_kstack中存储的地址给esp




    ;mov [esp+20],ebx ;将ret 地址保存在 thread_struct unsed_addr中 用来返回

    pop ebp
    pop ebx
    pop edi
    pop esi

    ret
