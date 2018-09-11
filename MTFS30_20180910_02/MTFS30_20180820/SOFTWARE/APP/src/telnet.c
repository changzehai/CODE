/*****************************************************************************
 * �ļ���:    telnet.c
 * ��  ��:    telnet��ش���
 * ��  ��:    2018-06-12 changzehai(DTT)
 * ��  ��:    ��
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


//tcp����������
/*****************************************************************************/
/* ��  ��:    Telnet_Server                                                  */
/* ��  ��:    ����ͻ��˵�telnet����                                         */
/* ��  ��:    prg ��OSTaskCreate()Telnet_Server()�Ĳ�����                    */
/* ��  ��:    ��                                                             */
/* ����ֵ:    ��                                                             */
/* ��  ��:    2018-05-12 changzehai(DTT)                                     */
/* ��  ��:    ��                                                             */
/*****************************************************************************/
static void Telnet_Server( void* arg )
{ 
    err_t err;               /* ��������ֵ       */           
    OS_ERR os_err;           /* ���ڼ�¼ϵͳ���� */
    struct netconn* conn;    /* ���������       */
    struct netconn* newconn; /* �ͻ�������       */
    OS_MSG_SIZE  msg_size;   /* ��Ϣ��С         */
    client_t* client;        /* �ͻ���������Ϣ   */
    
    
    LWIP_UNUSED_ARG( arg );
    
    /* ����TCP���� */
    conn = netconn_new(NETCONN_TCP);
    
    /* ��telnet����˿�(23) */
    netconn_bind(conn, IP_ADDR_ANY, 23);
    
    /* �������ģʽ */
    netconn_listen( conn );
    
#if LWIP_SO_RCVTIMEO
    /* ��ֹ�����߳� �ȴ�10ms */
    conn->recv_timeout = 10;
#endif
    
    /* zhuѭ�������ȴ��µĿͻ������� */
    while(1)
    {
        /* �ȴ������µĿͻ����������� */
        err = netconn_accept(conn, &newconn);
        
#if LWIP_SO_RCVTIMEO
        /* ʹ�ܽ�ֹ�����̵߳�����£�����ERR_TIMEOUT˵��һ��ʱ����û���������󣬲��Ǵ��� */
        if (err == ERR_TIMEOUT) 
        {
            /* ���տͻ���������������Ϣ */
            client = (client_t*)OSTaskQPend(0, OS_OPT_PEND_NON_BLOCKING, &msg_size, NULL, &os_err);
            
            /* ����յ��ͻ������������Ϣ�������ÿͻ����Ѿ����ߣ��ͷŸÿͻ��˶�ջ */
            if ((os_err == OS_ERR_NONE) && (client != NULL))
            {
                /* �ͷŶ��߿ͻ��˵Ķ�ջ��TCB�ռ� */
                free( client->Client_TaskTCB );
                free( client->Client_TASK_STK );
                free( client );
            }
            
            continue; /* ֱ�ӷ��أ����µȴ����� */
        }
        
        
        if (err == ERR_OK)
        {
            /* ��ֹ�����߳� �ȴ�5ms */
            newconn->recv_timeout = 5;
        }
#endif
        
        if (err == ERR_OK) /* ���������ӵ����� */
        {
            /* �����µ���������������� */
            if (Telnet_Client_Task_Create((void*)newconn) != ERR_OK)
            {
                netconn_close(newconn);
                netconn_delete(newconn);
            }
        }
        else /* ϵͳ���� */
        {
            netconn_close( conn );
            netconn_delete( conn );

        }
    }
}



//����������
static void Telnet_Client( void* arg )
{
    struct netbuf* recvbuf;
    //  struct pbuf *q;
    void* data;
    u16_t data_len = 0;     //��ǰpbuf�����ݳ���
    //  u16_t len = 0;              //��ǰ���յ������ܳ���
    err_t recv_err;
    OS_ERR os_err;
    client_t* client = ( client_t* )arg;
    while( ( recv_err = netconn_recv( client->newconn, &recvbuf ) ) == ERR_OK ) //���յ�����
    {
        do
        {
            netbuf_data( recvbuf, &data, &data_len );
            netconn_write( client->newconn, data, data_len, NETCONN_NOCOPY ); //����tcp_server_sendbuf�е�����
        }
        while( netbuf_next( recvbuf ) >= 0 );
        netbuf_delete( recvbuf );
    }
    netconn_close( client->newconn );
    netconn_delete( client->newconn );
    OSTaskQPost( &LwIP_task_TCB[TCP_SERVER_THREAD_PRIO], client, sizeof( client_t ), OS_OPT_POST_FIFO, &os_err );
    LWIP_ASSERT( "os_err == OS_ERR_NONE", os_err == OS_ERR_NONE );
    OSTaskDel( NULL, NULL ); //ɾ���Լ����񣬸������õ��Ķ�ջ��TCB�ռ���server thread�������ͷ�
}

