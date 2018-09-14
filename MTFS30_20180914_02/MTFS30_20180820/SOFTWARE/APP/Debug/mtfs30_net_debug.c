/*****************************************************************************/
/* �ļ���:    mtfs30_net_debug.c                                             */
/* ��  ��:    ������Դ���                                                       */
/* ��  ��:    2018-07-20 changzehai(DTT)                                     */
/* ��  ��:    ��                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include "mtfs30_debug.h"
#include "mtfs30_net_debug.h"
#include <includes.h>
#include "arch/cc.h"
#include "lwip/api.h"
#include "lwip/ip.h"
#include "lwip/tcp.h"
#include "Config/config.h"
#include "opt.h"


/*-------------------------------*/
/* ��������                      */
/*-------------------------------*/
static OS_TCB     gtask_net_debug_tcb;                       /* �������������ƿ�   */
static CPU_STK    gtask_net_debug_stk[NET_DEBUG_STACK_SIZE]; /* �����������ջ       */
static OS_Q       gnet_debug_msg;                            /* �����������Ϣ����   */
static OS_SEM     gnet_debug_sem;                            /* ����������ź���     */
static OS_MEM     gnet_debug_mem;                            /* ����������ڴ���ƿ� */

/*-------------------------------*/
/* ��������                      */
/*-------------------------------*/
static void net_debug_task( void* arg );



/*****************************************************************************/
/* ��  ��:    net_debug_init                                                 */
/* ��  ��:    ������Գ�ʼ��                                                 */
/* ��  ��:    ��                                                             */
/* ��  ��:    ��                                                             */
/* ����ֵ:    ��                                                             */
/* ��  ��:    2018-05-12 changzehai(DTT)                                     */
/* ��  ��:    ��                                                             */
/*****************************************************************************/
void net_debug_init( void )
{
    OS_ERR err;

    
    /* �����������ź��� */
    OSSemCreate ((OS_SEM*   ) &gnet_debug_sem,
                 (CPU_CHAR* ) "NET DEBUG SEM",
                 (OS_SEM_CTR) NET_DEBUG_MSGQEUE_SIZE,
                 (OS_ERR*   ) &err);   
    
    /* ������������Ϣ���� */
    OSQCreate((OS_Q*      ) &gnet_debug_msg,      
              (CPU_CHAR*  ) "NET DEBUG MSG",
              (OS_MSG_QTY ) NET_DEBUG_MSGQEUE_SIZE,
              (OS_ERR*    ) &err);  
    
    /* ΪҪ�������ڴ���������ڴ� */
    void *pnet_debug_mem_addr = malloc(NET_DEBUG_MSGQEUE_SIZE * DEBUG_MSG_MAX_LEN);

    if (pnet_debug_mem_addr == NULL)
    {
        //MTFS30_DEBUG("OS_MEM MALLOC ERROR");
        return;
    }
    
    /* �����ڴ���� */
    OSMemCreate((OS_MEM     *)&gnet_debug_mem,
               (CPU_CHAR    *)"NET DEBUG MEM",
               (void        *)pnet_debug_mem_addr,   /* �ڴ������ʼ��ַ       */
               (OS_MEM_QTY   )NET_DEBUG_MSGQEUE_SIZE,/* �ڴ��������ڴ������ */
               (OS_MEM_SIZE  )DEBUG_MSG_MAX_LEN,     /*ÿ���ڴ��Ĵ�С(�ֽ�)  */
               (OS_ERR      *)&err);
    
    /* ��������������� */
    OSTaskCreate( (OS_TCB        *)	&gtask_net_debug_tcb,
				  (CPU_CHAR      *)	"net debug task",
				  (OS_TASK_PTR    )	net_debug_task,
				  (void          *) 0,
				  (OS_PRIO        )	NET_DEBUG_TASK_PRIO,
				  (CPU_STK       *)	&gtask_net_debug_stk[0],
                  (CPU_STK       *) &gtask_net_debug_stk[NET_DEBUG_STACK_SIZE / 10],
				  (OS_STK_SIZE    )	NET_DEBUG_STACK_SIZE,
				  (OS_MSG_QTY     ) 0,
				  (OS_TICK        ) 0,
				  (void          *) 0,
				  (OS_OPT         ) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				  (OS_ERR        *) &err);		
 
}





/*****************************************************************************
 * ��  ��:    mtfs30_net_debug                                                           
 * ��  ��:    ͨ���������������Ϣ                                                                 
 * ��  ��:    ��                       
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/
void mtfs30_net_debug(u8_t *pnet_debug_data)
{
   u8_t *pnet_debug = NULL;
   u16_t debug_msg_len = 0;
   OS_ERR err;
   OS_SEM_CTR sem;

  
   /* pend�����ź��� */
   sem = OSSemPend ((OS_SEM *) &gnet_debug_sem,
                    (OS_TICK ) 0,
                    (OS_OPT  ) OS_OPT_PEND_BLOCKING,
                    (CPU_TS *) NULL,
                    (OS_ERR *) &err);
   
     

        
   /* ȡ�õ�����Ϣ���� */
   debug_msg_len = strlen((char *)pnet_debug_data);
   
   /* ������󳤶� */
   if (debug_msg_len > DEBUG_MSG_MAX_LEN - 1)
   {
       debug_msg_len = DEBUG_MSG_MAX_LEN - 1;
   }
   
   /* ��ȡһ���ڴ�� */
   pnet_debug = OSMemGet((OS_MEM *)&gnet_debug_mem, ((OS_ERR*    ) &err));
   if (pnet_debug != NULL)
   {
       /* ��������Ϣ��������ȡ�����ڴ�� */
       strcpy((char *)pnet_debug, (char *)pnet_debug_data);
       pnet_debug[debug_msg_len] = '\0';
       
       /* ��װ�е�����Ϣ���ڴ�������Ϣ���� */
       OSQPost((OS_Q*      ) &gnet_debug_msg,     
               (void*      ) pnet_debug,
               (OS_MSG_SIZE) debug_msg_len+1,
               (OS_OPT     ) OS_OPT_POST_FIFO,
               (OS_ERR*    ) &err);
  
        /* ���� */
        if (err != OS_ERR_NONE)
        {
            /* �黹��ȡ�����ڴ�� */
            OSMemPut((OS_MEM  *)&gnet_debug_mem, pnet_debug, &err);
            pnet_debug = NULL;
            
        }  
   }
   
}


