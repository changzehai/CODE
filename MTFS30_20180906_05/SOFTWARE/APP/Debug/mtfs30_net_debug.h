/*****************************************************************************/
/* �ļ���:    mtfs30_net_debug.h                                             */
/* ��  ��:    ������Դ���ͷ�ļ�                                             */
/* ��  ��:    2018-07-20 changzehai(DTT)                                     */
/* ��  ��:    ��                                                             */
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
/* �궨��                        */
/*-------------------------------*/
#define  NET_DEBUG_STACK_SIZE         1024    /* ��������ջ��С       */
#define  NET_DEBUG_TASK_PRIO          1       /* �����������ȼ�       */
#define  NET_DEBUG_MSGQEUE_SIZE       20      /* ��������Ϣ���д�С   */


/*-------------------------------*/
/* ��������                      */
/*-------------------------------*/
extern void net_debug_init( void );
extern void mtfs30_net_debug(u8_t *pnet_debug_data);

#endif