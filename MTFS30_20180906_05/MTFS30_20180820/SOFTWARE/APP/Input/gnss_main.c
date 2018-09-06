/*****************************************************************************/
/* 文件名:    gnss_main.c                                                    */
/* 描  述:    卫星模块主程序                                                 */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include <includes.h>
#include "arch/cc.h"
#include "lwip/api.h"
#include "Config/config.h"
#include "opt.h"
#include "gnss_main.h"
#include "gnss_receiver.h"
#include "sys_usart.h"
#include "mtfs30_debug.h"




/* 任务控制块 */
static OS_TCB gtask_gnss_tcb;  /* 卫星信息查询任务控制块 */


/* 任务栈 */
static CPU_STK    gtask_gnss_stk[GNSS_TASK_STACK_SIZE];  /* 卫星信息查询任务栈 */




/*-------------------------------*/
/* 全局变量声明                  */
/*-------------------------------*/
extern u8 gusart1_rx_buf[USART1_BUF_SIZE]; /* 串口1接收缓冲区 */
extern OS_SEM  gusart1_sem;     /* 串口1用信号量     */
extern OS_Q    gusart1_msg;        /* 串口1消息队列     */
extern OS_MEM  gusart1_mem; /* 串口1用内存控制块 */
/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
static void gnss_task(void *pdata);

/*****************************************************************************
 * 函  数:    gnss_main_task                                                           
 * 功  能:    创建卫星查询任务                                                                 
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/ 
void gnss_main_task(void)
{
    OS_ERR err;


    /* 创建卫星信息查询任务 */
    OSTaskCreate( (OS_TCB        *)	&gtask_gnss_tcb,
				  (CPU_CHAR      *)	"gnss task",
				  (OS_TASK_PTR    )	gnss_task,
				  (void          *) 0,
				  (OS_PRIO        )	GNSS_THREAD_PRIO,
				  (CPU_STK       *)	&gtask_gnss_stk[0],
                  (CPU_STK       *) &gtask_gnss_stk[GNSS_TASK_STACK_SIZE / 10],
				  (OS_STK_SIZE    )	GNSS_TASK_STACK_SIZE,
				  (OS_MSG_QTY     ) 0,
				  (OS_TICK        ) 0,
				  (void          *) 0,
				  (OS_OPT         ) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				  (OS_ERR        *) &err);		

}


/*****************************************************************************
 * 函  数:    gnss_task                                                           
 * 功  能:    卫星信息解析任务                                                               
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/  
static void gnss_task(void *pdata)
{
    u8_t *pmsg = NULL;
	OS_ERR  err;
	OS_MSG_SIZE msg_size;

	pdata = pdata;

	while(1)
	{
       
		/* 从消息队列中取出GNSS模块输出的卫星信息 */
		pmsg = OSQPend((OS_Q*        )&gusart1_msg,   
				(OS_TICK      )0,
				(OS_OPT       )OS_OPT_PEND_BLOCKING,
				(OS_MSG_SIZE* )&msg_size, 
				(CPU_TS*      )0,
				(OS_ERR*      )&err);

		if (pmsg != NULL)
		{  
                       
			/* 解析卫星信息 */
			gnss_receiver_msg_parser(pmsg);
            
            /* 归还获取到的内存块 */
           OSMemPut((OS_MEM  *)&gusart1_mem, pmsg, &err);            

		}
        BSP_Sleep(100);
        
	}
   
}



