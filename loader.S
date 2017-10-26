%include "boot.inc"

section loader vstart=LOADER_BASE_ADDR

jmp loader_start
;-----构建GDT内部的描述符------
GDT_BASE: dd 0x00000000
          dd 0x00000000

CODE_DESC: dd 0x0000ffff
           dd DESC_CODE_HIGH4

DATA_DESC: dd 0x0000ffff
           dd DESC_DATA_HIGH4

VIDEO_DESC: dd 0x80000007   ;limit=(0xbffff-0xb8000)/4k=0x7  低16位为段的界限  
            dd DESC_VIDEO_HIGH4  ;此时dpl=0


times 60 dq 0    ;此处预留60个描述符的空位

;equ 这种类似于宏定义 是不占内存空间的 会直接替换

GDT_SIZE equ $-GDT_BASE
GDT_LIMIT equ GDT_SIZE-1

SELECTOR_CODE equ (0x0001<<3)+TI_GDT+RPL0   ;TI 表示在GDT中索引描述符 RPL特权级
SELECTOR_DATA equ (0x0002<<3)+TI_GDT+RPL0
SELECTOR_VIDEO equ (0x0003<<3)+TI_GDT+RPL0




;一个段描述符8个字节 8*64=2^9 byte
;用于保存内存的容量 以字节为单位  此处的偏移地址为0x900+0x200=0xb00
total_mem_bytes dd 0    

gdt_ptr dw GDT_LIMIT
        dd GDT_BASE

;;;人工对齐 from total_mem_bytes to ards_nr 4+6+244+2=256 offset bytes
ards_buf times 244 db 0
ards_nr dw 0       ;记录ards结构体的数量
; 0xc00
loadermsg db '2 loader in real.'
type_msg db 0
strlen equ $-loadermsg


loader_start:
;-------------------------------------
;INT 10 功能号13 功能描述：打印字符串
;-------------------------------------
;AH:功能号13
;BH:页码
;BL:属性（若AL=00H或01H）
;CX:字符串长度
;（DH,DL）=坐标的（row,col）
;ES,BP:字符串地址
;AL:显示输出的方z式
;    0：字符串只含显示字符。其显示属性在AL中；显示后光标的位置不变
;    1：字符串只含显示字符。其显示属性在AL中；显示后光标的位置变
;    2：字符串含有显示字符和属性，显示后光标位置不变
;    3：字符串含有显示字符和属性，显示后光标位置变
;
;无返回值
    mov sp,LOADER_BASE_ADDR
    mov bp,loadermsg
    mov cx,strlen
    mov ax,0x1301 ;AH=13 AL=01
    mov bx,0x001f ;页号为0(BH=0h) 蓝底粉色字（BL=1Fh）
    mov dx,0x1800   
    int 0x10 
;---------------显示字符串结束----------------------------


;------------------内存检测-----------------------------------
; int 15h eax=000e820h,edx=534d4150h('SAMP') 获取内存布局
    xor ebx,ebx
    mov edx,0x534d4150
    mov di,ards_buf

    inc byte [type_msg]

.e820_mem_get_loop:
    mov eax,0x0000e820  ;执行int 15以后 eax的值变为534d4150h  所以每次执行都要更新eax
    mov ecx,20  ;ards 地址范围描述符的结构大小是20byte
    int 0x15

    jc .e820_failed_so_try_e801  ;cf位为1则有错误发发生，则执行 0xe801子功能

    add di,cx
    inc word [ards_nr]

    cmp ebx,0  ;如果eab=0 cf=0 那么说明ards全部返回
    jnz .e820_mem_get_loop

;在所有ards结构中找出（base_add_low+length_low）的最大值 即内存中的容量
    mov cx,[ards_nr]
    mov ebx,ards_buf
    xor edx,edx

.find_max_mem_area:
    mov eax,[ebx]
    add eax,[ebx+8]
    add ebx,20
    cmp edx,eax
    jge .next_ards
    mov edx,eax   ;edx为总内存的大小存储的地方

.next_ards:
    loop .find_max_mem_area
    jmp .mem_get_ok
