/*****************************************************************************/
/* 文件名:    mtfs30_usart.h                                                 */
/* 描  述:    串口相关处理头文件                                             */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#ifndef __MTFS30_USART_H_
#define __MTFS30_USART_H_
#include "cc.h"


/*-------------------------------*/
/* 宏定义                        */
/*-------------------------------*/

/* 串口发送数据的最大长度 */
#define USART_SEND_LEN_MAX    1024 

/* 串口号                 */
#define USART_COM_1    1    /* 串口1 */
#define USART_COM_2    2    /* 串口2 */
#define USART_COM_3    3    /* 串口3 */

/* 缓冲区大小 */
#define USART1_BUF_SIZE       1024     /* 串口1缓冲区大小   */
#define USART2_BUF_SIZE       1024     /* 串口2缓冲区大小   */
#define USART1_MSGQEUE_SIZE   10        /* 串口1消息队列大小 */
#define USART1_MSGQEUE_LEN    1024     /* 串口1消息长度     */


/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
void USART1_IRQHandler(void);
void USART2_IRQHandler(void);
void USART3_IRQHandler(void);
void sys_usart_init(void);
void usart_send(u8_t *pdata, u8_t com);


#endif
