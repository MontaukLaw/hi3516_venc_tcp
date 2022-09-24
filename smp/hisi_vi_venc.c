#include "sample_comm.h"
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <netdb.h>
#include <sys/socket.h>

#include <sys/ipc.h>
#include <sys/msg.h>
#include <net/if.h>

#include <linux/if_ether.h>
#include <linux/sockios.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../solid_process.h"

static HI_S32 s32Ret = HI_SUCCESS;
static SAMPLE_VI_CONFIG_S stViConfig;
static PIC_SIZE_E enPicSize;
static SIZE_S stSize;
static VB_CONFIG_S stVbConf;
static VPSS_GRP_ATTR_S stVpssGrpAttr;
static VPSS_CHN_ATTR_S astVpssChnAttr[VPSS_MAX_PHY_CHN_NUM];
static HI_BOOL abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {0};
static VPSS_GRP VpssGrp = 0;
static VI_PIPE ViPipe = 0;
static VI_CHN ViChn = 0;
static SAMPLE_RC_E enRcMode = SAMPLE_RC_CBR;
static VENC_GOP_MODE_E enGopMode;
static VENC_GOP_ATTR_S stGopAttr;
static HI_BOOL bRcnRefShareBuf = HI_FALSE;
static VENC_CHN VencChn[1] = {0};
static PAYLOAD_TYPE_E enPayLoad = PT_H264;
static PIC_SIZE_E enSize[2] = {PIC_1080P, PIC_720P};
static HI_U32 u32Profile = 0;
static VPSS_CHN VpssChn = VPSS_CHN0;
static pthread_t gs_myVencPid;

HI_S32 init_vi(void)
{

    // get sensor info first
    get_sensor_info(&stViConfig);

    // basically set sensor info
    set_vi_config(&stViConfig);

    // get picture size code from sensor type
    s32Ret = get_size_by_sensor(stViConfig.astViInfo[0].stSnsInfo.enSnsType, &enPicSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("get picture size by sensor failed!\n");
        return s32Ret;
    }

    // get exact pic size from code
    s32Ret = get_pic_size(enPicSize, &stSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("get picture size failed!\n");
        return s32Ret;
    }

    // config vb
    config_vb_buf(&stVbConf, stSize);

    // HI_MPI_VB_SetConfig
    // HI_MPI_VB_Init
    // HI_MPI_SYS_Init
    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("system init failed with %d!\n", s32Ret);
        return s32Ret;
    }

    return HI_SUCCESS;
}

void process_vi_venc(void)
{

    SAMPLE_PRT("init vi\n");
    /* init vi */
    s32Ret = init_vi();
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("init_vi failed.s32Ret:0x%x !\n", s32Ret);
        return;
    }

    SAMPLE_PRT("stat vi\n");
    /* start vi */
    s32Ret = start_vi(&stViConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
        return;
    }

    // config vpss
    SAMPLE_PRT("config vpss\n");
    config_vpss(&stVpssGrpAttr, astVpssChnAttr, stSize);

    // start vpss
    // HI_MPI_VPSS_CreateGrp
    // HI_MPI_VPSS_SetChnAttr
    // HI_MPI_VPSS_EnableChn
    // HI_MPI_VPSS_StartGrp
    SAMPLE_PRT("start vpss\n");
    abChnEnable[0] = HI_TRUE;
    s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vpss failed.s32Ret:0x%x !\n", s32Ret);
        return;
    }

    // bind to vpss chn0
    SAMPLE_PRT("bind vi and vpss\n");
    s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("vi bind vpss failed.s32Ret:0x%x !\n", s32Ret);
        return;
    }

    // set rc mode gop mode
    enRcMode = SAMPLE_RC_CBR;
    enGopMode = VENC_GOPMODE_NORMALP;
    SAMPLE_PRT("get gop attr\n");
    s32Ret = SAMPLE_COMM_VENC_GetGopAttr(enGopMode, &stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Get GopAttr for %#x!\n", s32Ret);
        return;
    }

    // vi->vpss
    bRcnRefShareBuf = HI_TRUE;

    // encode to h.264
    SAMPLE_PRT("start venc\n");
    s32Ret = start_venc(0, enPayLoad, enSize[0], enRcMode, u32Profile, bRcnRefShareBuf, &stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start_venc faild with%#x! \n", s32Ret);
        return;
    }

    //  from vpss chn0 bind to VencChn[0]
    s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(VpssGrp, VpssChn, 0);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Get GopAttr failed for %#x!\n", s32Ret);
        return;
    }

    SAMPLE_PRT("stream save process\n");
    SAMPLE_VENC_GETSTREAM_PARA_S s_stMyPara;

    memset(&s_stMyPara, 0, sizeof(s_stMyPara));

    s_stMyPara.bThreadStart = HI_TRUE;
    s_stMyPara.s32Cnt = 1;
    s_stMyPara.VeChn[0] = 0;

    pthread_create(&gs_myVencPid, 0, venc_stream_proc, (HI_VOID *)&s_stMyPara);
    
}