;-------------e820检查方式结束-----------------------------------------------


; ----------------------------------------------------
; int 15h ax=e801h 获取内存大小 最大支持4G
; 返回后ax与cx一样以1kb为单位   bx,dx一样以64kb为单位
; ax,cx寄存器存储低16M,dx,bx存储16M-4G

.e820_failed_so_try_e801:
    inc byte [type_msg]

    mov ax,0xe801
    int 0x15
    jc  .e801_failed_so_try_e88

    mov cx,0x400 ;1k为单位转换为byte
    mul cx
    shl edx,16  ;64k 为单位 转化为byte
    and eax,0x0000FFFF  ;此时还是16bit 实模式mul 16bit 结果保留在dx,ax
    or edx,eax
    add edx,0x100000
    mov esi,edx    ;此时将15M一下的内存计算出来

; 将16M以上的内存装换为byte
    xor eax,eax
    mov ax,bx
    mov ecx,0x10000
    mul ecx       ;mul 32bit时候结果地32位保留在eax
    add esi,ecx      
    mov edx,esi   ;edx为内存大小

    jmp .mem_get_ok
; ----------------e801检测方式结束-----------------------------

;------------------------------------------------------
; int 15h ah=0x88 获取内存大小，只能获取64M以内的
.e801_failed_so_try_e88:
    inc byte [type_msg]

    mov ah,0x88
    int 0x88
    jc .error_hlt

    mov cx,0x400
    mul cx
    shl edx,16
    and eax,0x0000ffff
    or edx,eax
    add edx,0x100000



.mem_get_ok:
    mov [total_mem_bytes],edx


;---------------------准备进入保护模式-----------------------------
;
;1 打开A20
;2 加载GDT
;3 将cr0的pe置1

    in al,0x92
    or al,0x02
    out 0x92,al
    
    ;-------------加载GDT-------------------------------------------
    lgdt [gdt_ptr]


    ;------------cr0 第0位置1---------------------------------------
    mov eax,cr0
    or eax,0x01
    mov cr0,eax

    jmp dword SELECTOR_CODE:p_mode_start

.error_hlt:
    jmp $  ;程序悬停在此处


[bits 32]
p_mode_start:
    mov ax,SELECTOR_DATA
    mov ds,ax
    mov es,ax
    mov ss,ax
    mov esp,LOADER_STACK_TOP
    mov ax,SELECTOR_VIDEO
    mov gs,ax
; ------------------加载内核-----------------------
    mov eax,KERNEL_START_SECTOR ;kernel.bin所在的扇区号
    mov ebx,KERNEL_BIN_BASE_ADDR ;从硬盘读出后写到ebx指定的地址
    mov ecx,200
    call rd_disk_m_32

; ------------------------------------------------
    call setup_page

    ; 先将描述符表地址偏移量写入内存 一会重新加载
    sgdt [gdt_ptr]

    ; 修改gdt中视频段的段描述符的段基址+0xc0000000
    mov ebx,[gdt_ptr+2] ;
    or dword [ebx+0x18+4],0xc0000000
    
    ; 将gdt的基址加上0xc0000000成为高地址
    add dword [gdt_ptr+2],0xc0000000
    ; 修改栈的指针
    add esp,0xc0000000

    ; 将page目录地址赋值给cr3
    mov eax,PAGE_DIR_TABLE_POS
    mov cr3,eax

    ; 打开CR0的Pg位
    mov eax,cr0
    or eax,0x1<<31
    mov cr0,eax

    lgdt [gdt_ptr]  ;重新载入页表刷新

    jmp SELECTOR_CODE:enter_kernel

enter_kernel:
    call kerne_init
    mov esp,0xc009f000
    jmp KERNEL_ENTRY_POINT