//����tcp server������������
//����argΪ��������������ʱ��tcp server�����񴫵ݵ��µ�struct netconn * newconn
/*****************************************************************************/
/* ��  ��:    Telnet_Client_Task_Create                                      */
/* ��  ��:    �������������Դ����µĿͻ�������                               */
/* ��  ��:    prg �µĿͻ���������Ϣ                                         */
/* ��  ��:    ��                                                             */
/* ����ֵ:    ��                                                             */
/* ��  ��:    2018-05-12 changzehai(DTT)                                     */
/* ��  ��:    ��                                                             */
/*****************************************************************************/
static err_t Telnet_Client_Task_Create( void* arg )
{
    OS_ERR err;
    client_t* client;
    static u8_t pro = 0;    //��ʼ����������������ȼ�����
    CPU_SR_ALLOC();
    client = ( client_t* )calloc( 1, sizeof( client_t ) );          //��������ƿ����ռ�
    if( client == NULL )
    {
        return ERR_MEM;
    }
    client->newconn = ( struct netconn* )arg;
    client->Client_TaskTCB   = ( OS_TCB* )calloc( 1, sizeof( OS_TCB ) );          //��������ƿ����ռ�
    if( client->Client_TaskTCB == NULL )
    {
        //����һ��ʧ�ܣ������ͷ�֮ǰ����ɹ����ڴ棬ԭ�����һ�پ���
        free( client );
        return ERR_MEM;
    }
    client->Client_TASK_STK = ( CPU_STK* )calloc( 1, CLIENT_TASK_STACK_SIZE * sizeof( CPU_STK ) ); //�������ջ����ռ�
    if( client->Client_TASK_STK == NULL )
    {
        free( client->Client_TaskTCB );
        free( client );
        return ERR_MEM;
    }
    CPU_CRITICAL_ENTER();   //���ж�
    OSTaskCreate( ( OS_TCB* )( client->Client_TaskTCB ),        //������ƿ�
                  ( CPU_CHAR* )"Echo Client Task",           //��������
                  ( OS_TASK_PTR )Telnet_Client,            //������
                  ( void* )client,                           //���ݸ��������Ĳ���
                  ( OS_PRIO )CLIENT_TASK_PRIO_BASE + ( pro++ ),      //�������ȼ�
                  ( CPU_STK* )( client->Client_TASK_STK ),                   //�����ջ����ַ
                  ( CPU_STK* ) & ( client->Client_TASK_STK[CLIENT_TASK_STACK_SIZE / 10] ), //�����ջ�����λ
                  ( OS_STK_SIZE )CLIENT_TASK_STACK_SIZE,     //�����ջ��С
                  ( OS_MSG_QTY )0,                       //�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                  ( OS_TICK )0,                          //��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                  ( void* )0,                            //�û�����Ĵ洢��
                  ( OS_OPT )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,   //����ѡ��
                  ( OS_ERR* )&err );                     //��Ÿú�������ʱ�ķ���ֵ
    CPU_CRITICAL_EXIT();        //���ж�
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
/* ��  ��:    Telnet_Server_Task                                             */
/* ��  ��:    ����telnet����������񣬴���ͻ��˵�telnet����                 */
/* ��  ��:    ��                                                             */
/* ��  ��:    ��                                                             */
/* ����ֵ:    ��                                                             */
/* ��  ��:    2018-05-12 changzehai(DTT)                                     */
/* ��  ��:    ��                                                             */
/*****************************************************************************/
void Telnet_Server_Task( void )
{
    sys_thread_new( "Echo_Server_Task", Telnet_Server, NULL, DEFAULT_THREAD_STACKSIZE, TCP_SERVER_THREAD_PRIO );
}