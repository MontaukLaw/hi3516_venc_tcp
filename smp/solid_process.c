#include "../solid_process.h"

#define MIPI_2_DEV_ID 1

void set_vi_config(SAMPLE_VI_CONFIG_S *pstViConfig)
{
    pstViConfig->s32WorkingViNum = 1;
    pstViConfig->as32WorkingViId[0] = 0;
    pstViConfig->astViInfo[0].stSnsInfo.MipiDev = MIPI_2_DEV_ID;
    pstViConfig->astViInfo[0].stSnsInfo.s32BusId = MIPI_2_DEV_ID;
    pstViConfig->astViInfo[0].stDevInfo.ViDev = MIPI_2_DEV_ID;
    pstViConfig->astViInfo[0].stDevInfo.enWDRMode = WDR_MODE_NONE;
    pstViConfig->astViInfo[0].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
    pstViConfig->astViInfo[0].stPipeInfo.aPipe[0] = 0;
    pstViConfig->astViInfo[0].stPipeInfo.aPipe[1] = -1;
    pstViConfig->astViInfo[0].stPipeInfo.aPipe[2] = -1;
    pstViConfig->astViInfo[0].stPipeInfo.aPipe[3] = -1;
    pstViConfig->astViInfo[0].stChnInfo.ViChn = 0;
    pstViConfig->astViInfo[0].stChnInfo.enPixFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    pstViConfig->astViInfo[0].stChnInfo.enDynamicRange = DYNAMIC_RANGE_SDR8;
    pstViConfig->astViInfo[0].stChnInfo.enVideoFormat = VIDEO_FORMAT_LINEAR;
    pstViConfig->astViInfo[0].stChnInfo.enCompressMode = COMPRESS_MODE_NONE;
}

void get_sensor_info(SAMPLE_VI_CONFIG_S *pstViConfig)
{
    HI_S32 i;

    for (i = 0; i < VI_MAX_DEV_NUM; i++)
    {
        pstViConfig->astViInfo[i].stSnsInfo.s32SnsId = i;
        pstViConfig->astViInfo[i].stSnsInfo.s32BusId = i;
        pstViConfig->astViInfo[i].stSnsInfo.MipiDev = i;
        memset_s(&pstViConfig->astViInfo[i].stSnapInfo, sizeof(SAMPLE_SNAP_INFO_S), 0, sizeof(SAMPLE_SNAP_INFO_S));
        pstViConfig->astViInfo[i].stPipeInfo.bMultiPipe = HI_FALSE;
        pstViConfig->astViInfo[i].stPipeInfo.bVcNumCfged = HI_FALSE;
    }

    pstViConfig->astViInfo[0].stSnsInfo.enSnsType = GALAXYCORE_GC2053_MIPI_2M_30FPS_10BIT;
    pstViConfig->astViInfo[1].stSnsInfo.enSnsType = GALAXYCORE_GC2053_MIPI_2M_30FPS_10BIT;
}

