/*****************************************************************************/
/* 文件名:    ut4b0_receiver.h                                               */
/* 描  述:    UT4B0接收机头文件                                              */
/* 创  建:    2018-07-19 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#ifndef __UT4B0_RECEIVER_H_
#define __UT4B0_RECEIVER_H_
#include "cc.h"
#include "mtfs30.h"


/*-------------------------------*/
/* 宏定义                        */
/*-------------------------------*/
#define  CHECKCODE_EQ    0     /* 校验码相同 */
#define  CHECKCODE_NOEQ  1     /* 校验码不同 */


/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
extern void ut4b0_receiver_init(void);
extern void ut4b0_receiver_msg_parser(u8_t *pdata);
extern void ut4b0_receiver_pps_set(pps_info_t *pps_info);
extern void ut4b0_receiver_acmode_set(u8_t ac_mode);
extern void ut4b0_receiver_msg_set(u8_t ac_mode);
extern void t4b0_receiver_msg_set(u8_t reset_mode);

#endif
