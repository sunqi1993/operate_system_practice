#include "init.h"
#include "asm_print.h"
#include "interrupt.h"
#include "timer.h"
#include "memory.h"

/*负责初始化所有模块 */
void init_all() {

#ifdef initAll
   put_str("init_all\n");
#endif
   idt_init();    // 初始化中断
   timer_init();  // 初始化PIT
   mem_init();	  // 初始化内存管理系统
}
