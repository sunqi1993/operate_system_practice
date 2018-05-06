//
// Created by sunqi on 18-5-3.
//

#ifndef SYSTEM_LIST_H
#define SYSTEM_LIST_H

#include "stdint.h"
#define NULL 0
//取结构体中某个成员的地址
#define offset(struct_type,member)  (uint32_t)(&((struct_type*)0)-> member)
#define elem2entry(struct_type, struct_member_name, elem_ptr)  \
            (struct_type*)((uint32_t)elem_ptr - offset(struct_type, struct_member_name))

//列表中的元素
typedef struct list_elem {
    struct list_elem* prev;
    struct list_elem* next;
}list_elem;

//列表的头部和尾部
typedef struct {
    list_elem head;
    list_elem tail;
}list;

typedef bool (function)(list_elem*,int argv);

void list_init(list*);
void list_insert_before(list_elem* before,list_elem* elem);
void list_push(list* plist,list_elem* elem);
void list_iterate(list* plist);
void list_append(list* plist,list_elem* elem);
void list_remove(list_elem* elem);
list_elem* list_pop(list* plist);
bool list_empty(list* plist);
uint32_t list_len(list* plist);
list_elem* list_traversal(list* list,function func,int arg);
bool elem_find(list* plist,list_elem* obj_elem);

#endif //SYSTEM_LIST_H
