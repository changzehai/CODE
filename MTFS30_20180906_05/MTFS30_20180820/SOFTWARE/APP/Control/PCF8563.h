/****************************************************************************
* Copyright (C), 2009-2010, 武汉扬硕电子有限公司
*
* 文件名: bsp_PCF8563.h
* 内容简述: PCF8563时钟芯片驱动
*
* 文件历史:
* 版本号 日期       作者    说明
* 01a    2009-06-29 zhg  创建该文件
*
*/

#ifndef _BSP_PCF8563_H_
#define _BSP_PCF8563_H_

//#include "TypeDef.h"


#define RTC_YEAR_MIN	2009
#define RTC_YEAR_MAX	2099
#define RTC_MONTH_MIN   1		/*  */
#define RTC_DAY_MIN   	1		/*  */

#define RTC_HOUR_MIN   	0		/*  */
#define RTC_MINUTE_MIN 	0		/*  */
#define RTC_SECOND_MIN  0		/*  */

/* Base day (1901.1.1 DOW = 2), which is used for day calculate */
#define RTC_BASEYEAR	1901
#define RTC_BASEMONTH	1
#define RTC_BASEDAY		1
#define RTC_BASEDOW		2



void pcf8563_InitI2c(void);
unsigned char pcf8563_SetDateTime(RTC_T *_tRtc);
unsigned char pcf8563_GetDateTime(RTC_T *_tRtc);
unsigned char IsValidDateTime(RTC_T *_tRtc);
int GetDOW (unsigned short year, unsigned char month, unsigned char day);
void InitRtcStruct(RTC_T *_tRtc);
#endif

