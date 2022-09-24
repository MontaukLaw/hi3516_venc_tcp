#ifndef __SOLID_PROCESS_H_
#define __SOLID_PROCESS_H_

#include "sample_comm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/prctl.h>
#include "data_process.h"

void set_vi_config(SAMPLE_VI_CONFIG_S *pstViConfig);

void get_sensor_info(SAMPLE_VI_CONFIG_S *pstViConfig);

HI_S32 get_size_by_sensor(SAMPLE_SNS_TYPE_E enMode, PIC_SIZE_E *penSize);

HI_S32 get_pic_size(PIC_SIZE_E enPicSize, SIZE_S *pstSize);

void config_vb_buf(VB_CONFIG_S *pstVbConf, SIZE_S stSize);

HI_S32 start_vi(SAMPLE_VI_CONFIG_S *pstViConfig);

void config_vpss(VPSS_GRP_ATTR_S *pstVpssGrpAttr, VPSS_CHN_ATTR_S *astVpssChnAttr, SIZE_S stSize);

HI_S32 start_venc(VENC_CHN VencChn, PAYLOAD_TYPE_E enType, PIC_SIZE_E enSize, SAMPLE_RC_E enRcMode,
                  HI_U32 u32Profile, HI_BOOL bRcnRefShareBuf, VENC_GOP_ATTR_S *pstGopAttr);

HI_VOID *venc_stream_proc(HI_VOID *p);

#endif