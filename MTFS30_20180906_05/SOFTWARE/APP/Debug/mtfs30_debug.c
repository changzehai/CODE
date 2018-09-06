/*****************************************************************************/
/* �ļ���:    mtfs30_debug.c                                                 */
/* ��  ��:    ���Դ���                                                       */
/* ��  ��:    2018-07-20 changzehai(DTT)                                     */
/* ��  ��:    ��                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include "mtfs30_debug.h"
#include "mtfs30_net_debug.h"
#include <includes.h>
#include "arch/cc.h"



/*-------------------------------*/
/* ��������                      */
/*-------------------------------*/
static void net_debug_task( void* arg );




/*****************************************************************************/
/* ��  ��:    net_debug_init                                                 */
/* ��  ��:    ���Թ��ܳ�ʼ��                                                 */
/* ��  ��:    ��                                                             */
/* ��  ��:    ��                                                             */
/* ����ֵ:    ��                                                             */
/* ��  ��:    2018-05-12 changzehai(DTT)                                     */
/* ��  ��:    ��                                                             */
/*****************************************************************************/
void debug_init( void )
{

#if MTFS30_NET_DEBUG_EN
    /* ������Գ�ʼ�� */
    net_debug_init();
#endif
    
    
}


/*****************************************************************************
 * ��  ��:    mtfs30_format_debug                                                           
 * ��  ��:    ��������Ϣ��ʽ���������                                                                 
 * ��  ��:    ��                       
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/
void mtfs30_format_debug(u8_t *format, ...)
{
        u8_t buf[DEBUG_MSG_MAX_LEN]; /* ���ڴ�Ÿ�ʽ����ĵ�����Ϣ */
        va_list args; 
        
        
        memset(buf, 0x00, sizeof(buf));
        
        va_start(args, format);
        
        /* ��ʽ��������Ϣ */
        vsnprintf(buf, DEBUG_MSG_MAX_LEN, (const char *)format, args);
        
        va_end(args);
        
#if MTFS30_NET_DEBUG_EN
        /* ͨ���������������Ϣ */
        mtfs30_net_debug((u8_t *)buf);
#endif
        
        
#if MTFS30_USART_DEBUG_EN
        /* ͨ�����ڵ�����Ϣ */
        usart_send((u8_t *)buf);
#endif
        
}