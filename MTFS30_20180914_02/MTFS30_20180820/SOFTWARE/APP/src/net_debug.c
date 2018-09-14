/*****************************************************************************/
/* �ļ���:    net_debug.c                                                    */
/* ��  ��:    ������ش���                                                   */
/* ��  ��:    2018-07-20 changzehai(DTT)                                     */
/* ��  ��:    ��                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include "net_debug.h"
#include "lwip/api.h"


static struct netconn* conn;



static void net_send(u8_t *pdata);

/*****************************************************************************
 * ��  ��:    debug_network_init                                                           
 * ��  ��:    ��ʼ������������                                                                 
 * ��  ��:    ��                       
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/
void debug_network_init(void)
{

    ip_addr_t server_ipaddr;
    err_t err;
    
    /* ����TCP����  */
    conn = netconn_new( NETCONN_TCP );
    
    if (conn != NULL)
    {
        IP4_ADDR(&server_ipaddr, 172, 18, 5, 89);// ������IP��ַ
        err = netconn_connect(conn, &server_ipaddr, 8888);
        
        if (err == ERR_OK)
        {
            netconn_write( conn, "MTFS30 CONNECT SUCCESS!", 23,  NETCONN_NOCOPY );
        }
         
    }
    
}

/*****************************************************************************
 * ��  ��:    net_send                                                           
 * ��  ��:    ͨ�����緢����Ϣ                                                                 
 * ��  ��:    ��                       
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/
static void net_send(u8_t *pdata)
{
    u16_t len;
    u8_t buf[NET_SEND_MAX] = {0};
    
    /* ���㷢�ͳ��� */
    len = strlen(pdata);
    
    /* ��������ͳ��� */
    if (len + 1 > NET_SEND_MAX)
    {
        len = NET_SEND_MAX - 1;
    }
    
    strncpy(buf, pdata, len);
    
    netconn_write(conn, buf, NET_SEND_MAX,  NETCONN_NOCOPY);
   
}

/*****************************************************************************
 * ��  ��:    net_debug                                                           
 * ��  ��:    ͨ�����緢�͵�����Ϣ                                                                 
 * ��  ��:    ��                       
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/
void net_debug(u8_t *pdata)
{
    net_send(pdata);  
}

