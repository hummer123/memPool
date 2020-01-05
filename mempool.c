#include "mempool.h"


/* 内部函数，参数有效性由调用者保证 */
STATIC MEM_BLOCK_S *__Mem_Pool_CreateBlock(IN MEM_POOL_S *pstMemPool)
{
    U32 i;
    U32 u32MemLen  = 0;
    U32 u32UnitCnt = 0;
    UCHAR       *pucSP    = NULL;
    MEM_BLOCK_S *pstBlock = NULL;

    /* u32MemLen = sizeof(block) + unitCnt * (unitSize + sizeof(unit)) + BP_BYTES_ALIGN(字节对齐) */
    u32UnitCnt = (BP_VAL_EQU(0, pstMemPool->u32BlockCnt) ? (pstMemPool->u32FirstBUCnt) : (pstMemPool->u32GrowBUCnt));
    u32MemLen = sizeof(MEM_BLOCK_S) + (u32UnitCnt * (pstMemPool->u32UnitSize + sizeof(MEM_UNIT_S))) + BP_BYTES_ALIGN;
    
    pucSP = (UCHAR *)BP_Malloc(u32MemLen);
    if (BP_IS_NULL(pucSP))
    {
        printf(">>>>>> Failed to malloc block memory.\n");
        return NULL;
    }
    
    pstBlock = (MEM_BLOCK_S *)pucSP;
    pstBlock->u32nUnit    = u32UnitCnt;
    pstBlock->u32nFree    = u32UnitCnt;
    pstBlock->u32BlockLen = u32MemLen;
    BP_DLIST_Init(&(pstBlock->stBlockList));

    pucSP += sizeof(MEM_BLOCK_S);
    pstBlock->pstMemUnit = (MEM_UNIT_S *)pucSP;
    pucSP += u32UnitCnt * sizeof(MEM_UNIT_S);
    /* 确保 BP_BYTES_ALIGN 字节对齐   */
    pucSP = BP_BYTES_ALIGNMENT(pucSP, BP_BYTES_ALIGN);
    
    for (i = 0; i < u32UnitCnt; i++)
    {
        pstBlock->pstMemUnit[i].bIsUsed     = BOOL_FALSE;
        pstBlock->pstMemUnit[i].u32UsedSize = 0;
        pstBlock->pstMemUnit[i].pvData      = (VOID *)(pucSP + i * pstMemPool->u32UnitSize);
    }
    pstBlock->pvLastBUAddr = (pstBlock->pstMemUnit[u32UnitCnt - 1].pvData);

    return pstBlock;
}

/* 内部函数，参数有效性由调用者保证 */
STATIC BOOL_T __MEM_Pool_CanReleaseBLock(IN MEM_POOL_S *pstMemPool, IN MEM_BLOCK_S *pstMemBlock)
{
    if ((pstMemPool->u32BlockCnt > pstMemPool->u32MaxBkCnt) && \
        (pstMemPool->u32FirstBUCnt != pstMemBlock->u32nUnit) && \
        BP_VAL_EQU(pstMemBlock->u32nFree, pstMemBlock->u32nUnit))
    {
        return BOOL_TRUE;
    }

    return BOOL_FALSE;
}

/* 内部函数，参数有效性由调用者保证 */
STATIC VOID *__Mem_Pool_MallocInBlock(IN MEM_BLOCK_S *pstBlock, IN U32 u32Size)
{
    U32   i;
    VOID *pvSP = NULL;

    for (i = 0; i < pstBlock->u32nUnit; i++)
    {
        if (BP_VAL_EQU(BOOL_FALSE, pstBlock->pstMemUnit[i].bIsUsed))
        {
            pstBlock->pstMemUnit[i].bIsUsed     = BOOL_TRUE;
            pstBlock->pstMemUnit[i].u32UsedSize = u32Size;
            pstBlock->u32nFree--;
            pvSP = pstBlock->pstMemUnit[i].pvData;
            
            break;
        }
    }

    return pvSP;
}