/*****************************************************************************/
/* ��  ��:    net_debug_task                                                 */
/* ��  ��:    ����Ϣ������ȡ��������Ϣ�������͸������                       */
/* ��  ��:    ��                                                             */
/* ��  ��:    ��                                                             */
/* ����ֵ:    ��                                                             */
/* ��  ��:    2018-05-12 changzehai(DTT)                                     */
/* ��  ��:    ��                                                             */
/*****************************************************************************/
static void net_debug_task( void* arg )
{ 
    void* data;
	u16_t data_len = 0;
	struct pbuf *q;
	err_t err,recv_err;
	static ip_addr_t server_ipaddr;
	static u16_t 		 server_port;
    u8_t *pmsg = NULL;
    OS_MSG_SIZE msg_size;
	OS_ERR  os_err;
    struct netconn *conn;

    struct netbuf *recvbuf;
    OS_SEM_CTR debug_sem;
    
    
    
	LWIP_UNUSED_ARG(arg);
    
    /* ����˶˿� */
	server_port = 1111;
    /* �����IP��ַ */
	IP4_ADDR(&server_ipaddr, 172, 18, 5, 138);

	
	while (1) 
	{
        /* ����һ��TCP���� */
		conn = netconn_new(NETCONN_TCP);  
        if (conn == NULL) 
        {
//            USART_DEBUG("netconn_new failed!");
            continue;
        }
        
        /* ���ӷ����� */
		err = netconn_connect(conn,&server_ipaddr, server_port);
		if (err != ERR_OK) 
        {
            netconn_delete(conn); /* ɾ��conn���� */
        }
		else if (err == ERR_OK)    //���������ӵ�����
		{ 
		
            conn->pcb.tcp->so_options |= SOF_KEEPALIVE;
#if LWIP_SO_RCVTIMEO
    /* ��ֹ�����߳� �ȴ�10ms */
    conn->recv_timeout = 10;
#endif            
			while(1)
			{
                /* ����Ϣ������ȡ��������Ϣ */
		        pmsg = OSQPend((OS_Q*        ) &gnet_debug_msg,   
				               (OS_TICK      ) 10,
				               (OS_OPT       ) OS_OPT_PEND_BLOCKING,
				               (OS_MSG_SIZE* ) &msg_size, 
				               (CPU_TS*      ) 0,
				               (OS_ERR*      ) &err);


		       if (pmsg != NULL)
		       {  
            
                   /* ���͵�����Ϣ������� */
                   netconn_write(conn, pmsg, msg_size, NETCONN_NOCOPY ); 
                   
#ifdef DEBUG_USART /* �������������Ϣ */
                   
                   USART_DEBUG("%s", pmsg); 
#endif                   

                   /* �黹�ڴ� */
                   OSMemPut((OS_MEM  *)&gnet_debug_mem, pmsg, &os_err);
                   pmsg = NULL;
                   
#ifdef DEBUG_USART /* �������������Ϣ */  
                   /* ��Ϣ����ʣ������ */
                   u16_t msgq_remain_size; 
                   msgq_remain_size = gdebug_msg.MsgQ.NbrEntriesSize - gdebug_msg.MsgQ.NbrEntries;
                   USART_DEBUG("msgq_remain_size = %d", msgq_remain_size);
#endif 
                   
                   
                   /* post�����ź��� */
                   debug_sem = OSSemPost ((OS_SEM* ) &gnet_debug_sem,
                                          (OS_OPT  ) OS_OPT_POST_1,
                                          (OS_ERR *) &err);
                   
#ifdef DEBUG_USART /* �������������Ϣ */                    
                   /* ���µ��ź�ֵ */
                   USART_DEBUG("debug_sem = %d", debug_sem);
#endif                  
		       }
               

#ifdef TODO                          
				/* ���յ�����˷��͵����� */	
				if ((recv_err = netconn_recv(conn,&recvbuf)) == ERR_OK)  
				{	
                    /* �����յ�������ԭ�������������� */
                    do
                    {
                        netbuf_data( recvbuf, &data, &data_len );
                        netconn_write(conn, data, data_len, NETCONN_NOCOPY ); 
                    }while( netbuf_next( recvbuf ) >= 0 );
                    netbuf_delete( recvbuf );
				}
                /* �ر����� */
                else if(recv_err == ERR_CLSD) 
				{
					netconn_close(conn);
					netconn_delete(conn);
					break;
				}
                
             if((conn->pcb.tcp->so_options & SOF_KEEPALIVE) && 
                 (conn->pcb.tcp->state != ESTABLISHED))
                (conn->pcb.tcp->state == CLOSE_WAIT) || 
                (conn->pcb.tcp->state == CLOSED) || 
                (conn->pcb.tcp->state == TIME_WAIT)) 
             {
					netconn_close(conn);
					netconn_delete(conn);
					break;                     
             }
#endif             
               /* �������100ms */
               OSTimeDlyHMSM(0, 0,0,50, OS_OPT_TIME_HMSM_STRICT,&os_err);
			}
		}

	}
}