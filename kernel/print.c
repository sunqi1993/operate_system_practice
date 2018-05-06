//
// Created by sunqi on 18-5-5.
//

#include "print.h"
#include "debug.h"
void set_pos(uint8_t line,uint16_t number)
{
    ASSERT(((line-1)*80+number)<2000 &&(line<=45)&&(number<=80)&&(line>=1)&&(number>=1));
    set_screen_cursor((line-1)*80+number-1);
}