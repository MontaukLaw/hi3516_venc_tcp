#include "../my_vi.h"

HI_S32 start_dev(SAMPLE_VI_INFO_S *pstViInfo)
{
    HI_S32 s32Ret;
    VI_DEV ViDev;
    SAMPLE_SNS_TYPE_E enSnsType;
    VI_DEV_ATTR_S stViDevAttr;

    ViDev = pstViInfo->stDevInfo.ViDev;
    enSnsType = pstViInfo->stSnsInfo.enSnsType;

    SAMPLE_COMM_VI_GetDevAttrBySns(enSnsType, &stViDevAttr);
    stViDevAttr.stWDRAttr.enWDRMode = pstViInfo->stDevInfo.enWDRMode;
    if (VI_PARALLEL_VPSS_OFFLINE == pstViInfo->stPipeInfo.enMastPipeMode || VI_PARALLEL_VPSS_PARALLEL == pstViInfo->stPipeInfo.enMastPipeMode)
    {
        stViDevAttr.enDataRate = DATA_RATE_X2;
    }

    s32Ret = HI_MPI_VI_SetDevAttr(ViDev, &stViDevAttr);

    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_VI_SetDevAttr failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VI_EnableDev(ViDev);

    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_VI_EnableDev failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 bind_pip_dev(SAMPLE_VI_INFO_S *pstViInfo)
{
    HI_S32 i;
    HI_S32 s32PipeCnt = 0;
    HI_S32 s32Ret;
    VI_DEV_BIND_PIPE_S stDevBindPipe = {0};

    for (i = 0; i < 4; i++)
    {
        if (pstViInfo->stPipeInfo.aPipe[i] >= 0 && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM)
        {
            stDevBindPipe.PipeId[s32PipeCnt] = pstViInfo->stPipeInfo.aPipe[i];
            s32PipeCnt++;
            stDevBindPipe.u32Num = s32PipeCnt;
        }
    }

    s32Ret = HI_MPI_VI_SetDevBindPipe(pstViInfo->stDevInfo.ViDev, &stDevBindPipe);

    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_VI_SetDevBindPipe failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return s32Ret;
}

HI_S32 start_vi_pipe(SAMPLE_VI_INFO_S *pstViInfo)
{
    HI_S32 i, j;
    HI_S32 s32Ret = HI_SUCCESS;
    VI_PIPE ViPipe;
    VI_PIPE_ATTR_S stPipeAttr;

    for (i = 0; i < WDR_MAX_PIPE_NUM; i++)
    {
        if (pstViInfo->stPipeInfo.aPipe[i] >= 0 && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM)
        {
            ViPipe = pstViInfo->stPipeInfo.aPipe[i];
            SAMPLE_COMM_VI_GetPipeAttrBySns(pstViInfo->stSnsInfo.enSnsType, &stPipeAttr);
            if (HI_TRUE == pstViInfo->stPipeInfo.bIspBypass)
            {
                stPipeAttr.bIspBypass = HI_TRUE;
                stPipeAttr.enPixFmt = pstViInfo->stPipeInfo.enPixFmt;
                stPipeAttr.enBitWidth = DATA_BITWIDTH_8;
            }

            if ((2 == ViPipe) || (3 == ViPipe))
            {
                stPipeAttr.enCompressMode = COMPRESS_MODE_NONE;
            }

            if ((pstViInfo->stSnapInfo.bSnap) && (pstViInfo->stSnapInfo.bDoublePipe) && (ViPipe == pstViInfo->stSnapInfo.SnapPipe))
            {
                s32Ret = HI_MPI_VI_CreatePipe(ViPipe, &stPipeAttr);
                if (s32Ret != HI_SUCCESS)
                {
                    SAMPLE_PRT("HI_MPI_VI_CreatePipe failed with %#x!\n", s32Ret);
                    goto EXIT;
                }
            }
            else
            {
                s32Ret = HI_MPI_VI_CreatePipe(ViPipe, &stPipeAttr);
                if (s32Ret != HI_SUCCESS)
                {
                    SAMPLE_PRT("HI_MPI_VI_CreatePipe failed with %#x!\n", s32Ret);
                    return HI_FAILURE;
                }

                if (HI_TRUE == pstViInfo->stPipeInfo.bVcNumCfged)
                {
                    s32Ret = HI_MPI_VI_SetPipeVCNumber(ViPipe, pstViInfo->stPipeInfo.u32VCNum[i]);
                    if (s32Ret != HI_SUCCESS)
                    {
                        HI_MPI_VI_DestroyPipe(ViPipe);
                        SAMPLE_PRT("HI_MPI_VI_SetPipeVCNumber failed with %#x!\n", s32Ret);
                        return HI_FAILURE;
                    }
                }

                s32Ret = HI_MPI_VI_StartPipe(ViPipe);
                if (s32Ret != HI_SUCCESS)
                {
                    HI_MPI_VI_DestroyPipe(ViPipe);
                    SAMPLE_PRT("HI_MPI_VI_StartPipe failed with %#x!\n", s32Ret);
                    return HI_FAILURE;
                }
            }
        }
    }

    return s32Ret;

EXIT:

    for (j = 0; j < i; j++)
    {
        ViPipe = j;
        stop_single_vi_pipe(ViPipe);
    }

    return s32Ret;
}

HI_S32 start_vi_chn(SAMPLE_VI_INFO_S *pstViInfo)
{
    HI_S32 i;
    HI_BOOL bNeedChn;
    HI_S32 s32Ret = HI_SUCCESS;
    VI_PIPE ViPipe;
    VI_CHN ViChn;
    VI_CHN_ATTR_S stChnAttr;
    VI_VPSS_MODE_E enMastPipeMode;

    for (i = 0; i < WDR_MAX_PIPE_NUM; i++)
    {
        if (pstViInfo->stPipeInfo.aPipe[i] >= 0 && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM)
        {
            ViPipe = pstViInfo->stPipeInfo.aPipe[i];
            ViChn = pstViInfo->stChnInfo.ViChn;

            SAMPLE_COMM_VI_GetChnAttrBySns(pstViInfo->stSnsInfo.enSnsType, &stChnAttr);
            stChnAttr.enDynamicRange = pstViInfo->stChnInfo.enDynamicRange;
            stChnAttr.enVideoFormat = pstViInfo->stChnInfo.enVideoFormat;
            stChnAttr.enPixelFormat = pstViInfo->stChnInfo.enPixFormat;
            stChnAttr.enCompressMode = pstViInfo->stChnInfo.enCompressMode;

            if (WDR_MODE_NONE == pstViInfo->stDevInfo.enWDRMode)
            {
                bNeedChn = HI_TRUE;
            }
            else
            {
                bNeedChn = (i > 0) ? HI_FALSE : HI_TRUE;
            }

            if (bNeedChn)
            {
                s32Ret = HI_MPI_VI_SetChnAttr(ViPipe, ViChn, &stChnAttr);

                if (s32Ret != HI_SUCCESS)
                {
                    SAMPLE_PRT("HI_MPI_VI_SetChnAttr failed with %#x!\n", s32Ret);
                    return HI_FAILURE;
                }

                enMastPipeMode = pstViInfo->stPipeInfo.enMastPipeMode;

                if (VI_OFFLINE_VPSS_OFFLINE == enMastPipeMode || VI_ONLINE_VPSS_OFFLINE == enMastPipeMode || VI_PARALLEL_VPSS_OFFLINE == enMastPipeMode)
                {
                    s32Ret = HI_MPI_VI_EnableChn(ViPipe, ViChn);

                    if (s32Ret != HI_SUCCESS)
                    {
                        SAMPLE_PRT("HI_MPI_VI_EnableChn failed with %#x!\n", s32Ret);
                        return HI_FAILURE;
                    }
                }
            }
        }
    }

    return s32Ret;
}

HI_S32 stop_vi_chn(SAMPLE_VI_INFO_S *pstViInfo)
{
    HI_S32 i;
    HI_BOOL bNeedChn;
    HI_S32 s32Ret = HI_SUCCESS;
    VI_PIPE ViPipe;
    VI_CHN ViChn;
    VI_VPSS_MODE_E enMastPipeMode;

    for (i = 0; i < 4; i++)
    {
        if (pstViInfo->stPipeInfo.aPipe[i] >= 0 && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM)
        {
            ViPipe = pstViInfo->stPipeInfo.aPipe[i];
            ViChn = pstViInfo->stChnInfo.ViChn;

            if (WDR_MODE_NONE == pstViInfo->stDevInfo.enWDRMode)
            {
                bNeedChn = HI_TRUE;
            }
            else
            {
                bNeedChn = (i > 0) ? HI_FALSE : HI_TRUE;
            }

            if (bNeedChn)
            {
                enMastPipeMode = pstViInfo->stPipeInfo.enMastPipeMode;

                if (VI_OFFLINE_VPSS_OFFLINE == enMastPipeMode || VI_ONLINE_VPSS_OFFLINE == enMastPipeMode || VI_PARALLEL_VPSS_OFFLINE == enMastPipeMode)
                {
                    s32Ret = HI_MPI_VI_DisableChn(ViPipe, ViChn);

                    if (s32Ret != HI_SUCCESS)
                    {
                        SAMPLE_PRT("HI_MPI_VI_DisableChn failed with %#x!\n", s32Ret);
                        return HI_FAILURE;
                    }
                }
            }
        }
    }

    return s32Ret;
}

HI_S32 stop_single_vi_pipe(VI_PIPE ViPipe)
{
    HI_S32 s32Ret;

    s32Ret = HI_MPI_VI_StopPipe(ViPipe);

    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_VI_StopPipe failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VI_DestroyPipe(ViPipe);

    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_VI_DestroyPipe failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return s32Ret;
}

HI_S32 stop_vi_pipe(SAMPLE_VI_INFO_S *pstViInfo)
{
    HI_S32 i;
    VI_PIPE ViPipe;

    for (i = 0; i < WDR_MAX_PIPE_NUM; i++)
    {
        if (pstViInfo->stPipeInfo.aPipe[i] >= 0 && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM)
        {
            ViPipe = pstViInfo->stPipeInfo.aPipe[i];
            stop_single_vi_pipe(ViPipe);
        }
    }

    return HI_SUCCESS;
}

HI_S32 stop_dev(SAMPLE_VI_INFO_S *pstViInfo)
{
    HI_S32 s32Ret;
    VI_DEV ViDev;

    ViDev = pstViInfo->stDevInfo.ViDev;
    s32Ret = HI_MPI_VI_DisableDev(ViDev);

    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_VI_DisableDev failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 create_single_vi(SAMPLE_VI_INFO_S *pstViInfo)
{
    HI_S32 s32Ret = HI_SUCCESS;

    s32Ret = start_dev(pstViInfo);

    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_StartDev failed !\n");
        return HI_FAILURE;
    }

    // we should bind pipe,then creat pipe
    s32Ret = SAMPLE_COMM_VI_BindPipeDev(pstViInfo);

    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_BindPipeDev failed !\n");
        goto EXIT1;
    }

    s32Ret = SAMPLE_COMM_VI_StartViPipe(pstViInfo);

    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_StartViPipe failed !\n");
        goto EXIT1;
    }

    s32Ret = SAMPLE_COMM_VI_StartViChn(pstViInfo);

    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_StartViChn failed !\n");
        goto EXIT2;
    }

    return HI_SUCCESS;

