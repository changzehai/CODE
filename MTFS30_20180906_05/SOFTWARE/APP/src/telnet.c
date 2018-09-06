/*****************************************************************************
 * 文件名:    telnet.c
 * 描  述:    telnet相关处理
 * 创  建:    2018-06-12 changzehai(DTT)
 * 更  新:    无
 * Copyright 1998 - 2018 DTT. All Rights Reserved
 *****************************************************************************/
#include <includes.h>
#include "api.h"
#include "lwip/debug.h"
#include "httpd.h"
#include "lwip/tcp.h"
#include "lwip/inet.h"

#include "fs.h"
#include "ustdlib.h"
#include "config.h"
#include <string.h>
#include <stdlib.h>
#include "telnet.h"

#define  TCP_CLIENT_MAX 7
OS_TCB   TcpClientTaskTCB[TCP_CLIENT_MAX];
CPU_STK  TcpClientTaskStk[TCP_CLIENT_MAX][LWIP_STK_SIZE];

u8_t client_status[TCP_CLIENT_MAX];


#define TCP_SERVER_THREAD_PRIO    0
#define TCP_Client_THREAD_PRIO    15

extern OS_TCB        LwIP_task_TCB[LWIP_TASK_MAX];

static void Telnet_Server( void* arg );
static void Telnet_Client( void* arg );
static err_t Telnet_Client_Task_Create( void* arg );


//tcp服务器任务
/*****************************************************************************/
/* 函  数:    Telnet_Server                                                  */
/* 功  能:    处理客户端的telnet请求                                         */
/* 输  入:    prg 是OSTaskCreate()Telnet_Server()的参数。                    */
/* 输  出:    无                                                             */
/* 返回值:    无                                                             */
/* 创  建:    2018-05-12 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/*****************************************************************************/
static void Telnet_Server( void* arg )
{ 
    err_t err;               /* 函数返回值       */           
    OS_ERR os_err;           /* 用于记录系统错误 */
    struct netconn* conn;    /* 服务端连接       */
    struct netconn* newconn; /* 客户端连接       */
    OS_MSG_SIZE  msg_size;   /* 消息大小         */
    client_t* client;        /* 客户端连接信息   */
    
    
    LWIP_UNUSED_ARG( arg );
    
    /* 创建TCP连接 */
    conn = netconn_new(NETCONN_TCP);
    
    /* 绑定telnet服务端口(23) */
    netconn_bind(conn, IP_ADDR_ANY, 23);
    
    /* 进入监听模式 */
    netconn_listen( conn );
    
#if LWIP_SO_RCVTIMEO
    /* 禁止阻塞线程 等待10ms */
    conn->recv_timeout = 10;
#endif
    
    /* zhu循环处理，等待新的客户端连接 */
    while(1)
    {
        /* 等待接收新的客户端连接请求 */
        err = netconn_accept(conn, &newconn);
        
#if LWIP_SO_RCVTIMEO
        /* 使能禁止阻塞线程的情况下，返回ERR_TIMEOUT说明一段时间内没有连接请求，不是错误 */
        if (err == ERR_TIMEOUT) 
        {
            /* 接收客户端子任务发来的消息 */
            client = (client_t*)OSTaskQPend(0, OS_OPT_PEND_NON_BLOCKING, &msg_size, NULL, &os_err);
            
            /* 如果收到客户端子任务的消息，表明该客户端已经下线，释放该客户端堆栈 */
            if ((os_err == OS_ERR_NONE) && (client != NULL))
            {
                /* 释放断线客户端的堆栈及TCB空间 */
                free( client->Client_TaskTCB );
                free( client->Client_TASK_STK );
                free( client );
            }
            
            continue; /* 直接返回，重新等待连接 */
        }
        
        
        if (err == ERR_OK)
        {
            /* 禁止阻塞线程 等待5ms */
            newconn->recv_timeout = 5;
        }
#endif
        
        if (err == ERR_OK) /* 处理新连接的数据 */
        {
            /* 创建新的子任务处理该新连接 */
            if (Telnet_Client_Task_Create((void*)newconn) != ERR_OK)
            {
                netconn_close(newconn);
                netconn_delete(newconn);
            }
        }
        else /* 系统错误 */
        {
            netconn_close( conn );
            netconn_delete( conn );

        }
    }
}



