#include "bp_pub.h"


STATIC VOID __Dlist_add(IN BP_DLIST_S *new, IN BP_DLIST_S *prev, IN BP_DLIST_S *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

STATIC VOID __Dlist_del(IN BP_DLIST_S * prev, IN BP_DLIST_S * next)
{
	next->prev = prev;
	prev->next = next;
}

VOID BP_DLIST_Init(IN BP_DLIST_S *list)
{
	list->next = list;
	list->prev = list;
}

VOID BP_DList_Add(IN BP_DLIST_S *head, IN BP_DLIST_S *new)
{
	__Dlist_add(new, head, head->next);
}

VOID BP_DList_Add_Tail(IN BP_DLIST_S *head, IN BP_DLIST_S *new)
{
	__Dlist_add(new, head->prev, head);
}

VOID BP_DList_Del(IN BP_DLIST_S *entry)
{
	__Dlist_del(entry->prev, entry->next);
	entry->next = LIST_POISON1;
	entry->prev = LIST_POISON2;
}

INT BP_DList_Empty(IN const BP_DLIST_S *head)
{
	return head->next == head;
}



