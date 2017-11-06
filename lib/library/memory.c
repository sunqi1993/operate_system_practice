//
// Created by sunqi on 17-11-6.
//

#include "memory.h"
#include "stdint.h"
#include "print.h"

#define PG_SIZE 4096

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

