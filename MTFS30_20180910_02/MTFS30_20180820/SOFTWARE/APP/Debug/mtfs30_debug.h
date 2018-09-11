/*****************************************************************************/
/* �ļ���:    mtfs30_debug.h                                                 */
/* ��  ��:    ���Դ���ͷ�ļ�                                                 */
/* ��  ��:    2018-07-20 changzehai(DTT)                                     */
/* ��  ��:    ��                                                             */
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
/* �궨��                        */
/*-------------------------------*/
#define  DEBUG_MSG_MAX_LEN        250     /* ÿ��������Ϣ��󳤶� */

/* ���Կ��� */
#define  MTFS30_DEBUG_EN             1       /* ʹ�ܵ�����Ϣ���     */
#define  MTFS30_ERROR_EN             0       /* ʹ�ܴ�����Ϣ���     */
#define  MTFS30_NET_DEBUG_EN         1       /* ʹ���������         */
#define  MTFS30_USART_DEBUG_EN       0       /* ʹ�ܴ��ڵ���         */



/*-------------------------------*/
/* ��������                      */
/*-------------------------------*/
extern void debug_init( void );
extern void mtfs30_format_debug(u8_t *format, ...);

/*-------------------------------*/
/* �꺯������                    */
/*-------------------------------*/
#define  MTFS30_ERROR(fmt, args...)       mtfs30_format_debug("[ERROR][%s:%d] "fmt, __FUNCTION__, __LINE__, ##args) /* ���������Ϣ */
#define  MTFS30_DEBUG(fmt, args...)       mtfs30_format_debug("[DEBUG][%s:%d] "fmt, __FUNCTION__, __LINE__, ##args) /* ���������Ϣ */


#endif