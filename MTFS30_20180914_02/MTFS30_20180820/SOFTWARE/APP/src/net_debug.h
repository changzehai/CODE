/* �ļ���:    net_debug.h                                                   */
/* ��  ��:    ������Դ���ͷ�ļ�                                           */
/* ��  ��:    2018-07-20 changzehai(DTT)                                     */
/* ��  ��:    ��                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#ifndef __NET_DEBUG_H_
#define __NET_DEBUG_H_
#include "cc.h"

#define  NET_SEND_MAX    1024  /* ͨ�����緢����󳤶� */

/*-------------------------------*/
/* ��������                      */
/*-------------------------------*/
extern void debug_network_init(void);
extern void net_debug(u8_t *pdata);

/* ���Ժ꺯�� */
#define MTFS30_DEBUG(fmt, args...) do {char buf[1024];snprintf(buf, 1024, "[MTFS30_DEBUG][%s-%d]:   "fmt, __FUNCTION__, __LINE__, ##args); net_debug((u8_t*)buf);} while(0)
	
#endif