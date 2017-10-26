TI_GDT  equ 0
RPL0 equ 0
SELECTOR_VIDEO equ (0x0003<<3)+TI_GDT+RPL0

[bits 32]
section .text
global put_str
;--------------------------------------
; c编译器会吧字符串结尾自动加上\0 它作为字符串的结束标记 有了这个标记咱们才能知道字符串的长度
; \0的ascii码为0
;--------------------------------------
put_str:
    ;由于本函数只用到了ebx ecx只备份这两个寄存器
    push ebx
    push ecx
    xor ecx,ecx
    mov ebx,[esp+12]
.goon:
    mov cl,[ebx]
    cmp cl,0  
    jz .str_over
    push ecx
    call put_char
    add esp,4
    inc ebx
    jmp .goon
.str_over
    pop ecx
    pop ebx
    ret 


global put_char
; ----------------put_char----------------
; 功能描述：把栈中的字符写入光标所在处
; ----------------------------------------
put_char:
    pushad ;备份32位寄存器环境

    mov ax,SELECTOR_VIDEO
    mov gs,ax
    
    ;获取当前光标位置
    ;先获得高八位
    mov dx,0x3d4
    mov al,0x0e  ;提供光标位置的高八位
    out dx,al
    mov dx,0x3d5
    in al,dx     ;得到光标位置的高八位
    mov ah,al

    ;再获取低八位
    mov dx,0x3d4
    mov al,0x0f
    out dx,al
    mov dx,0x3d5
    in al,dx

    ;将光标存入bx
    mov bx,ax
    ;在栈中获取待打印的字符 pushad压人了4*8=32个直接加上主调用函数地址4byte
    mov ecx,[esp+36]
    cmp cl,0xd    ;CR是0x0d LF是0x0a
    jz .is_carriage_return
    cmp cl,0x0a
    jz .is_line_feed

    cmp cl,0x08   ;backspace 的asc码是8
    jz .is_backspace
    jmp .put_other


.is_backspace:
    dec bx
    shl bx,1  ;光标的位置左移动一位等于乘以2

    mov byte [gs:bx],0x20
    inc bx
    mov byte [gs:bx],0x07
    shr bx,1
    jmp .set_cursor

.put_other:
    shl bx,1

    mov byte [gs:bx],cl  ;ascii 字符本身
    inc bx
    mov byte [gs:bx],0x07
    shr bx,1

    inc bx   ;下一个光标值

    cmp bx,2000
    jl .set_cursor

.is_line_feed     ;换行符号
.is_carriage_return     ;回车符号
    ;32模式下 被除数32位 
    xor dx,dx  ;dx放32位数字的高16位
    mov ax,bx  ;ax 低16位
    mov si,80  
    div si  ;结果dx存储余数 ax存储商
    
    sub bx,dx

.is_carriage_return_end:
    add bx,80
    cmp bx,2000

.is_line_feed_end:
    jl .set_cursor

; 屏幕行范围0-24 滚屏的原理是让屏幕的第1-24行 迁移到0-23
.roll_screen:
    cld
    ; 换行
    mov ecx,960  ;(2000-80)*2/4=960
    mov esi,0xc00b80a0    ;第一行行首
    mov edi,0xc00b8000    ;第0行行首
    rep  movsd

    ; 将最后一行填充为空白
    mov ebx,3840  ;最后一行行首的偏移地址1920*2
    mov ecx,80

.cls:
    mov word [gs:ebx],0x0720 ;0x0720是空白字符
    add ebx,2
    loop .cls
    mov ebx,1920  ;设置光标位置

.set_cursor:
    ;先设置高八位
    mov dx,0x03d4
    mov al,0x0e
    out dx,al
    mov dx,0x03d5
    mov al,bh   ;将bx的高位放入al写入
    out dx,al

    ; 再设置低八位
    mov dx,0x03d4
    mov al,0x0f
    out dx,al
    mov dx,0x03d5
    mov al,bl   ;将bx的高位放入al写入
    out dx,al
  
.put_char_done:
    popad
    ret 

section .data

put_int_buffer dq 0
; ---------------------------------------
; 将小端字节序列的数字变成对应的ascii码后 倒置
; 输入 ：栈中的参数为待打印的数字
; 如打印十进制数字15，会直接打印f，不会是0xf
; ---------------------------------------

global put_int

put_int:
    pushad
    mov ebp,esp
    mov eax,[ebp+4*9]
    mov edx,eax
    mov edi,7   ;指定在put_int_buffer中的初始偏移量
    mov ecx,8   ;32位数字中，16进制数字的个数是8个
    mov ebx,put_int_buffer

; 将32为数字按照十六进制的形式从低位到高位逐个进行处理
.16based_4bits:
    and edx,0x0000000F
    cmp edx,9
    jg .isA2F
    add edx,'0'
    jmp .store
.isA2F:
    sub edx,10
    add edx,'A'   ;A-F对应的ascii码

.store:
    mov [ebx+edi],dl
    dec edi
    shr eax,4
    mov edx,eax
    loop .16based_4bits

; 现在put_int_buffer中全是字符，打印之前
; 把高位连续的字符去掉 如把00123变成123
.ready_to_print:
    inc edi
.skip_prefix_0:
    cmp edi,8
    je .fulll0
; 找出连续的0字符 edi作为非0的字符的偏移
.go_on_skip:
    mov cl,[put_int_buffer+edi]
    inc edi
    cmp cl,'0'
    je .skip_prefix_0
    dec edi
    jmp .put_each_num
.fulll0:
    mov cl,'0'
.put_each_num:
    push ecx
    call put_char
    add esp,4
    inc edi
    mov cl,[put_int_buffer+edi]
    cmp edi,8
    jl .put_each_num
    popad
    
    ret 