HI_S32 get_size_by_sensor(SAMPLE_SNS_TYPE_E enMode, PIC_SIZE_E *penSize)
{
    HI_S32 s32Ret = HI_SUCCESS;

    if (!penSize)
    {
        return HI_FAILURE;
    }

    switch (enMode)
    {
    case GALAXYCORE_GC2053_MIPI_2M_30FPS_10BIT:
        *penSize = PIC_1080P;
        break;
    case SONY_IMX327_MIPI_2M_30FPS_12BIT:
    case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
        *penSize = PIC_1080P;
        break;

    case SONY_IMX307_MIPI_2M_30FPS_12BIT:
    case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
        *penSize = PIC_1080P;
        break;

    case PANASONIC_MN34220_LVDS_2M_30FPS_12BIT:
        *penSize = PIC_1080P;
        break;

    case SONY_IMX335_MIPI_5M_30FPS_12BIT:
    case SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1:
        *penSize = PIC_2592x1944;
        break;

    case SONY_IMX335_MIPI_4M_30FPS_12BIT:
    case SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1:
        *penSize = PIC_2592x1536;
        break;

    case SMART_SC4210_MIPI_3M_30FPS_12BIT:
    case SMART_SC4210_MIPI_3M_30FPS_10BIT_WDR2TO1:
        *penSize = PIC_2560x1440;
        break;

    case OMNIVISION_OS08A10_MIPI_8M_30FPS_10BIT:
    case SONY_IMX458_MIPI_8M_30FPS_10BIT:
    case OMNIVISION_OV12870_MIPI_8M_30FPS_10BIT:
    case SONY_IMX415_MIPI_8M_30FPS_12BIT:
    case SONY_IMX415_MIPI_8M_20FPS_12BIT:
        *penSize = PIC_3840x2160;
        break;
    case SONY_IMX458_MIPI_12M_20FPS_10BIT:
    case OMNIVISION_OV12870_MIPI_12M_30FPS_10BIT:
        *penSize = PIC_4000x3000;
        break;
    case SONY_IMX458_MIPI_4M_60FPS_10BIT:
    case SONY_IMX458_MIPI_4M_40FPS_10BIT:
        *penSize = PIC_2716x1524;
        break;
    case SONY_IMX458_MIPI_2M_90FPS_10BIT:
    case OMNIVISION_OV12870_MIPI_2M_120FPS_10BIT:
        *penSize = PIC_1080P;
        break;
    case SONY_IMX458_MIPI_1M_129FPS_10BIT:
    case OMNIVISION_OV12870_MIPI_1M_240FPS_10BIT:
        *penSize = PIC_720P;
        break;

    case OMNIVISION_OS04B10_MIPI_4M_30FPS_10BIT:
        *penSize = PIC_2560x1440;
        break;

    case OMNIVISION_OS05A_MIPI_4M_30FPS_12BIT:
    case OMNIVISION_OS05A_MIPI_4M_30FPS_10BIT_WDR2TO1:
        *penSize = PIC_2688x1536;
        break;

    default:
        *penSize = PIC_1080P;
        break;
    }

    return s32Ret;
}

void config_vb_buf(VB_CONFIG_S *pstVbConf, SIZE_S stSize)
{
    HI_U32 u32BlkSize;

    memset_s(pstVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
    pstVbConf->u32MaxPoolCnt = 2;

    u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, SAMPLE_PIXEL_FORMAT, DATA_BITWIDTH_8,
                                         COMPRESS_MODE_SEG, DEFAULT_ALIGN);
    pstVbConf->astCommPool[0].u64BlkSize = u32BlkSize;
    pstVbConf->astCommPool[0].u32BlkCnt = 10;

    u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_SEMIPLANAR_420 /*PIXEL_FORMAT_RGB_BAYER_16BPP*/,
                                     COMPRESS_MODE_NONE, DEFAULT_ALIGN);
    pstVbConf->astCommPool[1].u64BlkSize = u32BlkSize;
    pstVbConf->astCommPool[1].u32BlkCnt = 4;
}

