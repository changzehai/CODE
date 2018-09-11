/*****************************************************************************/
/* 文件名:    mtfs30_debug.c                                                 */
/* 描  述:    调试处理                                                       */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include "mtfs30_debug.h"
#include "mtfs30_net_debug.h"
#include <includes.h>
#include "arch/cc.h"



/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
static void net_debug_task( void* arg );




/*****************************************************************************/
/* 函  数:    net_debug_init                                                 */
/* 功  能:    调试功能初始化                                                 */
/* 输  入:    无                                                             */
/* 输  出:    无                                                             */
/* 返回值:    无                                                             */
/* 创  建:    2018-05-12 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/*****************************************************************************/
void debug_init( void )
{

#if MTFS30_NET_DEBUG_EN
    /* 网络调试初始化 */
    net_debug_init();
#endif
    
    
}


/*****************************************************************************
 * 函  数:    mtfs30_format_debug                                                           
 * 功  能:    将调试信息格式化后再输出                                                                 
 * 输  入:    无                       
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void mtfs30_format_debug(u8_t *format, ...)
{
        u8_t buf[DEBUG_MSG_MAX_LEN]; /* 用于存放格式化后的调试信息 */
        va_list args; 
        
        
        memset(buf, 0x00, sizeof(buf));
        
        va_start(args, format);
        
        /* 格式化调试信息 */
        vsnprintf(buf, DEBUG_MSG_MAX_LEN, (const char *)format, args);
        
        va_end(args);
        
#if MTFS30_NET_DEBUG_EN
        /* 通过网络输出调试信息 */
        mtfs30_net_debug((u8_t *)buf);
#endif
        
        
#if MTFS30_USART_DEBUG_EN
        /* 通过串口调试信息 */
        usart_send((u8_t *)buf);
#endif
        
}