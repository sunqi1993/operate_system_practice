//
// Created by sunqi on 17-11-6.
//

#include <stddef.h>
#include <debug.h>
#include "memory.h"
#include "stdint.h"
#include "print.h"
#include "string.h"

#define PG_SIZE 4096
//地址线的最高10位表示页目录表
#define PDE_IDX(addr)  ((addr&0xffc00000)>>22)
//地址线的12-23位表示 页的序列偏移地址
#define PTE_IDX(addr)  ((addr&0x3ff000)>>12)


/*位图地址：
 * 因为0xc009f000是内核主线程栈顶，0xc009e000是内核主线程的pcb
 * 一个页框的大小的位图可以表示128M内存 位图地址安置在地址0xc009a000
 *本系统最大支持四个框的位图 :512M
 */


/*
 *  0xc0000000 是内核地址从虚拟地址3G起
 *  0x100000 意指跨过低端1M内存 使得地址在1M内连续
 */

#define K_HEAP_START 0XC0100000
#define MEM_BITMAP_BASE 0XC009A000

/*虚拟地址池 用于虚拟地址管理*/
struct virtual_addr{
    struct bitmap vaddr_bitmap;   //虚拟地址用到的位图结构
    uint32_t vaddr_start;         //虚拟地址起始数值
};

/*内存池结构用于生成两个实例用于管理内核内存池和用户内存池*/
struct pool{
    struct bitmap pool_bitmap; //本内存池用到的位图结构
    uint32_t phy_addr_start;   //本内存池所管理的物理内存的起始数值
    uint32_t pool_size;        //本内存池的直接容量
};

struct pool kernel_pool,user_pool;  //定义内核内存池和用户内存池
struct virtual_addr kernel_vaddr;   //此结构用来给内核分配虚拟地址


/*初始化内存池*/
static void mem_pool_init(uint32_t all_mem)
{
    put_str("mem_pool_init start!\n");

    /* 页表的大小 1页的目录表+第0-768个页目录项指向同一个页表
     * 769-1022页目录项共指向254个页表，一共256个页框
     */

    uint32_t page_table_size=PG_SIZE*256;

    uint32_t used_mem=page_table_size+0x100000;  //0x10000为低端的1M内存
    uint32_t free_mem=all_mem-used_mem;
    uint16_t all_free_pages= free_mem/PG_SIZE;

    uint16_t kernel_free_pages=all_free_pages/2;
    uint16_t user_free_pages=all_free_pages-kernel_free_pages;

    /*为了简化位图操作 余数不做处理 这样的坏处是会丢内存 好处是 不用做内存的月结检查 因为位图表示的内存小鱼实际内存*/
    uint32_t kbm_length=kernel_free_pages/8;   //一位表示一页 以直接为单位 kernel_bitmap的长度
    uint32_t ubm_length=user_free_pages/8;     // User_bitmap的长度

    //内核内存池的起始地址
    uint32_t kp_start =used_mem;
    //用户内存池的起始地址
    uint32_t up_start =kp_start+kernel_free_pages*PG_SIZE;

    kernel_pool.phy_addr_start=kp_start;
    user_pool.phy_addr_start=up_start;

    kernel_pool.pool_size=kernel_free_pages*PG_SIZE;
    user_pool.pool_size=user_free_pages*PG_SIZE;

    kernel_pool.pool_bitmap.bitmap_bytes_len=kbm_length;
    user_pool.pool_bitmap.bitmap_bytes_len=ubm_length;

    /*
     * 内核内存池和用户内存池的位图
     * 位图是全局的数据 长度不固定。
     * 全局或静态的数组需要在编译的时候知道其长度
     * 而我们需要根据内存的大小计算处需要多少字节，所以改为指定的一块内存来生成位图
     *
     * 内核使用的最高地址是0xc009f000 这是主线程的栈地址
     * 内核的大小预计为70kb
     * 32M内存占用的位图为1kb
     *
     * 内核内存池的位图定义在0xc009a000处 0xc009a000-0xc009e000 预留4*4K位图空间 512M的内存位图便于管理
     */
    kernel_pool.pool_bitmap.bits=(void*)MEM_BITMAP_BASE;
    /*用户内存池紧跟在内核内存池后面*/
    user_pool.pool_bitmap.bits=(void*)(MEM_BITMAP_BASE+kbm_length);

    /*输出内存池信息*/
    put_str("\nkernel_pool bitmap start:");
    put_int((int)kernel_pool.pool_bitmap.bits);
    put_str(" kernel_pool_phy_addr start:");
    put_int((int)kernel_pool.phy_addr_start);
    put_str("\n");

    put_str("user_pool bitmap start:");
    put_int((int)user_pool.pool_bitmap.bits);
    put_str(" user_pool_phy_addr_start:");
    put_int((int)user_pool.phy_addr_start);
    put_str("\n");

    /*将位图置0*/
    bitmap_init(&kernel_pool.pool_bitmap);
    bitmap_init(&user_pool.pool_bitmap);

    /*下面初始化虚拟内核的地址位图按实际的物理大小生成数组*/
    kernel_vaddr.vaddr_bitmap.bitmap_bytes_len=kbm_length;

    /*位图的数组指向一块为使用的位图*/
    kernel_vaddr.vaddr_bitmap.bits=(void*)(MEM_BITMAP_BASE+kbm_length+ubm_length);

    kernel_vaddr.vaddr_start=K_HEAP_START;

    bitmap_init(&kernel_vaddr.vaddr_bitmap);
    put_str("   mem_pool_init done!\n");

}