HI_S32 get_pic_size(PIC_SIZE_E enPicSize, SIZE_S *pstSize)
{
    switch (enPicSize)
    {
    case PIC_CIF: /* 352 * 288 */
        pstSize->u32Width = 352;
        pstSize->u32Height = 288;
        break;

    case PIC_360P: /* 640 * 360 */
        pstSize->u32Width = 640;
        pstSize->u32Height = 360;
        break;

    case PIC_D1_PAL: /* 720 * 576 */
        pstSize->u32Width = 720;
        pstSize->u32Height = 576;
        break;

    case PIC_D1_NTSC: /* 720 * 480 */
        pstSize->u32Width = 720;
        pstSize->u32Height = 480;
        break;

    case PIC_720P: /* 1280 * 720 */
        pstSize->u32Width = 1280;
        pstSize->u32Height = 720;
        break;

    case PIC_1080P: /* 1920 * 1080 */
        pstSize->u32Width = 1920;
        pstSize->u32Height = 1080;
        break;

    case PIC_2592x1520:
        pstSize->u32Width = 2592;
        pstSize->u32Height = 1520;
        break;

    case PIC_2592x1944:
        pstSize->u32Width = 2592;
        pstSize->u32Height = 1944;
        break;

    case PIC_2592x1536:
        pstSize->u32Width = 2592;
        pstSize->u32Height = 1536;
        break;

    case PIC_2560x1440:
        pstSize->u32Width = 2560;
        pstSize->u32Height = 1440;
        break;

    case PIC_2716x1524:
        pstSize->u32Width = 2716;
        pstSize->u32Height = 1524;
        break;

    case PIC_3840x2160:
        pstSize->u32Width = 3840;
        pstSize->u32Height = 2160;
        break;

    case PIC_3000x3000:
        pstSize->u32Width = 3000;
        pstSize->u32Height = 3000;
        break;

    case PIC_4000x3000:
        pstSize->u32Width = 4000;
        pstSize->u32Height = 3000;
        break;

    case PIC_4096x2160:
        pstSize->u32Width = 4096;
        pstSize->u32Height = 2160;
        break;

    case PIC_7680x4320:
        pstSize->u32Width = 7680;
        pstSize->u32Height = 4320;
        break;
    case PIC_3840x8640:
        pstSize->u32Width = 3840;
        pstSize->u32Height = 8640;
        break;

    case PIC_2688x1536:
        pstSize->u32Width = 2688;
        pstSize->u32Height = 1536;
        break;

    default:
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 start_vi(SAMPLE_VI_CONFIG_S *pstViConfig)
{
    HI_S32 s32Ret = HI_SUCCESS;

    if (!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return HI_FAILURE;
    }

    SAMPLE_PRT("SAMPLE_COMM_VI_StartMIPI\n");
    // remember set  lane_divide_mode = LANE_DIVIDE_MODE_1 inside;
    s32Ret = SAMPLE_COMM_VI_StartMIPI(pstViConfig);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_StartMIPI failed!\n");
        return HI_FAILURE;
    }

    SAMPLE_PRT("SAMPLE_COMM_VI_SetParam\n");
    s32Ret = SAMPLE_COMM_VI_SetParam(pstViConfig);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_SetParam failed!\n");
        return HI_FAILURE;
    }

    s32Ret = SAMPLE_COMM_VI_CreateVi(pstViConfig);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_CreateVi failed!\n");
        return HI_FAILURE;
    }

    s32Ret = SAMPLE_COMM_VI_CreateIsp(pstViConfig);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_COMM_VI_DestroyVi(pstViConfig);
        SAMPLE_PRT("SAMPLE_COMM_VI_CreateIsp failed!\n");
        return HI_FAILURE;
    }

    return s32Ret;
}

// important part
void config_vpss(VPSS_GRP_ATTR_S *pstVpssGrpAttr, VPSS_CHN_ATTR_S *astVpssChnAttr, SIZE_S stSize)
{
    memset_s(pstVpssGrpAttr, sizeof(VPSS_GRP_ATTR_S), 0, sizeof(VPSS_GRP_ATTR_S));
    pstVpssGrpAttr->stFrameRate.s32SrcFrameRate = -1;
    pstVpssGrpAttr->stFrameRate.s32DstFrameRate = -1;
    pstVpssGrpAttr->enDynamicRange = DYNAMIC_RANGE_SDR8;
    pstVpssGrpAttr->enPixelFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    pstVpssGrpAttr->u32MaxW = stSize.u32Width;
    pstVpssGrpAttr->u32MaxH = stSize.u32Height;
    pstVpssGrpAttr->bNrEn = HI_TRUE;
    pstVpssGrpAttr->stNrAttr.enCompressMode = COMPRESS_MODE_FRAME;
    pstVpssGrpAttr->stNrAttr.enNrMotionMode = NR_MOTION_MODE_NORMAL;

    astVpssChnAttr[0].u32Width = stSize.u32Width;
    astVpssChnAttr[0].u32Height = stSize.u32Height;
    astVpssChnAttr[0].enChnMode = VPSS_CHN_MODE_USER;
    astVpssChnAttr[0].enCompressMode = COMPRESS_MODE_NONE;
    astVpssChnAttr[0].enDynamicRange = DYNAMIC_RANGE_SDR8;
    astVpssChnAttr[0].enVideoFormat = VIDEO_FORMAT_LINEAR;
    astVpssChnAttr[0].enPixelFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    astVpssChnAttr[0].stFrameRate.s32SrcFrameRate = 30;
    astVpssChnAttr[0].stFrameRate.s32DstFrameRate = 30;
    astVpssChnAttr[0].u32Depth = 0;
    astVpssChnAttr[0].bMirror = HI_FALSE;
    astVpssChnAttr[0].bFlip = HI_FALSE;
    astVpssChnAttr[0].stAspectRatio.enMode = ASPECT_RATIO_NONE;
}

