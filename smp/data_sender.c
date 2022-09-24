#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <netdb.h>
#include <sys/socket.h>

#include <sys/ioctl.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <netinet/if_ether.h>
#include <net/if.h>

#include <linux/if_ether.h>
#include <linux/sockios.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "sample_comm.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <errno.h>
#include <math.h>
#include <signal.h>

#define SEND_TCP write(udpfd, rawUdpSendBuf, bytes);
#define SEND_UDP sendto(udpfd, rawUdpSendBuf, bytes, 0, (struct sockaddr *)&g_server, sizeof(g_server));
#define RAW_UDP_PACK_LEN 1400
// #define nalu_sent_len 1400

static pthread_t gs_RtpPid;
int g_count = 0;
int udpfd;
int tcpfd;
struct list_head RTPbuf_head = LIST_HEAD_INIT(RTPbuf_head);

uint8_t rawUdpSendBuf[RAW_UDP_PACK_LEN];

extern struct sockaddr_in g_server;

void send_udp(char *buf, int bufSize)
{
    sendto(udpfd, buf, bufSize, 0, (struct sockaddr *)&g_server, sizeof(g_server));
}

HI_S32 send_h264_raw_udp_package(char *buffer, int buflen)
{
    static int frameCounter = 0;
    // sendto(udpfd, buffer, buflen, 0, (struct sockaddr *)&g_server, sizeof(g_server));
    int udpPacketLen = buflen;
    // printf("buflen = %d\n", buflen);
    if (buflen == 20)
    {
        SAMPLE_PRT("frame number in 1 sec: %d\n", frameCounter);
        frameCounter = 0;
    }
    frameCounter++;

    int bytes = 0;

    if (udpPacketLen < RAW_UDP_PACK_LEN)
    {
        write(tcpfd, buffer, buflen);
        // sendto(udpfd, buffer, buflen, 0, (struct sockaddr *)&g_server, sizeof(g_server));
    }
    else if (udpPacketLen > RAW_UDP_PACK_LEN)
    {
        int pktTotalNumber = 0, tailBytes = 0;
        pktTotalNumber = udpPacketLen / RAW_UDP_PACK_LEN;
        tailBytes = udpPacketLen % RAW_UDP_PACK_LEN;
        int pktSent = 0;
        while (pktSent <= pktTotalNumber)
        {
            if (pktSent == 0) // first packet
            {
                memcpy(rawUdpSendBuf, buffer, RAW_UDP_PACK_LEN);
                bytes = RAW_UDP_PACK_LEN;
                SEND_TCP
                // SEND_UDP
                pktSent++;
            }
            else if (pktSent == pktTotalNumber) // last packet
            {
                memcpy(rawUdpSendBuf, buffer + pktSent * RAW_UDP_PACK_LEN, tailBytes);
                bytes = tailBytes;
                SEND_TCP
                // SEND_UDP
                pktSent++;
            }
            else if (pktSent < pktTotalNumber && pktSent != 0) // all packet
            {
                memcpy(rawUdpSendBuf, buffer + pktSent * RAW_UDP_PACK_LEN, RAW_UDP_PACK_LEN);
                bytes = RAW_UDP_PACK_LEN;
                SEND_TCP
                // SEND_UDP
                pktSent++;
            }
        }
    }
}

HI_VOID *rtp_send_thread(HI_VOID *p)
{
    static uint8_t counter = 0;
    while (1)
    {
        if (!list_empty(&RTPbuf_head))
        {
            counter++;

            // 取出头item进行发送
            RTPbuf_s *p = get_first_item(&RTPbuf_head, RTPbuf_s, list);

            // send_h264_raw_udp_package(p->buf, p->len);

            SAMPLE_PRT("writing :%d\n", p->len);
            write(tcpfd, p->buf, p->len);

            list_del(&(p->list));
            free(p->buf);
            free(p);
            p = NULL;
            g_count--;
            // printf("g_count = %d\n",g_count);

            if (counter > 30)
            {
                // exit(0);
            }
        }
        usleep(5000);
    }
}

void init_data_sender_thread(void)
{
    pthread_create(&gs_RtpPid, 0, rtp_send_thread, NULL);
}