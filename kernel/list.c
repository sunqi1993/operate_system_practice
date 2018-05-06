//
// Created by sunqi on 18-5-3.
//

#include "list.h"
#include "interrupt.h"

void  list_init(list* plist)
{
    plist->head.prev=NULL;
    plist->head.next=&plist->tail;
    plist->tail.prev=&plist->head;
    plist->tail.next=NULL;
}

void list_insert_before(list_elem* before,list_elem* elem)
{
    enum intr_status old_status=intr_disable();

    before->prev->next=elem;
    elem->prev=before->prev;

    before->prev=elem;
    elem->next=before;

    intr_set_status(old_status);
}

//将元素放到栈的队首类似于push操作
void list_push(list* plist,list_elem* elem)
{
    list_insert_before(plist->head.next,elem);
}


//将元素插入到队尾部
void list_append(list* plist,list_elem* elem)
{
    list_insert_before(&plist->tail,elem);
}

//使得元素脱离链表
void list_remove(list_elem* elem)
{
    enum intr_status old_status=intr_disable();
    elem->prev->next=elem->next;
    elem->next->prev=elem->prev;
    intr_set_status(old_status);
}

//将第一个元素弹出并且返回
list_elem* list_pop(list* plist)
{
    list_elem* elem=plist->head.next;
    list_remove(elem);
    return elem;
}

bool elem_find(list* plist,list_elem* obj_elem)
{
    list_elem* elem=plist->head.next;
    while (elem!=&plist->tail)
    {
        if (elem==obj_elem) return true;
        else elem=elem->next;
    }
    return false;
}

list_elem* list_traversal(list* plist,function func,int arg)
{
    list_elem* elem=plist->head.next;
    if(elem==&plist->tail)
    {
        return NULL;
    }

    while (elem!=&plist->tail)
    {
        if(func(elem,arg)) return elem;
        else elem=elem->next;
    }
    return NULL;
}


uint32_t  list_len(list* plist)
{
    list_elem* elem=plist->head.next;
    uint32_t len=0;
    while (elem!=&plist->tail)
    {
        len++;
        elem=elem->next;
    }
    return len;
}


bool list_empty(list* plist)
{
    return (plist->head.next==&plist->tail?true:false);
}