/* 内部函数，参数有效性由调用者保证 */
STATIC U32 __Mem_Pool_FreeInBlock(IN MEM_BLOCK_S *pstBlock, IN VOID *pvData)
{
    U32 i;

    /* Free memory addr is in this block */
    if ((pstBlock->pstMemUnit[0].pvData > pvData) || (pstBlock->pvLastBUAddr < pvData))
    {
        return BP_COMM_ERR;
    }

    for (i = 0; i < pstBlock->u32nUnit; i++)
    {
       if (BP_VAL_EQU(pstBlock->pstMemUnit[i].pvData, pvData))
       {
            pstBlock->pstMemUnit[i].bIsUsed     = BOOL_FALSE;
            pstBlock->pstMemUnit[i].u32UsedSize = 0;
            pstBlock->u32nFree++;
            return BP_SUCCESS;
       }
    }

    return BP_COMM_ERR;
}

/* 内部函数，参数有效性由调用者保证 */
STATIC U32 __Mem_Pool_StatsBlockUsed
(
    IN MEM_POOL_S *pstMemPool,
    IN MEM_BLOCK_S *pstBlock
)
{
    U32 i;
    U32 u32MemUsed = sizeof(MEM_BLOCK_S) + pstBlock->u32nUnit * sizeof(MEM_UNIT_S) + BP_BYTES_ALIGN;

    printf("###### bIsUsed      u32UsedSize   pBegAddr    pEndAddr\n");
    for (i = 0; i < pstBlock->u32nUnit; i++)
    {
       if (BP_VAL_EQU(BOOL_TRUE, pstBlock->pstMemUnit[i].bIsUsed))
       {
            u32MemUsed += pstBlock->pstMemUnit[i].u32UsedSize;
       }
       
       printf("------    %01d          %08u    %08p   %08p\n", \
            pstBlock->pstMemUnit[i].bIsUsed, pstBlock->pstMemUnit[i].u32UsedSize, \
            pstBlock->pstMemUnit[i].pvData, \
            (pstBlock->pstMemUnit[i].pvData + pstMemPool->u32UnitSize));
    }

    return u32MemUsed;
}

#if 0
#endif

MEM_POOL_S *MEM_Pool_Create
(
    IN U32 u32UnitSize,
    IN U32 u32FirstBUCnt,
    IN U32 u32MaxBKCnt
)
{
    MEM_POOL_S *pstMemPool = NULL;

    if ((1 > u32UnitSize) || (1 > u32FirstBUCnt) || (1 > u32MaxBKCnt))
    {
        printf(">>>>>> Memory pool init input parameter is invalid.\n");
        return NULL;
    }

    pstMemPool = (MEM_POOL_S *)BP_Malloc(sizeof(MEM_POOL_S));
    if (BP_IS_NULL(pstMemPool))
    {
        printf(">>>>>> Failed to malloc.\n");
        return NULL;
    }

    pstMemPool->bIsInit         = BOOL_TRUE;
    pstMemPool->u32BlockCnt     = 0;
    pstMemPool->u32MaxBkCnt     = u32MaxBKCnt;
    pstMemPool->u32UnitSize     = u32UnitSize;
    pstMemPool->u32FirstBUCnt   = u32FirstBUCnt;
    pstMemPool->u32GrowBUCnt    = (u32FirstBUCnt >> 1);
    
    BP_DLIST_Init(&pstMemPool->stBlockList);
    (VOID)pthread_rwlock_init(&pstMemPool->stRWlock, NULL);

    return pstMemPool;
}

