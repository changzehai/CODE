/*****************************************************************************/
/* 文件名:    net_debug.c                                                    */
/* 描  述:    网络相关处理                                                   */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include "net_debug.h"
#include "lwip/api.h"


static struct netconn* conn;



static void net_send(u8_t *pdata);

/*****************************************************************************
 * 函  数:    debug_network_init                                                           
 * 功  能:    初始试用网络连接                                                                 
 * 输  入:    无                       
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void debug_network_init(void)
{

    ip_addr_t server_ipaddr;
    err_t err;
    
    /* 创建TCP连接  */
    conn = netconn_new( NETCONN_TCP );
    
    if (conn != NULL)
    {
        IP4_ADDR(&server_ipaddr, 172, 18, 5, 89);// 服务器IP地址
        err = netconn_connect(conn, &server_ipaddr, 8888);
        
        if (err == ERR_OK)
        {
            netconn_write( conn, "MTFS30 CONNECT SUCCESS!", 23,  NETCONN_NOCOPY );
        }
         
    }
    
}

/*****************************************************************************
 * 函  数:    net_send                                                           
 * 功  能:    通过网络发送信息                                                                 
 * 输  入:    无                       
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static void net_send(u8_t *pdata)
{
    u16_t len;
    u8_t buf[NET_SEND_MAX] = {0};
    
    /* 计算发送长度 */
    len = strlen(pdata);
    
    /* 超过最大发送长度 */
    if (len + 1 > NET_SEND_MAX)
    {
        len = NET_SEND_MAX - 1;
    }
    
    strncpy(buf, pdata, len);
    
    netconn_write(conn, buf, NET_SEND_MAX,  NETCONN_NOCOPY);
   
}

/*****************************************************************************
 * 函  数:    net_debug                                                           
 * 功  能:    通过网络发送调试信息                                                                 
 * 输  入:    无                       
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void net_debug(u8_t *pdata)
{
    net_send(pdata);  
}