void mem_init()
{
    put_str("mem_init start\n");
    uint32_t mem_bytes_total=(*(uint32_t*)(0xb00));
    mem_pool_init(mem_bytes_total);
    put_str("mem_init done!\n");
}

/*
 * pf表示在虚拟内存池中申请pg_cnt个虚拟页
 * 成功则返回虚拟页的起始地址，失败则返回NULL
 */
static void* vaddr_get(enum pool_flags pf,uint32_t pg_cnt)
{
    int vaddr_start=0,bit_idx_start=-1;
    uint32_t  cnt=0;
    if (pf==PF_KERNEL)
    {
        bit_idx_start=bitmap_check_bit_idx(&kernel_vaddr.vaddr_bitmap,pg_cnt);
        if(bit_idx_start==-1)
            return NULL;

        while (cnt<pg_cnt)
        {
            bitmap_set(&kernel_vaddr.vaddr_bitmap,bit_idx_start+cnt++,1);
        }
        //计算出申请的空间的地址
        vaddr_start=kernel_vaddr.vaddr_start+bit_idx_start*PG_SIZE;
        #ifdef DEBUG
        put_str("vaddr_get info: bit_idx vaddr");
        put_int(bit_idx_start);
        put_str("  ");
        put_int(vaddr_start);
        put_str("\n");
        #endif

    } else{
        //用户内存池 将来实现用户进程再补充

    }
    return (void*)vaddr_start;
}

/*得到虚拟地址vaddr对应的pte指针*/
uint32_t* pte_ptr(uint32_t vaddr)
{
    /*
     * 先把页目录当成也页表访问 找出原地址对应的页表地址
     * */
    uint32_t* pte=(uint32_t*)(0xffc00000+\
                            ((vaddr&0xffc00000)>>10)+\
                             PTE_IDX(vaddr)*4
                            );
    return pte;
}

/*得到vaddr所对应的PDE指针*/
uint32_t* pde_ptr(uint32_t  vaddr)
{
    uint32_t* pde=(uint32_t*)(0xfffff000+ PDE_IDX(vaddr)*4);
    return  pde;
}

/*在m_pool指向的污泥内存池中分配一个物理内存页
 * 成功页返回页框的污泥地址 失败则返回NULL
 * */

static void* palloc(struct pool* m_pool)
{
    /*扫描或者设置位图要保证原子操作*/
    int bit_idx=bitmap_scan(&m_pool->pool_bitmap,1);
    if(bit_idx==-1)
    {
        return NULL;
    }
    bitmap_set(&m_pool->pool_bitmap,bit_idx,1);    //将此处bit_idx设置为占用
    uint32_t page_phyaddr=(bit_idx*PG_SIZE)+m_pool->phy_addr_start;
    return (void*)page_phyaddr;
}

/*
 * 在页表中添加虚拟地址_vaddr与物理地址_page_phyaddr的映射
 * */

