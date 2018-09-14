/*****************************************************************************/
/* �ļ���:    gnss_receiver.h                                                */
/* ��  ��:    GNSS��������ش���ͷ�ļ�                                       */
/* ��  ��:    2018-07-20 changzehai(DTT)                                     */
/* ��  ��:    ��                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#ifndef __GNSS_RECEIVER_H_
#define __GNSS_RECEIVER_H_
#include "cc.h"


/*-------------------------------*/
/* �궨��                        */
/*-------------------------------*/

/* �������� */
#define SAT_TYPE_GPS  0       /* GPS����    */
#define SAT_TYPE_BDS  1       /* BDS����    */
#define SAT_TYPE_GLO  2       /* GLO����    */
#define SAT_TYPE_GAL  3       /* GAL����    */  

/* ���ջ�����ģʽ */
#define REV_ACMODE_GPS    0x01    /* ��GPS��λ      */
#define REV_ACMODE_BDS    0x02    /* ��BDS��λ      */
#define REV_ACMODE_GLO    0x04    /* ��GLO��λ      */
#define REV_ACMODE_GAL    0x08    /* ��GAL��λ      */
#define REV_ACMODE_ALL    0x0F    /* ��ϵͳ���϶�λ */ 

/* ���ջ�����ģʽ */
#define REV_RESETMODE_ALL         0x01   /* �����������ã�����������λ����Ϣ�ȣ�Ȼ������ */
#define REV_RESETMODE_CONFIGSAVE  0x02   /* �����û�����������          */
#define REV_RESETMODE_EPHEM       0x03   /* ����������������������    */
#define REV_RESETMODE_POSITION    0x04   /* ��������λ����Ϣ������    */
#define REV_RESETMODE_ALMANAC     0x05   /* ��������������Ϣ������    */
#define REV_RESETMODE_IONUTC      0x06   /* ��������� UTC ���������� */      


/* ���ջ����Ͷ��� */
#define REC_TYPE_UT4B0 0    /* UT4B0���ջ� */


/*-------------------------------*/
/* ��������                      */
/*-------------------------------*/
extern void gnss_receiver_msg_parser(u8_t *pdata);
extern void gnss_receiver_init(void);
extern void gnss_receiver_acmode_set(u8_t ac_mode);
extern void gnss_receiver_msg_set(u8_t ac_mode);
extern void gnss_receiver_reset(u8_t reset_mode);


#endif
