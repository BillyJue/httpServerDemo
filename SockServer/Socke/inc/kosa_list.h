
#ifndef __KOSA_LIST_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define __KOSA_LIST_H__

#include "kosa.h"

/*����: ����MEMBER��Ա��TYPE�ṹ���е�ƫ����*/
#define my_offsetof(TYPE, MEMBER)  (unsigned long)(&(((TYPE*)0)->MEMBER))

/**********************************************************
����: ��������Ԫ�ص���ʼ��ַ
����:
    ptr��   type�ṹ���е�����ָ��
    type:   �ṹ������
    member: �����Ա����
**********************************************************/
#define container_of(ptr, type, member) \
            (type *)((char*)(ptr) - my_offsetof(type, member))

/**
* list_entry - get the struct for this entry
* @ptr:	the &struct list_head pointer.
* @type:	the type of the struct this is embedded in.
* @member:	the name of the list_struct within the struct.
*/
#define list_entry(ptr, type, member) \
            container_of(ptr, type, member)

/**
* list_for_each	-	iterate over a list
* @pos:	the &struct list_head to use as a loop cursor.
* @head:	the head for your list.
*/
#define list_for_each(pos, head) \
    for(pos=(head)->next; pos != (head); pos=pos->next)

/**
 * list_for_each_safe   -   iterate over a list safe against removal of list entry
 * @pos:    the &struct list_head to use as a loop counter.
 * @n:      another &struct list_head to use as temporary storage
 * @head:   the head for your list.
 */
#define list_for_each_safe(pos, n, head) \
    for(pos=(head)->next, n=pos->next; pos!=(head); \
        pos=n, n=pos->next)

#define list_for_each_safe_prev(pos, n, head) \
    for(pos=(head)->prev, n=pos->prev; pos!=(head); \
        pos=n, n=pos->prev)
        

struct list_head
{
    struct list_head* prev;
    struct list_head* next;
};

KOSA_API Void list_init(struct list_head* head);

KOSA_API Int32 list_empty(struct list_head* head);

KOSA_API Void list_add(struct list_head* newNode, struct list_head* head);

KOSA_API Void list_add_tail(struct list_head* newNode, struct list_head* head);

KOSA_API Void list_del(struct list_head* entry);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __KOSA_LIST_H__ */