VOID *MEM_Pool_Malloc(IN MEM_POOL_S *pstMemPool, IN U32 u32Size)
{
    VOID        *pvSP      = NULL;
    BP_DLIST_S  *pstNode    = NULL;
    BP_DLIST_S  *pstTmpNode = NULL;
    MEM_BLOCK_S *pstBlock   = NULL;

    if (BP_IS_NULL(pstMemPool) || (pstMemPool->u32UnitSize < u32Size))
    {
        printf(">>>>>> Invalid paramete.\n");
        return NULL;
    }

    pthread_rwlock_wrlock(&pstMemPool->stRWlock);
    if (BP_VAL_EQU(BOOL_FALSE, pstMemPool->bIsInit))
    {
        pthread_rwlock_unlock(&pstMemPool->stRWlock);
        printf(">>>>>> Mem Pool Not Init.\n");
        return NULL;
    }
    
    BP_DLIST_FOR_EACH_SAFE(&pstMemPool->stBlockList, pstNode, pstTmpNode)
    {
        pstBlock = (MEM_BLOCK_S *)pstNode;
        pvSP = __Mem_Pool_MallocInBlock(pstBlock, u32Size);
        if (!BP_IS_NULL(pvSP))
        {
            pthread_rwlock_unlock(&pstMemPool->stRWlock);
            return pvSP;
        }
    }

    /* Is create block */
    if (BP_IS_NULL(pvSP) && (pstMemPool->u32MaxBkCnt > pstMemPool->u32BlockCnt))
    {
        pstBlock = __Mem_Pool_CreateBlock(pstMemPool);
        if (BP_IS_NULL(pstBlock))
        {
            printf(">>>>>> Mem Pool Malloc Block Error.\n");
            pthread_rwlock_unlock(&pstMemPool->stRWlock);
            return pvSP;
        }

        BP_DList_Add_Tail(&pstMemPool->stBlockList, &pstBlock->stBlockList);
        pstMemPool->u32BlockCnt++;
        pstBlock->u32nFree--;
        pstBlock->pstMemUnit[0].bIsUsed     = BOOL_TRUE;
        pstBlock->pstMemUnit[0].u32UsedSize = u32Size;
        pvSP = pstBlock->pstMemUnit[0].pvData;
    }
    pthread_rwlock_unlock(&pstMemPool->stRWlock);
    
    return pvSP;
}

U32 MEM_Pool_Free(IN MEM_POOL_S *pstMemPool, INOUT UCHAR *pucData)
{
    U32 u32Ret              = BP_SUCCESS;
    BP_DLIST_S  *pstNode    = NULL;
    BP_DLIST_S  *pstTmpNode = NULL;
    MEM_BLOCK_S *pstBlock   = NULL;

    if (BP_IS_NULL(pstMemPool) || BP_IS_NULL(pucData))
    {
        printf(">>>>>> Invalid paramete.\n");
        return BP_INVALID_PARAM;
    }

    pthread_rwlock_wrlock(&pstMemPool->stRWlock);
    if (BP_VAL_EQU(BOOL_FALSE, pstMemPool->bIsInit))
    {
        pthread_rwlock_unlock(&pstMemPool->stRWlock);
        printf(">>>>>> Mem Pool Not Init.\n");
        return BP_COMM_ERR;
    }
    
    BP_DLIST_FOR_EACH_SAFE(&pstMemPool->stBlockList, pstNode, pstTmpNode)
    {
        pstBlock = (MEM_BLOCK_S *)pstNode;
        u32Ret = __Mem_Pool_FreeInBlock(pstBlock, (VOID *)pucData);
        if (BP_IS_SUCCESS(u32Ret))
        {
            pucData = NULL;
            if(__MEM_Pool_CanReleaseBLock(pstMemPool, pstBlock))
            {
                BP_DList_Del(pstNode);
                BP_Free(pstNode);
                pstMemPool->u32BlockCnt--;
            }
            pthread_rwlock_unlock(&pstMemPool->stRWlock);
            return BP_SUCCESS;
        }
    }
    pthread_rwlock_unlock(&pstMemPool->stRWlock);

    return BP_COMM_ERR;
}

VOID MEM_Pool_AdjustMaxBlock(IN MEM_POOL_S *pstMemPool, IN U32 u32MaxBKCnt)
{
    BP_DLIST_S *pstNode    = NULL;
    BP_DLIST_S *pstTmpNode = NULL;

    if (BP_IS_NULL(pstMemPool) || (1 > u32MaxBKCnt))
    {
        printf(">>>>>> Invalid paramete.\n");
        return;
    }

    pthread_rwlock_wrlock(&pstMemPool->stRWlock);
    if (BP_VAL_EQU(BOOL_FALSE, pstMemPool->bIsInit))
    {
        pthread_rwlock_unlock(&pstMemPool->stRWlock);
        printf(">>>>>> Mem Pool Not Init.\n");
        return;
    }
    
    pstMemPool->u32MaxBkCnt = u32MaxBKCnt;
    pthread_rwlock_unlock(&pstMemPool->stRWlock);

    return;
}