//子连接任务
static void Telnet_Client( void* arg )
{
    struct netbuf* recvbuf;
    //  struct pbuf *q;
    void* data;
    u16_t data_len = 0;     //当前pbuf的数据长度
    //  u16_t len = 0;              //当前接收的数据总长度
    err_t recv_err;
    OS_ERR os_err;
    client_t* client = ( client_t* )arg;
    while( ( recv_err = netconn_recv( client->newconn, &recvbuf ) ) == ERR_OK ) //接收到数据
    {
        do
        {
            netbuf_data( recvbuf, &data, &data_len );
            netconn_write( client->newconn, data, data_len, NETCONN_NOCOPY ); //发送tcp_server_sendbuf中的数据
        }
        while( netbuf_next( recvbuf ) >= 0 );
        netbuf_delete( recvbuf );
    }
    netconn_close( client->newconn );
    netconn_delete( client->newconn );
    OSTaskQPost( &LwIP_task_TCB[TCP_SERVER_THREAD_PRIO], client, sizeof( client_t ), OS_OPT_POST_FIFO, &os_err );
    LWIP_ASSERT( "os_err == OS_ERR_NONE", os_err == OS_ERR_NONE );
    OSTaskDel( NULL, NULL ); //删除自己任务，该任务用到的堆栈和TCB空间在server thread任务中释放
}

//创建tcp server的连接子任务
//参数arg为创建子连接任务时，tcp server主任务传递的新的struct netconn * newconn
/*****************************************************************************/
/* 函  数:    Telnet_Client_Task_Create                                      */
/* 功  能:    创建子任务用以处理新的客户端连接                               */
/* 输  入:    prg 新的客户端连接信息                                         */
/* 输  出:    无                                                             */
/* 返回值:    无                                                             */
/* 创  建:    2018-05-12 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/*****************************************************************************/
static err_t Telnet_Client_Task_Create( void* arg )
{
    OS_ERR err;
    client_t* client;
    static u8_t pro = 0;    //初始化连接子任务的优先级索引
    CPU_SR_ALLOC();
    client = ( client_t* )calloc( 1, sizeof( client_t ) );          //给任务控制块分配空间
    if( client == NULL )
    {
        return ERR_MEM;
    }
    client->newconn = ( struct netconn* )arg;
    client->Client_TaskTCB   = ( OS_TCB* )calloc( 1, sizeof( OS_TCB ) );          //给任务控制块分配空间
    if( client->Client_TaskTCB == NULL )
    {
        //到这一步失败，必须释放之前申请成功的内存，原则就是一荣俱荣
        free( client );
        return ERR_MEM;
    }
    client->Client_TASK_STK = ( CPU_STK* )calloc( 1, CLIENT_TASK_STACK_SIZE * sizeof( CPU_STK ) ); //给任务堆栈分配空间
    if( client->Client_TASK_STK == NULL )
    {
        free( client->Client_TaskTCB );
        free( client );
        return ERR_MEM;
    }
    CPU_CRITICAL_ENTER();   //关中断
    OSTaskCreate( ( OS_TCB* )( client->Client_TaskTCB ),        //任务控制块
                  ( CPU_CHAR* )"Echo Client Task",           //任务名字
                  ( OS_TASK_PTR )Telnet_Client,            //任务函数
                  ( void* )client,                           //传递给任务函数的参数
                  ( OS_PRIO )CLIENT_TASK_PRIO_BASE + ( pro++ ),      //任务优先级
                  ( CPU_STK* )( client->Client_TASK_STK ),                   //任务堆栈基地址
                  ( CPU_STK* ) & ( client->Client_TASK_STK[CLIENT_TASK_STACK_SIZE / 10] ), //任务堆栈深度限位
                  ( OS_STK_SIZE )CLIENT_TASK_STACK_SIZE,     //任务堆栈大小
                  ( OS_MSG_QTY )0,                       //任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                  ( OS_TICK )0,                          //当使能时间片轮转时的时间片长度，为0时为默认长度，
                  ( void* )0,                            //用户补充的存储区
                  ( OS_OPT )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,   //任务选项
                  ( OS_ERR* )&err );                     //存放该函数错误时的返回值
    CPU_CRITICAL_EXIT();        //开中断
    if( err != OS_ERR_NONE )
    {
        free( client->Client_TaskTCB );
        free( client->Client_TASK_STK );
        free( client );
        return ERR_RTE;
    }
    return ERR_OK;
}


/*****************************************************************************/
/* 函  数:    Telnet_Server_Task                                             */
/* 功  能:    创建telnet服务端主任务，处理客户端的telnet请求                 */
/* 输  入:    无                                                             */
/* 输  出:    无                                                             */
/* 返回值:    无                                                             */
/* 创  建:    2018-05-12 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/*****************************************************************************/
void Telnet_Server_Task( void )
{
    sys_thread_new( "Echo_Server_Task", Telnet_Server, NULL, DEFAULT_THREAD_STACKSIZE, TCP_SERVER_THREAD_PRIO );
}