static void page_table_map(void* _vaddr,void* _page_phyaddr)
{
    uint32_t vaddr=_vaddr;
    uint32_t page_phyaddr=_page_phyaddr;
    uint32_t *pte=pte_ptr(vaddr);
    uint32_t *pde=pde_ptr(vaddr);


#ifdef DEBUG
put_str("\npage_table_map params:vaddr page_phyaddr *pde *pte \n");
put_int((int)vaddr);
    put_str(" ");
    put_int((uint32_t)page_phyaddr);
    put_str(" ");
    put_int((uint32_t)pde);
    put_str(" ");
    put_int((uint32_t)pte);
    put_str("\n");
    #endif

    /*
     * 注意执行 *pte会访问pde表 所以请确保要pde创建完成以后才能执行 *pte操作
     * 否者会引发page_fault错误 因此在 *pde为0的时候 *pte只能出现在它的else语句中
     */
    /*所在目录 判断目录项的p位是否是1，若为1表示该表已经存在*/
    if(*pde & 0x00000001)
    {
        ASSERT(!(*pte&0x00000001));
        //页目录项和页表项的P位为1 此处判断页目录项是否存在


        if(!(*pte&0x00000001))
        {
            //页表不存在的话就需要创建页表
            // US=1 RW=1 P=1
            *pte=(page_phyaddr|PG_US_S|PG_RW_W|PG_P_1);

        } else{
            //目前不会执行到这里 因为前面有个断言函数 判断是否会被占用
            PANIC("PTE retreat");
            *pte=(page_phyaddr|PG_US_S|PG_RW_W|PG_P_1);
        }

    } else{
        //页目录项不存在所以先创建页目录项 在创建页表项
        uint32_t pde_phyaddr=(uint32_t)palloc(&kernel_pool);
        *pde=(page_phyaddr|PG_US_S|PG_RW_W|PG_P_1);
        //将页表对应的地址的整个4K数据都清除为0
        memset((void*)((int)pte & 0xfffff000),0,PG_SIZE);
        ASSERT(!(*pte&0x00000001));
        //既然页目录不存在 就往页表上随意写地址了
        *pte=(page_phyaddr|PG_US_U,PG_RW_W|PG_P_1);
    }
}

/*分配pg_cnt个页空间 成功则返回起始虚拟地址 失败则返回NULL*/

void* malloc_page(enum pool_flags pf,uint32_t pg_cnt)
{
    ASSERT(pg_cnt>0&&pg_cnt<3840);
    /**************malloc_page的原理是三个动作的合成*******************
     *1.通过vaddr_get在虚拟内存池中生气虚拟地址
     *2.palloc在物理内存池中申请物理页
     *3.通过map映射虚拟地址和物理地址之间的联系
     */
    void* vaddr_start=vaddr_get(pf,pg_cnt);
    if(vaddr_start==NULL)
    {
        return NULL;
    }
    uint32_t vaddr=(uint32_t)vaddr_start,cnt=pg_cnt;
    struct pool* mem_pool=pf&PF_KERNEL? &kernel_pool: &user_pool;
    while (cnt-->0)
    {
        void* page_phyaddr=palloc(mem_pool);
        if(page_phyaddr==NULL)
        {
            //失败的时候要将已经申请的虚拟地址和物理页全部回滚，在将来完成内存回收的时候再进行补充
            return NULL;
        }
        #ifdef DEBUG
        put_str("malloc_page params:");
        put_int(vaddr);
        put_str(" ");
        put_int((int32_t)page_phyaddr);
        put_str(" \n");
        #endif

        page_table_map((void*)vaddr,page_phyaddr);
        vaddr+=PG_SIZE;     //下一个虚拟页
    }
    return vaddr_start;
}

/*从内核的物理内存池中申请内存，成功则返回其虚拟地址 失败则返回NULL*/

void* get_kernel_pages(uint32_t pg_cnt)
{
    put_str("get_kernel_pages start\n");
    void* vaddr=malloc_page(PF_KERNEL,pg_cnt);
    put_str("get_kernel_pages: vaddr=");
    put_int((uint32_t)vaddr);
    put_str("\n");
    if(vaddr!=NULL)
    {
        memset(vaddr,0,pg_cnt*PG_SIZE);
    }
    return vaddr;
}