HI_S32 create_venc_chn(VENC_CHN VencChn, PAYLOAD_TYPE_E enType, PIC_SIZE_E enSize, SAMPLE_RC_E enRcMode,
                       HI_U32 u32Profile, HI_BOOL bRcnRefShareBuf, VENC_GOP_ATTR_S *pstGopAttr)
{
    HI_S32 s32Ret;
    SIZE_S stPicSize;
    VENC_CHN_ATTR_S stVencChnAttr;
    SAMPLE_VI_CONFIG_S stViConfig;
    HI_U32 u32FrameRate;
    HI_U32 u32StatTime;
    HI_U32 u32Gop = 30;

    get_pic_size(enSize, &stPicSize);

    get_sensor_info(&stViConfig);

    // what the fuck
    u32FrameRate = 30;

    /******************************************
     step 1:  Create Venc Channel
    ******************************************/
    stVencChnAttr.stVencAttr.enType = enType;
    stVencChnAttr.stVencAttr.u32MaxPicWidth = stPicSize.u32Width;
    stVencChnAttr.stVencAttr.u32MaxPicHeight = stPicSize.u32Height;
    stVencChnAttr.stVencAttr.u32PicWidth = stPicSize.u32Width;                          /*the picture width*/
    stVencChnAttr.stVencAttr.u32PicHeight = stPicSize.u32Height;                        /*the picture height*/
    stVencChnAttr.stVencAttr.u32BufSize = stPicSize.u32Width * stPicSize.u32Height * 2; /*stream buffer size*/
    stVencChnAttr.stVencAttr.u32Profile = u32Profile;
    stVencChnAttr.stVencAttr.bByFrame = HI_TRUE; /*get stream mode is slice mode or frame mode?*/

    u32StatTime = 1;

    VENC_H264_CBR_S stH264Cbr;

    stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
    stH264Cbr.u32Gop = u32Gop;                 /*the interval of IFrame*/
    stH264Cbr.u32StatTime = u32StatTime;       /* stream rate statics time(s) */
    stH264Cbr.u32SrcFrameRate = u32FrameRate;  /* input (vi) frame rate */
    stH264Cbr.fr32DstFrameRate = u32FrameRate; /* target frame rate */
    switch (enSize)
    {
    case PIC_720P:
        stH264Cbr.u32BitRate = 1024 * 3 + 1024 * u32FrameRate / 30;
        break;
    case PIC_1080P:
        stH264Cbr.u32BitRate = 1024 * 2 + 2048 * u32FrameRate / 30;
        break;
    case PIC_2592x1944:
        stH264Cbr.u32BitRate = 1024 * 4 + 3072 * u32FrameRate / 30;
        break;
    case PIC_3840x2160:
        stH264Cbr.u32BitRate = 1024 * 8 + 5120 * u32FrameRate / 30;
        break;
    case PIC_4000x3000:
        stH264Cbr.u32BitRate = 1024 * 12 + 5120 * u32FrameRate / 30;
        break;
    case PIC_7680x4320:
        stH264Cbr.u32BitRate = 1024 * 24 + 5120 * u32FrameRate / 30;
        break;
    default:
        stH264Cbr.u32BitRate = 1024 * 24 + 5120 * u32FrameRate / 30;
        break;
    }

    memcpy(&stVencChnAttr.stRcAttr.stH264Cbr, &stH264Cbr, sizeof(VENC_H264_CBR_S));

    stVencChnAttr.stVencAttr.stAttrH264e.bRcnRefShareBuf = bRcnRefShareBuf;

    memcpy(&stVencChnAttr.stGopAttr, pstGopAttr, sizeof(VENC_GOP_ATTR_S));

    s32Ret = HI_MPI_VENC_CreateChn(VencChn, &stVencChnAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VENC_CreateChn [%d] faild with %#x! ===\n",
                   VencChn, s32Ret);
        return s32Ret;
    }

    s32Ret = SAMPLE_COMM_VENC_CloseReEncode(VencChn);
    if (HI_SUCCESS != s32Ret)
    {
        HI_MPI_VENC_DestroyChn(VencChn);
        return s32Ret;
    }

    return HI_SUCCESS;
}