EXIT2:
    stop_vi_pipe(pstViInfo);

EXIT1:
    stop_dev(pstViInfo);

    return s32Ret;
}

static HI_S32 destory_single_vi(SAMPLE_VI_INFO_S *pstViInfo)
{
    SAMPLE_COMM_VI_StopViChn(pstViInfo);

    SAMPLE_COMM_VI_StopViPipe(pstViInfo);

    SAMPLE_COMM_VI_StopDev(pstViInfo);

    return HI_SUCCESS;
}

HI_S32 create_vi(SAMPLE_VI_CONFIG_S *pstViConfig)   
{
    HI_S32 i, j;
    HI_S32 s32ViNum;
    HI_S32 s32Ret = HI_SUCCESS;
    SAMPLE_VI_INFO_S *pstViInfo = HI_NULL;

    if (!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return HI_FAILURE;
    }

    s32ViNum = pstViConfig->as32WorkingViId[i];
    pstViInfo = &pstViConfig->astViInfo[s32ViNum];

    s32Ret = create_single_vi(pstViInfo);

    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_CreateSingleVi failed !\n");
        goto EXIT;
    }

    return HI_SUCCESS;
EXIT:

    s32ViNum = pstViConfig->as32WorkingViId[j];
    pstViInfo = &pstViConfig->astViInfo[s32ViNum];

    destory_single_vi(pstViInfo);

    return s32Ret;
}