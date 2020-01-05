#include "bp_pub.h"
#include "mempool.h"


/* memroy test */
typedef struct stMemTest
{
    U32         u32SizeLen;
    UCHAR       *pucData;
}MEM_TEST_S;


int main()
{
    /* 1024 * 1024     = 1048576 
     * 1024 * 1024 * 2 = 2097152
     * 1024 * 1024 * 3 = 3145728
    */
    MEM_TEST_S stMemTest[20] = {{1024, NULL}, {2048, NULL}, {4096, NULL}, {1048576, NULL}, {1000500, NULL}, \
                            {2097152, NULL}, {2090000, NULL}, {3145728, NULL}, {3100000, NULL}, {2048, NULL}, \
                            {3100000, NULL}, {3145728, NULL}, {2097152, NULL}, {1048576, NULL}, {4096, NULL}, \
                            {2048, NULL}, {3245728, NULL}, {4096, NULL}, {1024, NULL}, {5678, NULL}};
    
    MEM_POOL_S *pstMemPool = NULL;
    U32 i;
    U32 u32Ret = BP_SUCCESS;
    
    pstMemPool = MEM_Pool_Create(1024 * 1024 * 3, 18, 1);
    if (BP_IS_NULL(pstMemPool))
    {
        printf(">>>>>> Failed to Create Mem pool.\n");
        return BP_NO_MEMORY;
    }

    for (i = 0; i < (sizeof(stMemTest) / sizeof(stMemTest[0])); i++)
    {
        stMemTest[i].pucData = MEM_Pool_Malloc(pstMemPool, stMemTest[i].u32SizeLen);
        if (BP_IS_NULL(stMemTest[i].pucData))
        {
            printf(">>>>>> Failed to Malloc, size[%u].\n", stMemTest[i].u32SizeLen);
        }
    }
    MEM_Pool_UseInfo(pstMemPool);

    for (i = 0; i < sizeof(stMemTest) / sizeof(stMemTest[0]); i++)
    {
        u32Ret = MEM_Pool_Free(pstMemPool, stMemTest[i].pucData);
        if(BP_IS_ERR(u32Ret))
        {
            printf(">>>>>> Failed to free memory, size[%u], memAddr[%p].\n", \
                    stMemTest[i].u32SizeLen, stMemTest[i].pucData);
        }
    }
    MEM_Pool_UseInfo(pstMemPool);

    MEM_Pool_AdjustMaxBlock(pstMemPool, 2);
    for (i = 0; i < (sizeof(stMemTest) / sizeof(stMemTest[0])); i++)
    {
        stMemTest[i].pucData = MEM_Pool_Malloc(pstMemPool, stMemTest[i].u32SizeLen);
        if (BP_IS_NULL(stMemTest[i].pucData))
        {
            printf(">>>>>> Failed to Malloc, size[%u].\n", stMemTest[i].u32SizeLen);
        }
    }
    MEM_Pool_UseInfo(pstMemPool);

    MEM_Pool_AdjustMaxBlock(pstMemPool, 1);
    for (i = 0; i < sizeof(stMemTest) / sizeof(stMemTest[0]); i++)
    {
        u32Ret = MEM_Pool_Free(pstMemPool, stMemTest[i].pucData);
        if(BP_IS_ERR(u32Ret))
        {
            printf(">>>>>> Failed to free memory, size[%u], memAddr[%p].\n", \
                    stMemTest[i].u32SizeLen, stMemTest[i].pucData);
        }
    }
    MEM_Pool_UseInfo(pstMemPool);
    
    MEM_Pool_Destroy(pstMemPool);
    
    return BP_SUCCESS;
}


