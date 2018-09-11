/*****************************************************************************/
/* 文件名:    mtfs30_net_debug.c                                             */
/* 描  述:    网络调试处理                                                       */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
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
/* 变量定义                      */
/*-------------------------------*/
static OS_TCB     gtask_net_debug_tcb;                       /* 网络调试任务控制块   */
static CPU_STK    gtask_net_debug_stk[NET_DEBUG_STACK_SIZE]; /* 网络调试任务栈       */
static OS_Q       gnet_debug_msg;                            /* 网络调试用消息队列   */
static OS_SEM     gnet_debug_sem;                            /* 网络调试用信号量     */
static OS_MEM     gnet_debug_mem;                            /* 网络调试用内存控制块 */

/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
static void net_debug_task( void* arg );



/*****************************************************************************/
/* 函  数:    net_debug_init                                                 */
/* 功  能:    网络调试初始化                                                 */
/* 输  入:    无                                                             */
/* 输  出:    无                                                             */
/* 返回值:    无                                                             */
/* 创  建:    2018-05-12 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/*****************************************************************************/
void net_debug_init( void )
{
    OS_ERR err;

    
    /* 创建调试用信号量 */
    OSSemCreate ((OS_SEM*   ) &gnet_debug_sem,
                 (CPU_CHAR* ) "NET DEBUG SEM",
                 (OS_SEM_CTR) NET_DEBUG_MSGQEUE_SIZE,
                 (OS_ERR*   ) &err);   
    
    /* 创建调试用消息队列 */
    OSQCreate((OS_Q*      ) &gnet_debug_msg,      
              (CPU_CHAR*  ) "NET DEBUG MSG",
              (OS_MSG_QTY ) NET_DEBUG_MSGQEUE_SIZE,
              (OS_ERR*    ) &err);  
    
    /* 为要创建的内存分区分配内存 */
    void *pnet_debug_mem_addr = malloc(NET_DEBUG_MSGQEUE_SIZE * DEBUG_MSG_MAX_LEN);

    if (pnet_debug_mem_addr == NULL)
    {
        //MTFS30_DEBUG("OS_MEM MALLOC ERROR");
        return;
    }
    
    /* 创建内存分区 */
    OSMemCreate((OS_MEM     *)&gnet_debug_mem,
               (CPU_CHAR    *)"NET DEBUG MEM",
               (void        *)pnet_debug_mem_addr,   /* 内存分区起始地址       */
               (OS_MEM_QTY   )NET_DEBUG_MSGQEUE_SIZE,/* 内存分区里的内存块数量 */
               (OS_MEM_SIZE  )DEBUG_MSG_MAX_LEN,     /*每个内存块的大小(字节)  */
               (OS_ERR      *)&err);
    
    /* 创建网络调试任务 */
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
 * 函  数:    mtfs30_net_debug                                                           
 * 功  能:    通过网络输出调试信息                                                                 
 * 输  入:    无                       
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void mtfs30_net_debug(u8_t *pnet_debug_data)
{
   u8_t *pnet_debug = NULL;
   u16_t debug_msg_len = 0;
   OS_ERR err;
   OS_SEM_CTR sem;

  
   /* pend调试信号量 */
   sem = OSSemPend ((OS_SEM *) &gnet_debug_sem,
                    (OS_TICK ) 0,
                    (OS_OPT  ) OS_OPT_PEND_BLOCKING,
                    (CPU_TS *) NULL,
                    (OS_ERR *) &err);
   
     

        
   /* 取得调试信息长度 */
   debug_msg_len = strlen((char *)pnet_debug_data);
   
   /* 超过最大长度 */
   if (debug_msg_len > DEBUG_MSG_MAX_LEN - 1)
   {
       debug_msg_len = DEBUG_MSG_MAX_LEN - 1;
   }
   
   /* 获取一个内存块 */
   pnet_debug = OSMemGet((OS_MEM *)&gnet_debug_mem, ((OS_ERR*    ) &err));
   if (pnet_debug != NULL)
   {
       /* 将调试信息拷贝进获取到的内存块 */
       strcpy((char *)pnet_debug, (char *)pnet_debug_data);
       pnet_debug[debug_msg_len] = '\0';
       
       /* 将装有调试信息的内存块放入消息队列 */
       OSQPost((OS_Q*      ) &gnet_debug_msg,     
               (void*      ) pnet_debug,
               (OS_MSG_SIZE) debug_msg_len+1,
               (OS_OPT     ) OS_OPT_POST_FIFO,
               (OS_ERR*    ) &err);
  
        /* 出错 */
        if (err != OS_ERR_NONE)
        {
            /* 归还获取到的内存块 */
            OSMemPut((OS_MEM  *)&gnet_debug_mem, pnet_debug, &err);
            pnet_debug = NULL;
            
        }  
   }
   
}


/*****************************************************************************/
/* 函  数:    net_debug_task                                                 */
/* 功  能:    从消息队列中取出调试信息，并发送给服务端                       */
/* 输  入:    无                                                             */
/* 输  出:    无                                                             */
/* 返回值:    无                                                             */
/* 创  建:    2018-05-12 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
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
    
    /* 服务端端口 */
	server_port = 1111;
    /* 服务端IP地址 */
	IP4_ADDR(&server_ipaddr, 172, 18, 5, 138);

	
	while (1) 
	{
        /* 创建一个TCP链接 */
		conn = netconn_new(NETCONN_TCP);  
        if (conn == NULL) 
        {
//            USART_DEBUG("netconn_new failed!");
            continue;
        }
        
        /* 连接服务器 */
		err = netconn_connect(conn,&server_ipaddr, server_port);
		if (err != ERR_OK) 
        {
            netconn_delete(conn); /* 删除conn连接 */
        }
		else if (err == ERR_OK)    //处理新连接的数据
		{ 
		
            conn->pcb.tcp->so_options |= SOF_KEEPALIVE;
#if LWIP_SO_RCVTIMEO
    /* 禁止阻塞线程 等待10ms */
    conn->recv_timeout = 10;
#endif            
			while(1)
			{
                /* 从消息队列中取出调试信息 */
		        pmsg = OSQPend((OS_Q*        ) &gnet_debug_msg,   
				               (OS_TICK      ) 10,
				               (OS_OPT       ) OS_OPT_PEND_BLOCKING,
				               (OS_MSG_SIZE* ) &msg_size, 
				               (CPU_TS*      ) 0,
				               (OS_ERR*      ) &err);


		       if (pmsg != NULL)
		       {  
            
                   /* 发送调试信息给服务端 */
                   netconn_write(conn, pmsg, msg_size, NETCONN_NOCOPY ); 
                   
#ifdef DEBUG_USART /* 串口输出调试信息 */
                   
                   USART_DEBUG("%s", pmsg); 
#endif                   

                   /* 归还内存 */
                   OSMemPut((OS_MEM  *)&gnet_debug_mem, pmsg, &os_err);
                   pmsg = NULL;
                   
#ifdef DEBUG_USART /* 串口输出调试信息 */  
                   /* 消息队列剩余容量 */
                   u16_t msgq_remain_size; 
                   msgq_remain_size = gdebug_msg.MsgQ.NbrEntriesSize - gdebug_msg.MsgQ.NbrEntries;
                   USART_DEBUG("msgq_remain_size = %d", msgq_remain_size);
#endif 
                   
                   
                   /* post调试信号量 */
                   debug_sem = OSSemPost ((OS_SEM* ) &gnet_debug_sem,
                                          (OS_OPT  ) OS_OPT_POST_1,
                                          (OS_ERR *) &err);
                   
#ifdef DEBUG_USART /* 串口输出调试信息 */                    
                   /* 最新的信号值 */
                   USART_DEBUG("debug_sem = %d", debug_sem);
#endif                  
		       }
               

#ifdef TODO                          
				/* 接收到服务端发送的数据 */	
				if ((recv_err = netconn_recv(conn,&recvbuf)) == ERR_OK)  
				{	
                    /* 将接收到的数据原样返还给服务器 */
                    do
                    {
                        netbuf_data( recvbuf, &data, &data_len );
                        netconn_write(conn, data, data_len, NETCONN_NOCOPY ); 
                    }while( netbuf_next( recvbuf ) >= 0 );
                    netbuf_delete( recvbuf );
				}
                /* 关闭连接 */
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
               /* 任务挂起100ms */
               OSTimeDlyHMSM(0, 0,0,50, OS_OPT_TIME_HMSM_STRICT,&os_err);
			}
		}

	}
}