/*****************************************************************************/
/* �ļ���:    mtfs30_usart.h                                                 */
/* ��  ��:    ������ش���ͷ�ļ�                                             */
/* ��  ��:    2018-07-20 changzehai(DTT)                                     */
/* ��  ��:    ��                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#ifndef __MTFS30_USART_H_
#define __MTFS30_USART_H_
#include "cc.h"


/*-------------------------------*/
/* �궨��                        */
/*-------------------------------*/

/* ���ڷ������ݵ���󳤶� */
#define USART_SEND_LEN_MAX    1024 

/* ���ں�                 */
#define USART_COM_1    1    /* ����1 */
#define USART_COM_2    2    /* ����2 */
#define USART_COM_3    3    /* ����3 */

/* ��������С */
#define USART1_BUF_SIZE       1024     /* ����1��������С   */
#define USART2_BUF_SIZE       1024     /* ����2��������С   */
#define USART1_MSGQEUE_SIZE   10        /* ����1��Ϣ���д�С */
#define USART1_MSGQEUE_LEN    1024     /* ����1��Ϣ����     */


/*-------------------------------*/
/* ��������                      */
/*-------------------------------*/
void USART1_IRQHandler(void);
void USART2_IRQHandler(void);
void USART3_IRQHandler(void);
void sys_usart_init(void);
void usart_send(u8_t *pdata, u8_t com);


#endif
