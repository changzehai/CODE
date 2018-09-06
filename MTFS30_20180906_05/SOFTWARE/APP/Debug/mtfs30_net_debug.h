/*****************************************************************************/
/* 文件名:    mtfs30_net_debug.h                                             */
/* 描  述:    网络调试处理头文件                                             */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#ifndef __MTFS30_NET_DEBUG_H_
#define __MTFS30_NET_DEBUG_H_

#include <includes.h>
#include "arch/cc.h"
#include "lwip/api.h"
#include "Config/config.h"
#include "opt.h"


/*-------------------------------*/
/* 宏定义                        */
/*-------------------------------*/
#define  NET_DEBUG_STACK_SIZE         1024    /* 调试任务栈大小       */
#define  NET_DEBUG_TASK_PRIO          1       /* 调试任务优先级       */
#define  NET_DEBUG_MSGQEUE_SIZE       20      /* 调试用消息队列大小   */


/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
extern void net_debug_init( void );
extern void mtfs30_net_debug(u8_t *pnet_debug_data);

#endif