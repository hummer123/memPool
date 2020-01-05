#ifndef _BP_PUB_H  
#define _BP_PUB_H  

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef char                CHAR;
typedef unsigned char       UCHAR;
typedef unsigned char       U8;
typedef int                 INT;
typedef int                 BOOL_T;
typedef unsigned int        U32;
typedef long                LONG;
typedef unsigned long       ULONG;
typedef unsigned long long  U64;
typedef void                VOID;

#define STATIC  static              

#ifndef BOOL_TRUE
#define BOOL_TRUE  1
#endif

#ifndef BOOL_FALSE
#define BOOL_FALSE  0
#endif

#define IN
#define OUT
#define INOUT

#define LIST_POISON1  ((void *) 0x00100100)
#define LIST_POISON2  ((void *) 0x00200200)

/***************** public Error code ********************/
typedef enum Bp_ErrorCode
{
    BP_SUCCESS          = 0,
    BP_FAILED           = 1,

    BP_COMM_ERR         = 4,
    BP_NO_MEMORY        = 5,
    BP_SYSCALL_ERR      = 6,
    BP_INVALID_PARAM    = 7,   

    BP_BUTT_ERR
}BP_ERROR_CODE_E;

#define BP_VAL_EQU(a, b)        ((a) == (b))
#define BP_IS_NULL(a)           (NULL == (a))
#define BP_IS_ERR(u32Ret)       (BP_SUCCESS != (u32Ret))
#define BP_IS_SUCCESS(u32Ret)   (BP_SUCCESS == (u32Ret))


#define BP_Malloc(size)     malloc(size)
#define BP_Free(pucData)    do { free(pucData); pucData = NULL; } while (0)


#define BP_BYTES_ALIGN       (4)
#define BP_BYTES_ALIGNMENT(sp, bytes)        \
                    (UCHAR *)(((U32)(sp) + ((U32)bytes - 1)) & (~((U32)bytes - 1  )))

/****************** public function *********************/

typedef struct Bp_DList
{
	struct Bp_DList *next, *prev;
}BP_DLIST_S;

#define BP_DLIST_FOR_EACH(head, pos) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

#define BP_DLIST_FOR_EACH_SAFE(head, pos, n) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
            pos = n, n = pos->next)



VOID BP_DLIST_Init(IN BP_DLIST_S *list);
VOID BP_DList_Add(IN BP_DLIST_S *head, IN BP_DLIST_S *new);
VOID BP_DList_Add_Tail(IN BP_DLIST_S *head, IN BP_DLIST_S *new);   
VOID BP_DList_Del(IN BP_DLIST_S *entry);
INT BP_DList_Empty(IN const BP_DLIST_S *head);



#endif  /* _BP_PUB_H */

