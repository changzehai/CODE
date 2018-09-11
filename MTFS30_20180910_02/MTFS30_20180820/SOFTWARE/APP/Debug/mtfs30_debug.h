/*****************************************************************************/
/* 文件名:    mtfs30_debug.h                                                 */
/* 描  述:    调试处理头文件                                                 */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#ifndef __MTFS30_DEBUG_H_
#define __MTFS30_DEBUG_H_
#include <includes.h>
#include "arch/cc.h"
#include "lwip/api.h"
#include "Config/config.h"
#include "opt.h"


/*-------------------------------*/
/* 宏定义                        */
/*-------------------------------*/
#define  DEBUG_MSG_MAX_LEN        250     /* 每条调试信息最大长度 */

/* 调试开关 */
#define  MTFS30_DEBUG_EN             1       /* 使能调试信息输出     */
#define  MTFS30_ERROR_EN             0       /* 使能错误信息输出     */
#define  MTFS30_NET_DEBUG_EN         1       /* 使能网络调试         */
#define  MTFS30_USART_DEBUG_EN       0       /* 使能串口调试         */



/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
extern void debug_init( void );
extern void mtfs30_format_debug(u8_t *format, ...);

/*-------------------------------*/
/* 宏函数定义                    */
/*-------------------------------*/
#define  MTFS30_ERROR(fmt, args...)       mtfs30_format_debug("[ERROR][%s:%d] "fmt, __FUNCTION__, __LINE__, ##args) /* 输出错误信息 */
#define  MTFS30_DEBUG(fmt, args...)       mtfs30_format_debug("[DEBUG][%s:%d] "fmt, __FUNCTION__, __LINE__, ##args) /* 输出调试信息 */


#endif