VOID MEM_Pool_Destroy(IN MEM_POOL_S *pstMemPool)
{
    BP_DLIST_S *pstNode    = NULL;
    BP_DLIST_S *pstTmpNode = NULL;

    if (BP_IS_NULL(pstMemPool))
    {
        printf(">>>>>> Invalid paramete.\n");
        return;
    }

    pthread_rwlock_wrlock(&pstMemPool->stRWlock);
    BP_DLIST_FOR_EACH_SAFE(&pstMemPool->stBlockList, pstNode, pstTmpNode)
    {
        BP_DList_Del(pstNode);
        BP_Free(pstNode);        
    }
    pthread_rwlock_unlock(&pstMemPool->stRWlock);

    pthread_rwlock_destroy(&pstMemPool->stRWlock);
    BP_Free(pstMemPool);

    return;
}

VOID MEM_Pool_UseInfo(IN MEM_POOL_S *pstMemPool)
{
    U32 u32MemLen     = 0;
    U32 u32UnitCnt    = 0;
    U32 u32MemUsedLen = 0;

    BP_DLIST_S  *pstNode    = NULL;
    BP_DLIST_S  *pstTmpNode = NULL;
    MEM_BLOCK_S *pstBlock   = NULL;

    if (BP_IS_NULL(pstMemPool))
    {
        printf(">>>>>> Invalid paramete.\n");
        return;
    }

    pthread_rwlock_rdlock(&pstMemPool->stRWlock);
    if (BP_VAL_EQU(BOOL_FALSE, pstMemPool->bIsInit))
    {
        pthread_rwlock_unlock(&pstMemPool->stRWlock);
        printf(">>>>>> Mem Pool Not Init.\n");
        return;
    }

    printf("\n====== MemPool Info.\n");
    printf("====== bIsInit   u32UnitSize   u32FirstBUCnt   u32GrowBUCnt   u32MaxBkCnt   u32BlockCnt\n");
    printf("======    %d        %u          %u               %u             %u            %u\n", \
                    pstMemPool->bIsInit, pstMemPool->u32UnitSize, pstMemPool->u32FirstBUCnt, \
                    pstMemPool->u32GrowBUCnt, pstMemPool->u32MaxBkCnt, pstMemPool->u32BlockCnt);

    if (0 < pstMemPool->u32BlockCnt)
    {
        printf("====== u32BlockLen   u32nUnit   u32nFree   pvLastBUAddr\n");
    }

    /*
     * block info: u32BlockLen    u32nUnit    u32nFree    pvLastBUAddr 
     * Unit  info: bIsUsed      u32UsedSize   pBegAddr    pEndAddr 
    */
    
    u32MemLen     = sizeof(MEM_POOL_S);
    u32MemUsedLen = u32MemLen;
    BP_DLIST_FOR_EACH_SAFE(&pstMemPool->stBlockList, pstNode, pstTmpNode)
    {
        pstBlock = (MEM_BLOCK_S *)pstNode;
        printf("******  %u        %u        %u         %08p\n", \
                    pstBlock->u32BlockLen, pstBlock->u32nUnit, \
                    pstBlock->u32nFree, pstBlock->pvLastBUAddr);
                
        u32MemLen += pstBlock->u32BlockLen;
        u32MemUsedLen += __Mem_Pool_StatsBlockUsed(pstMemPool, pstBlock);
    }
    pthread_rwlock_unlock(&pstMemPool->stRWlock);

    printf(">>>>>> Memory pool usage[%u / 100], u32MemUsedLen[%u], u32MemLen[%u].\n\n", \
            ((u32MemUsedLen * 10) / (u32MemLen / 10)), u32MemUsedLen, u32MemLen);

    return;
}






