/*****************************************************************************/
/* 文件名:    gnss_handler.h                                                 */
/* 描  述:    GNSS相关设置/查询处理头文件                                    */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#ifndef __GNSS_HANDLER_H_
#define __GNSS_HANDLER_H_
#include "cc.h"





/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
extern void gnss_acmode_set_handler(u8_t *pparam);
extern void gnss_receiver_reset_handler(u8_t *pparam);
#endif