; -------------kernal载入相关------------------------
kerne_init:
    xor eax,eax
    xor ebx,ebx ;ebx记录程序头表地址
    xor ecx,ecx ;cx记录program header数量
    xor edx,edx ;dx 记录program header尺寸 e_phentsize

    mov dx,[KERNEL_BIN_BASE_ADDR+42] ;偏移文件42字节处是属性e_phentsize 
    mov ebx,[KERNEL_BIN_BASE_ADDR+28];偏移文件28字节的 e_phoff 表示第一个program header在文件中的偏移量

    add ebx,KERNEL_BIN_BASE_ADDR ;e_phoff+base_addr=program header start:[p_type]

    mov cx,[KERNEL_BIN_BASE_ADDR+44] ;偏移部分44byte 是e_phnum 表示有几个program header


.each_segment:
    cmp byte [ebx+0],PT_NULL
    je .PTNULL
    ; 为函数mem_cpy压人参数，参数是从右往左依次压人
    ; 为参数 mem_cpy(dst,src,size)

    push dword [ebx+16]  ;p_type + offset 16 byte =p_filesz 本段在文件中的大小
    mov eax,[ebx+4] ;p_offset 本段在文件类的起始偏移字节
    add eax,KERNEL_BIN_BASE_ADDR  ;加法以后 eax代表内核文件被加载的地址+段的偏移地址
    push eax ;压人第二个参数 src addr
    push dword [ebx+8] ;压人第一个参数 目的地址 

    call mem_cpy

    add esp,12 ;清理栈中的三个参数
    


.PTNULL:
;   该段可以忽略 通过增加偏移地址使得移动到可执行的段
    add ebx,edx ;edx 为program header的大小
    loop .each_segment
    ret 

; -----------------逐字节拷贝----------------------------
mem_cpy:
    cld 
    push ebp
    mov ebp,esp
    push ecx

    mov edi,[ebp+8]  ;dst
    mov esi,[ebp+12] ;src
    mov ecx,[ebp+16] ;size
    rep movsb  ;逐字节拷贝

    ; 恢复环境
    pop ecx
    pop ebp
    ret 

; ---------------------------------------------------
    ; 显示进入的模式 1,2,3
    mov ax,48  ;0的ascii 48
    add ax,[type_msg]
    mov byte [gs:160],al

    mov byte [gs:161],0xa4
    jmp $




; --------------页表设置----------------------------
setup_page:
    mov ecx,4096
    mov esi,0

;先把目录页的所占空间全部清0
.clear_page_dir:
    mov byte [PAGE_DIR_TABLE_POS+esi],0
    inc esi
    loop .clear_page_dir

; 开始创建页PDE目录项
.create_pde:
    mov eax,PAGE_DIR_TABLE_POS
    add eax,0x1000 
    mov ebx,eax  ;为create pte做准备 此处存放的是第一个页表的地址
    ;设置页目录项目的属性
    or eax, PG_US_U|PG_RW_W|PG_P
    mov [PAGE_DIR_TABLE_POS],eax   ;第一个目录项 目前在实模式 防止模式切换以后！m控件变了
    ;虚拟内存中3-4G内存分布的是操作系统 作为共享区域供进程调用 0xc00/4=index
    mov [PAGE_DIR_TABLE_POS+0xc00],eax

    sub eax,0x1000
    mov [PAGE_DIR_TABLE_POS+4092],eax ;页表项的最以后一项指向PDE的地址

;   下面创建页表项
    mov ecx,256
    mov esi,0
    mov edx,PG_P|PG_RW_W|PG_US_U

.create_pte:
    ; 在页表项中写入对应的4k表的地址  0-1M 共256个页表项
    mov [ebx+esi*4],edx
    add edx,4096
    inc esi
    loop .create_pte

; 创建内核以及其他页表
    mov eax,PAGE_DIR_TABLE_POS
    add eax,0x2000  
    or eax,PG_P|PG_RW_W|PG_US_U
    mov ebx,PAGE_DIR_TABLE_POS
    mov ecx,254
    mov esi,769  
.create_kernal_pde:  ;将PDE 0xc0 index上的全部
    mov [ebx+esi*4],eax
    inc esi 
    add eax,0x1000
    loop .create_kernal_pde

    ret 

;-------------------------------------------------------------------------------
			   ;功能:读取硬盘n个扇区 将内核文件迁移到内存