HI_S32 start_venc(VENC_CHN VencChn, PAYLOAD_TYPE_E enType, PIC_SIZE_E enSize, SAMPLE_RC_E enRcMode, HI_U32 u32Profile, HI_BOOL bRcnRefShareBuf, VENC_GOP_ATTR_S *pstGopAttr)
{
    HI_S32 s32Ret;
    VENC_RECV_PIC_PARAM_S stRecvParam;

    /******************************************
     step 1:  Creat Encode Chnl
    ******************************************/
    s32Ret = create_venc_chn(VencChn, enType, enSize, enRcMode, u32Profile, bRcnRefShareBuf, pstGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VENC_Creat faild with%#x! \n", s32Ret);
        return HI_FAILURE;
    }

    /******************************************
     step 2:  Start Recv Venc Pictures
    ******************************************/
    stRecvParam.s32RecvPicNum = -1;
    s32Ret = HI_MPI_VENC_StartRecvFrame(VencChn, &stRecvParam);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VENC_StartRecvPic faild with%#x! \n", s32Ret);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

HI_VOID *venc_stream_proc(HI_VOID *p)
{
    HI_S32 s32ChnTotal;
    VENC_CHN_ATTR_S stVencChnAttr;
    // SAMPLE_VENC_GETSTREAM_PARA_S *pstPara;
    HI_S32 maxfd = 0;
    struct timeval TimeoutVal;
    fd_set read_fds;
    HI_S32 VencFd[VENC_MAX_CHN_NUM];
    HI_CHAR aszFileName[VENC_MAX_CHN_NUM][64];
    FILE *pFile[VENC_MAX_CHN_NUM];
    char szFilePostfix[10];
    VENC_CHN_STATUS_S stStat;
    VENC_STREAM_S stStream;
    HI_S32 s32Ret;
    VENC_CHN VencChn;
    PAYLOAD_TYPE_E enPayLoadType[VENC_MAX_CHN_NUM];
    VENC_STREAM_BUF_INFO_S stStreamBufInfo[VENC_MAX_CHN_NUM];

    prctl(PR_SET_NAME, "GetVencStream", 0, 0, 0);

    // pstPara = (SAMPLE_VENC_GETSTREAM_PARA_S *)p;
    // s32ChnTotal = pstPara->s32Cnt;

    s32ChnTotal = 1;

    SAMPLE_PRT("s32ChnTotal:%d \n", s32ChnTotal);

    /******************************************
     step 1:  check & prepare save-file & venc-fd
    ******************************************/
    if (s32ChnTotal >= VENC_MAX_CHN_NUM)
    {
        SAMPLE_PRT("input count invaild\n");
        return NULL;
    }

    /* decide the stream file name, and open file to save stream */
    // VencChn = pstPara->VeChn[0];
    VencChn = 0;
    s32Ret = HI_MPI_VENC_GetChnAttr(VencChn, &stVencChnAttr);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_VENC_GetChnAttr chn[%d] failed with %#x!\n",
                   VencChn, s32Ret);
        return NULL;
    }
    enPayLoadType[0] = stVencChnAttr.stVencAttr.enType;

    /* Set Venc Fd. */
    VencFd[0] = HI_MPI_VENC_GetFd(0);
    if (VencFd[0] < 0)
    {
        SAMPLE_PRT("HI_MPI_VENC_GetFd failed with %#x!\n",
                   VencFd[0]);
        return NULL;
    }
    if (maxfd <= VencFd[0])
    {
        maxfd = VencFd[0];
    }

    s32Ret = HI_MPI_VENC_GetStreamBufInfo(0, &stStreamBufInfo[0]);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VENC_GetStreamBufInfo failed with %#x!\n", s32Ret);
        return (void *)HI_FAILURE;
    }

    /******************************************
     step 2:  Start to get streams of each channel.
    ******************************************/
    while (HI_TRUE)
    {
        FD_ZERO(&read_fds);

        FD_SET(VencFd[0], &read_fds);

        TimeoutVal.tv_sec = 2;
        TimeoutVal.tv_usec = 0;
        s32Ret = select(maxfd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if (s32Ret < 0)
        {
            SAMPLE_PRT("select failed!\n");
            break;
        }
        else if (s32Ret == 0)
        {
            SAMPLE_PRT("get venc stream time out, exit thread\n");
            continue;
        }
        else
        {
            if (FD_ISSET(VencFd[0], &read_fds))
            {
                /*******************************************************
                 step 2.1 : query how many packs in one-frame stream.
                *******************************************************/
                memset(&stStream, 0, sizeof(stStream));

                s32Ret = HI_MPI_VENC_QueryStatus(0, &stStat);
                if (HI_SUCCESS != s32Ret)
                {
                    SAMPLE_PRT("HI_MPI_VENC_QueryStatus chn[%d] failed with %#x!\n", 0, s32Ret);
                    break;
                }

                /*******************************************************
                step 2.2 :suggest to check both u32CurPacks and u32LeftStreamFrames at the same time,for example:
                 if(0 == stStat.u32CurPacks || 0 == stStat.u32LeftStreamFrames)
                 {
                    SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
                    continue;
                 }
                *******************************************************/
                if (0 == stStat.u32CurPacks)
                {
                    SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
                    continue;
                }
                /*******************************************************
                 step 2.3 : malloc corresponding number of pack nodes.
                *******************************************************/
                stStream.pstPack = (VENC_PACK_S *)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
                if (NULL == stStream.pstPack)
                {
                    SAMPLE_PRT("malloc stream pack failed!\n");
                    break;
                }

                /*******************************************************
                 step 2.4 : call mpi to get one-frame strei am
                *******************************************************/
                stStream.u32PackCount = stStat.u32CurPacks;
                s32Ret = HI_MPI_VENC_GetStream(0, &stStream, HI_TRUE);
                if (HI_SUCCESS != s32Ret)
                {
                    free(stStream.pstPack);
                    stStream.pstPack = NULL;
                    SAMPLE_PRT("HI_MPI_VENC_GetStream failed with %#x!\n", s32Ret);
                    break;
                }

                /*******************************************************
                 step 2.5 : add data to list
                *******************************************************/
                add_stream_pack_to_list(&stStream);

                /*******************************************************
                 step 2.6 : release stream
                 *******************************************************/
                s32Ret = HI_MPI_VENC_ReleaseStream(0, &stStream);
                if (HI_SUCCESS != s32Ret)
                {
                    SAMPLE_PRT("HI_MPI_VENC_ReleaseStream failed!\n");
                    free(stStream.pstPack);
                    stStream.pstPack = NULL;
                    break;
                }

                /*******************************************************
                 step 2.7 : free pack nodes
                *******************************************************/
                free(stStream.pstPack);
                stStream.pstPack = NULL;
            }
        }
    }
    /*******************************************************
     * step 3 : close save-file
     *******************************************************/
    return NULL;
}