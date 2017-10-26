%include "boot.inc"

section MBR vstart=0x7c00
    mov ax,cs
    mov ds,ax
    mov es,ax
    mov ss,ax
    mov fs,ax
    mov sp,0x7c00
    mov ax,0xb800
    mov gs,ax

;;清屏
;;利用0x06功能号，上卷全部行则可以清除屏幕
;
; INT 0x10 功能号0x06 功能描述 上卷窗口
;输入功能号 AH=0x06
;AL上卷的行数（如果为0 则表示全部行）
;BH 上卷行的属性
; (cl ch) 左上角（x,y）的位置
; （DL,DH）窗口右下角（x,y）的位置
; 无返回值

    mov ax,0600h
    mov bx,0700h
    mov cx,0   ;左上角（0,0）
    mov dx,184fh   ;右下角（80,25）
    
    int 10h

    ;直接写显存来实现数据的显示
    ;输出 背景色绿色 前景色红色 跳动字符串“1 MBR”
    mov byte [gs:0x00],"1"
    mov byte [gs:0x01],0xa4    ;A表示绿色闪烁 4表示背景色为红色

    mov byte [gs:0x02],"M"
    mov byte [gs:0x03],0xa4

    mov byte [gs:0x04],"B"
    mov byte [gs:0x05],0xa4

    mov byte [gs:0x06],"R"
    mov byte [gs:0x07],0xa4

    ; jmp $    ;通过死循环悬停在此

    mov eax,LOADER_START_SECTOR  ;起始扇区的lba地址
    mov bx,LOADER_BASE_ADDR    ;写入地址
    mov cx,4       ;待读入的扇区数
    call rd_disk_m_16    ;以下读取程序的起始部分

    jmp LOADER_BASE_ADDR


; 读取硬盘的n个扇区
rd_disk_m_16:
; 备份eax cx
    mov esi,eax
    mov di,cx

; step1 设置要读取的扇区数
    mov dx,0x1f2  ;读写sector count 端口
    mov al,cl   ;将要读取的扇区数存在al中
    out dx,al   ;将待读取的区块数值写入端口

    mov eax,esi  ;恢复ax

; step2 将LBA地址存如0x1f3-0x1f6

    ;0-7 写入0x1f3
    mov dx,0x1f3
    out dx,al

    ;9-15 写入0x1f4
    mov cl,8
    mov dx,0x1f4
    shr eax,cl
    out dx,al

    ;16-23写入0x1f5
    shr eax,cl
    mov dx,0x1f5
    out dx,al 

    shr eax,cl  
    and al,0x0f  ;lba 24-27位
    or al,0xe0   ;7-4位设置为1110 表示lba模式：111 主盘:0
    mov dx,0x1f6
    out dx,al

;step3 向0x1f7写入读命令，0x20
    mov dx,0x1f7
    mov al,0x20
    out dx,al

; step4 检查硬盘的状态
.not_ready:
    ; 同一端口 写时表示写入命令字 读取时表示读入硬盘状态
    nop
    in al,dx 
    and al,0x88  ;第三位1：表示硬盘已经就绪 第七位表示硬盘忙
    cmp al,0x08
    jnz .not_ready

; step5 从0x1f0端口读取数据
    mov ax,di
    mov dx,256   ;一个扇区512b 一次读取一个字2b 需要读取的次数
    mul dx
    mov cx,ax   ;总共需要读取的次数

    ;切换到数据端口
    mov dx,0x1f0
.go_on_read:
    in ax,dx
    mov [bx],ax
    add bx,2
    loop .go_on_read
    ret 

    times 510-($-$$) db 0

    db 0x55,0xaa