rd_disk_m_32:	   
;-------------------------------------------------------------------------------
							 ; eax=LBA扇区号
							 ; ebx=将数据写入的内存地址
							 ; ecx=读入的扇区数
      mov esi,eax	   ; 备份eax
      mov di,cx		   ; 备份扇区数到di
;读写硬盘:
;第1步：设置要读取的扇区数
      mov dx,0x1f2
      mov al,cl
      out dx,al            ;读取的扇区数

      mov eax,esi	   ;恢复ax

;第2步：将LBA地址存入0x1f3 ~ 0x1f6

      ;LBA地址7~0位写入端口0x1f3
      mov dx,0x1f3                       
      out dx,al                          

      ;LBA地址15~8位写入端口0x1f4
      mov cl,8
      shr eax,cl
      mov dx,0x1f4
      out dx,al

      ;LBA地址23~16位写入端口0x1f5
      shr eax,cl
      mov dx,0x1f5
      out dx,al

      shr eax,cl
      and al,0x0f	   ;lba第24~27位
      or al,0xe0	   ; 设置7～4位为1110,表示lba模式
      mov dx,0x1f6
      out dx,al

;第3步：向0x1f7端口写入读命令，0x20 
      mov dx,0x1f7
      mov al,0x20                        
      out dx,al

;;;;;;; 至此,硬盘控制器便从指定的lba地址(eax)处,读出连续的cx个扇区,下面检查硬盘状态,不忙就能把这cx个扇区的数据读出来

;第4步：检测硬盘状态
  .not_ready:		   ;测试0x1f7端口(status寄存器)的的BSY位
      ;同一端口,写时表示写入命令字,读时表示读入硬盘状态
      nop
      in al,dx
      and al,0x88	   ;第4位为1表示硬盘控制器已准备好数据传输,第7位为1表示硬盘忙
      cmp al,0x08
      jnz .not_ready	   ;若未准备好,继续等。

;第5步：从0x1f0端口读数据
      mov ax, di	   ;以下从硬盘端口读数据用insw指令更快捷,不过尽可能多的演示命令使用,
			   ;在此先用这种方法,在后面内容会用到insw和outsw等

      mov dx, 256	   ;di为要读取的扇区数,一个扇区有512字节,每次读入一个字,共需di*512/2次,所以di*256
      mul dx
      mov cx, ax	   
      mov dx, 0x1f0
  .go_on_read:
      in ax,dx		
      mov [ebx], ax
      add ebx, 2
			  ; 由于在实模式下偏移地址为16位,所以用bx只会访问到0~FFFFh的偏移。
			  ; loader的栈指针为0x900,bx为指向的数据输出缓冲区,且为16位，
			  ; 超过0xffff后,bx部分会从0开始,所以当要读取的扇区数过大,待写入的地址超过bx的范围时，
			  ; 从硬盘上读出的数据会把0x0000~0xffff的覆盖，
			  ; 造成栈被破坏,所以ret返回时,返回地址被破坏了,已经不是之前正确的地址,
			  ; 故程序出会错,不知道会跑到哪里去。
			  ; 所以改为ebx代替bx指向缓冲区,这样生成的机器码前面会有0x66和0x67来反转。
			  ; 0X66用于反转默认的操作数大小! 0X67用于反转默认的寻址方式.
			  ; cpu处于16位模式时,会理所当然的认为操作数和寻址都是16位,处于32位模式时,
			  ; 也会认为要执行的指令是32位.
			  ; 当我们在其中任意模式下用了另外模式的寻址方式或操作数大小(姑且认为16位模式用16位字节操作数，
			  ; 32位模式下用32字节的操作数)时,编译器会在指令前帮我们加上0x66或0x67，
			  ; 临时改变当前cpu模式到另外的模式下.
			  ; 假设当前运行在16位模式,遇到0X66时,操作数大小变为32位.
			  ; 假设当前运行在32位模式,遇到0X66时,操作数大小变为16位.
			  ; 假设当前运行在16位模式,遇到0X67时,寻址方式变为32位寻址
			  ; 假设当前运行在32位模式,遇到0X67时,寻址方式变为16位寻址.
      loop .go_on_read
      ret


