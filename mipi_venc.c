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

#include "sample_comm.h"
#include "data_sender.h"

#include "hisi_vi_venc.h"

#define TCP_PORT 54322

static HI_S32 s32Ret = HI_SUCCESS;
// static HI_S32 s32ViCnt = 1;
// static VI_DEV ViDev = 0;
// static VO_CHN VoChn = 0;
// static PAYLOAD_TYPE_E enType = PT_H265;
// static PIC_SIZE_E enSize[2] = {PIC_360P, PIC_720P};

extern int udpfd;
extern int tcpfd;
struct sockaddr_in g_server;

void SAMPLE_VIO_HandleSig(HI_S32 signo)
{
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);

    if (SIGINT == signo || SIGTERM == signo)
    {
        SAMPLE_COMM_VENC_StopGetStream();
        SAMPLE_COMM_All_ISP_Stop();
        SAMPLE_COMM_VO_HdmiStop();
        SAMPLE_COMM_SYS_Exit();
        SAMPLE_PRT("\033[0;31mprogram termination abnormally!\033[0;39m\n");
    }
    exit(-1);
}

void init_tcp_connection(char *ipAddr)
{

    tcpfd = socket(AF_INET, SOCK_STREAM, 0); // TCP

    if (tcpfd < 0)
    {
        SAMPLE_PRT("socket error\n");
    }
    g_server.sin_family = AF_INET;
    g_server.sin_port = htons(TCP_PORT);
    // int rtn = inet_pton(AF_INET, "192.168.31.211", (struct in_addr *)&g_server.sin_addr.s_addr);
    int rtn = inet_pton(AF_INET, ipAddr, (struct in_addr *)&g_server.sin_addr.s_addr);
    if (rtn < 0)
    {
        SAMPLE_PRT("ip error\n");
    }
    // SAMPLE_PRT("udp up\n");
    rtn = connect(tcpfd, (struct sockaddr *)&g_server, sizeof(g_server));

    if (rtn < 0)
    {
        SAMPLE_PRT("connet error\n");
    }
    // sendto(udpfd, "abcde", 5, 0, (struct sockaddr *)&g_server, sizeof(g_server));
}

void init_udp_sender(char *ipAddr)
{

    udpfd = socket(AF_INET, SOCK_DGRAM, 0); // UDP

    g_server.sin_family = AF_INET;
    g_server.sin_port = htons(54321);
    // int rtn = inet_pton(AF_INET, "192.168.31.211", (struct in_addr *)&g_server.sin_addr.s_addr);
    int rtn = inet_pton(AF_INET, ipAddr, (struct in_addr *)&g_server.sin_addr.s_addr);
    if (rtn < 0)
    {
        SAMPLE_PRT("ip error\n");
    }
    SAMPLE_PRT("udp up\n");
    sendto(udpfd, "abcde", 5, 0, (struct sockaddr *)&g_server, sizeof(g_server));
}

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        SAMPLE_PRT("input streaming address please \n");
        return s32Ret;
    }

    SAMPLE_PRT("Streaming to %s:%d\n", argv[1], TCP_PORT);

    init_tcp_connection(argv[1]);
    // init_udp_sender(argv[1]);

    init_data_sender_thread();

    process_vi_venc();

#if 0

    /* config vpss */
    config_vpss(&stVpssGrpAttr, astVpssChnAttr, stSize);

    /*start vpss*/
    abChnEnable[0] = HI_TRUE;
    s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);

    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
        goto EXIT1;
    }

    /*vi bind vpss*/
    s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);

    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("vi bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
        goto EXIT2;
    }

    enRcMode = SAMPLE_RC_CBR;
    enGopMode = VENC_GOPMODE_NORMALP;
    s32Ret = SAMPLE_COMM_VENC_GetGopAttr(enGopMode, &stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Get GopAttr for %#x!\n", s32Ret);
        goto EXIT_VI_VPSS_UNBIND;
    }

    // vi->vpss
    bRcnRefShareBuf = HI_TRUE;

    /***encode h.265 **/
    s32Ret = SAMPLE_COMM_VENC_Start(VencChn[0], enPayLoad, enSize[0], enRcMode,
                                    u32Profile, bRcnRefShareBuf, &stGopAttr);

    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
        goto EXIT_VI_VPSS_UNBIND;
    }

    s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(VpssGrp, VpssChn, VencChn[0]);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Get GopAttr failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H265_STOP;
    }

    /******************************************
    stream save process
    ******************************************/

    SAMPLE_PRT("stream save process\n");
    s32Ret = SAMPLE_COMM_VENC_StartGetStream(VencChn, 1);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Venc failed!\n");
        goto EXIT_VENC_H264_UnBind;
    }
#endif

    PAUSE();

    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}

#if 0
int main_ouput_vo(int argc, char *argv[])
{

    /* config vi */
    init_vi();

    /* start vi */
    s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
        goto EXIT;
    }

    /* config vpss */
    config_vpss(&stVpssGrpAttr, astVpssChnAttr, stSize);

    /*start vpss*/
    abChnEnable[0] = HI_TRUE;
    s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);

    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
        goto EXIT1;
    }

    /*vi bind vpss*/
    s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);

    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("vi bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
        goto EXIT2;
    }

    /*config vo*/
    SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);

    // output direction
    stVoConfig.enVoIntfType = VO_INTF_HDMI;
    stVoConfig.enPicSize = enPicSize;

    /*start vo*/
    s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vo failed. s32Ret: 0x%x !\n", s32Ret);
        goto EXIT4;
    }

    /*vpss bind vo*/
    s32Ret = SAMPLE_COMM_VPSS_Bind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("vo bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
        goto EXIT5;
    }

    PAUSE();

EXIT6:
    SAMPLE_COMM_VPSS_UnBind_VO(VpssGrp, VpssChn, stVoConfig.VoDev, VoChn);
EXIT5:
    SAMPLE_COMM_VO_StopVO(&stVoConfig);
EXIT4:
    SAMPLE_COMM_VPSS_UnBind_VENC(VpssGrp, VpssChn, VencChn[0]);
EXIT3:
    SAMPLE_COMM_VENC_Stop(VencChn[0]);
EXIT2:
    SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);
EXIT1:
    SAMPLE_COMM_VI_StopVi(&stViConfig);
EXIT:
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}

#endif