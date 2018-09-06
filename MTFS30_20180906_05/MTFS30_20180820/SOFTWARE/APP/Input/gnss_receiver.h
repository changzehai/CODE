/*****************************************************************************/
/* 文件名:    gnss_receiver.h                                                */
/* 描  述:    GNSS接收器相关处理头文件                                       */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#ifndef __GNSS_RECEIVER_H_
#define __GNSS_RECEIVER_H_
#include "cc.h"


/*-------------------------------*/
/* 宏定义                        */
/*-------------------------------*/

/* 卫星类型 */
#define SAT_TYPE_GPS  0       /* GPS卫星    */
#define SAT_TYPE_BDS  1       /* BDS卫星    */
#define SAT_TYPE_GLO  2       /* GLO卫星    */
#define SAT_TYPE_GAL  3       /* GAL卫星    */  

/* 接收机工作模式 */
#define REV_ACMODE_GPS    0x01    /* 单GPS定位      */
#define REV_ACMODE_BDS    0x02    /* 单BDS定位      */
#define REV_ACMODE_GLO    0x04    /* 单GLO定位      */
#define REV_ACMODE_GAL    0x08    /* 单GAL定位      */
#define REV_ACMODE_ALL    0x0F    /* 多系统联合定位 */ 

/* 接收机重启模式 */
#define REV_RESETMODE_ALL         0x01   /* 清除保存的设置，卫星星历、位置信息等，然后重启 */
#define REV_RESETMODE_CONFIGSAVE  0x02   /* 保存用户配置再重启          */
#define REV_RESETMODE_EPHEM       0x03   /* 清除保存的卫星星历再重启    */
#define REV_RESETMODE_POSITION    0x04   /* 清除保存的位置信息再重启    */
#define REV_RESETMODE_ALMANAC     0x05   /* 清除保存的历书信息再重启    */
#define REV_RESETMODE_IONUTC      0x06   /* 清除电离层和 UTC 参数再重启 */      


/* 接收机类型定义 */
#define REC_TYPE_UT4B0 0    /* UT4B0接收机 */


/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
extern void gnss_receiver_msg_parser(u8_t *pdata);
extern void gnss_receiver_init(void);
extern void gnss_receiver_acmode_set(u8_t ac_mode);
extern void gnss_receiver_msg_set(u8_t ac_mode);
extern void gnss_receiver_reset(u8_t reset_mode);


#endif
