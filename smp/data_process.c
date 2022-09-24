#include "../data_process.h"

// HI_U32 g_u32BlkCnt = 4;
extern int g_count;
extern struct list_head RTPbuf_head;

void add_pkt_list(VENC_STREAM_S *pstStream)
{
    HI_S32 packIdx, packLength = 0;

    for (packIdx = 0; packIdx < pstStream->u32PackCount; packIdx++)
    {
        RTPbuf_s *p = (RTPbuf_s *)malloc(sizeof(RTPbuf_s));
        INIT_LIST_HEAD(&(p->list));

        packLength = pstStream->pstPack[packIdx].u32Len - pstStream->pstPack[packIdx].u32Offset;
        p->buf = (char *)malloc(packLength);
        p->len = packLength;
        memcpy(p->buf, pstStream->pstPack[packIdx].pu8Addr + pstStream->pstPack[packIdx].u32Offset,
               packLength);

        list_add_tail(&(p->list), &RTPbuf_head);
        g_count++;
        // printf("count = %d\n",g_count);
    }
}

HI_S32 add_stream_pack_to_list(VENC_STREAM_S *pstStream)
{

    HI_S32 packIdx, clientIdx, packLength = 0;

    add_pkt_list(pstStream);

    return HI_SUCCESS;
}