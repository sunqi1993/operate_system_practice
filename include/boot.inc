;-------------	 loader和kernel   ----------

LOADER_BASE_ADDR equ 0x900 
LOADER_STACK_TOP equ LOADER_BASE_ADDR
LOADER_START_SECTOR equ 0x2

KERNEL_BIN_BASE_ADDR equ 0x70000
KERNEL_START_SECTOR equ 0x9
KERNEL_ENTRY_POINT equ 0xc0001500

;-------------   页表配置   ----------------

;---------------gdt描述符性质------------------

; DESC_G_4K equ 0x800000   ;23bit offset from 0bit
; DESC_D_32 equ 0x400000   ;22   D 1/0 操作数32/16bit选择位
; DESC_L    equ 0x000000   ;21   是否是64位代码段 这里选择0 否 32位模式

; ;64位代码标记此处标记为０即可
; DESC_AVL  equ 0x000000   ;20 CPU此位不用暂时为0
; DESC_LIMIT_CODEC2  equ  0xf0000  ;段的界限16-19
; DESC_LIMIT_DATA2   equ  DESC_LIMIT_CODEC2
; DESC_LIMIT_VIDEO2  equ  0x00000
; DESC_P    equ 0x8000  ;bit 15  段是否存在的检查结果cpu给的
; ;bit 14 13
; DESC_DPL_0 equ 0x0000 ;DPL bit14 13  (0--0)2 (0 00)16 程序的特权阶级
; DESC_DPL_1 equ 0x2000
; DESC_DPL_2 equ 0x4000
; DESC_DPL_3 equ 0x6000
; ;bit 12
; DESC_S_CODE equ 0x1000  ;S字段  0/1 系统段/数据段
; DESC_S_DATA equ DESC_S_CODE
; DESC_S_sys  equ 0x0000  ;
; ;bit 11-8
; DESC_TYPE_CODE equ 0x800 ;x=1 e=0 w=0 a=0  代码是可执行的 非一致 不可读，已访问位a清零 
; DESC_TYPE_DATA equ 0x200 ;  0   0   1   0   数据段是不可执行的 向上扩展 可写 已访问位a清零
; ------------------------------------------------------------------------
DESC_G_4K equ 0x1<<23   ;23bit offset from 0bit
DESC_D_32 equ 0x1<<22   ;22   D 1/0 操作数32/16bit选择位
DESC_L    equ 0x000000   ;21   是否是64位代码段 这里选择0 否 32位模式

;64位代码标记此处标记为０即可
DESC_AVL  equ 0x0<<20   ;20 CPU此位不用暂时为0
DESC_LIMIT_CODEC2  equ  0xf<<16  ;段的界限16-19
DESC_LIMIT_DATA2   equ  DESC_LIMIT_CODEC2
DESC_LIMIT_VIDEO2  equ  0x0<<16
DESC_P    equ 0x1<<15  ;bit 15  段是否存在的检查结果cpu给的
;bit 14 13
DESC_DPL_0 equ 0x0<<13 ;DPL bit14 13  (0--0)2 (0 00)16 程序的特权阶级
DESC_DPL_1 equ 0x1<<13
DESC_DPL_2 equ 0x2<<13
DESC_DPL_3 equ 0x3<<13
;bit 12
DESC_S_CODE equ 0x1<<12  ;S字段  0/1 系统段/数据段
DESC_S_DATA equ DESC_S_CODE
DESC_S_sys  equ 0x0<<12  ;
;bit 11-8
DESC_TYPE_CODE equ 0x8<<8 ;x=1 e=0 w=0 a=0  代码是可执行的 非一致 不可读，已访问位a清零 
DESC_TYPE_DATA equ 0x2<<8 ;  0   0   1   0   数据段是不可执行的 向上扩展 可写 已访问位a清零


DESC_CODE_HIGH4 equ (0x00<<24)+DESC_G_4K+DESC_D_32+\
DESC_L+DESC_AVL+DESC_LIMIT_CODEC2+\
DESC_P+DESC_DPL_0+DESC_S_CODE+\
DESC_TYPE_CODE+0x00

DESC_DATA_HIGH4 equ (0x00<<24)+DESC_G_4K+DESC_D_32+\
DESC_L+DESC_AVL+DESC_LIMIT_DATA2+\
DESC_P+DESC_DPL_0+DESC_S_DATA+\
DESC_TYPE_DATA+0x00

DESC_VIDEO_HIGH4 equ (0x00<<24)+DESC_G_4K+DESC_D_32+\
DESC_L+DESC_AVL+DESC_LIMIT_VIDEO2+\
DESC_DPL_0+DESC_P+DESC_S_DATA+\
DESC_TYPE_DATA+0x0b ;黑白模式下的b8000地址 高位b留在了高32位的控制字段上

;-------------选择子属性----------------
RPL0 equ 00b
RPL1 equ 01b
RPL2 equ 10b
RPL3 equ 11b

TI_GDT equ 000b
TI_LDT equ 100b

; -----------页相关属性--------------------------
PAGE_DIR_TABLE_POS equ 0x100000

PG_P equ 1b
PG_RW_R equ 00b
PG_RW_W EQU 10b
PG_US_S EQU 000B
PG_US_U EQU 100B

;-------------  program type 定义   --------------
PT_NULL equ 0