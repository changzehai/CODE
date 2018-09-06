/*****************************************************************************/
/* �ļ���:    ut4b0_receiver.h                                               */
/* ��  ��:    UT4B0���ջ�ͷ�ļ�                                              */
/* ��  ��:    2018-07-19 changzehai(DTT)                                     */
/* ��  ��:    ��                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#ifndef __UT4B0_RECEIVER_H_
#define __UT4B0_RECEIVER_H_
#include "cc.h"
#include "mtfs30.h"


/*-------------------------------*/
/* �궨��                        */
/*-------------------------------*/
#define  CHECKCODE_EQ    0     /* У������ͬ */
#define  CHECKCODE_NOEQ  1     /* У���벻ͬ */


/*-------------------------------*/
/* ��������                      */
/*-------------------------------*/
extern void ut4b0_receiver_init(void);
extern void ut4b0_receiver_msg_parser(u8_t *pdata);
extern void ut4b0_receiver_pps_set(pps_info_t *pps_info);
extern void ut4b0_receiver_acmode_set(u8_t ac_mode);
extern void ut4b0_receiver_msg_set(u8_t ac_mode);
extern void t4b0_receiver_msg_set(u8_t reset_mode);

#endif
