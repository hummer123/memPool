#ifndef _MEM_POOL_H  
#define _MEM_POOL_H  

#include "bp_pub.h"
#include <pthread.h>

/* memory unit */
typedef struct stMemUnit
{
    BOOL_T  bIsUsed;
    U32     u32UsedSize;

    VOID    *pvData;
}MEM_UNIT_S;

/* memory block */
typedef struct stMemBlock
{
    BP_DLIST_S  stBlockList;
    
    U32         u32BlockLen;
    U32         u32nUnit;
    U32         u32nFree;
    VOID        *pvLastBUAddr;  /* last block unit addr */
    MEM_UNIT_S  *pstMemUnit;
}MEM_BLOCK_S;

/* memory pool */
typedef struct stMemPool
{
    pthread_rwlock_t    stRWlock;

    BOOL_T              bIsInit;  
    U32                 u32UnitSize;
    U32                 u32FirstBUCnt;
    U32                 u32GrowBUCnt;
    U32                 u32MaxBkCnt;

    U32                 u32BlockCnt;
    BP_DLIST_S          stBlockList;
}MEM_POOL_S;

#if 0
#endif

MEM_POOL_S *MEM_Pool_Create(IN U32 u32UnitSize, IN U32 u32FirstBUCnt, IN U32 u32MaxBKCnt);
VOID *MEM_Pool_Malloc(IN MEM_POOL_S *pstMemPool, IN U32 u32Size);
U32 MEM_Pool_Free(IN MEM_POOL_S *pstMemPool, INOUT UCHAR *pucData);
VOID MEM_Pool_AdjustMaxBlock(IN MEM_POOL_S *pstMemPool, IN U32 u32MaxBKCnt);
VOID MEM_Pool_Destroy(IN MEM_POOL_S *pstMemPool);
VOID MEM_Pool_UseInfo(IN MEM_POOL_S *pstMemPool);


#endif  /* _MEM_POOL_H */

