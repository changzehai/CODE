/* 文件名:    net_debug.h                                                   */
/* 描  述:    网络调试处理头文件                                           */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#ifndef __NET_DEBUG_H_
#define __NET_DEBUG_H_
#include "cc.h"

#define  NET_SEND_MAX    1024  /* 通过网络发送最大长度 */

/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
extern void debug_network_init(void);
extern void net_debug(u8_t *pdata);

/* 调试宏函数 */
#define MTFS30_DEBUG(fmt, args...) do {char buf[1024];snprintf(buf, 1024, "[MTFS30_DEBUG][%s-%d]:   "fmt, __FUNCTION__, __LINE__, ##args); net_debug((u8_t*)buf);} while(0)
	
#endif