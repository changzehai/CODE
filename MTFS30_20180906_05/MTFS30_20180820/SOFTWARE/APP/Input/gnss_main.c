/*****************************************************************************/
/* �ļ���:    gnss_main.c                                                    */
/* ��  ��:    ����ģ��������                                                 */
/* ��  ��:    2018-07-20 changzehai(DTT)                                     */
/* ��  ��:    ��                                                             */
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




/* ������ƿ� */
static OS_TCB gtask_gnss_tcb;  /* ������Ϣ��ѯ������ƿ� */


/* ����ջ */
static CPU_STK    gtask_gnss_stk[GNSS_TASK_STACK_SIZE];  /* ������Ϣ��ѯ����ջ */




/*-------------------------------*/
/* ȫ�ֱ�������                  */
/*-------------------------------*/
extern u8 gusart1_rx_buf[USART1_BUF_SIZE]; /* ����1���ջ����� */
extern OS_SEM  gusart1_sem;     /* ����1���ź���     */
extern OS_Q    gusart1_msg;        /* ����1��Ϣ����     */
extern OS_MEM  gusart1_mem; /* ����1���ڴ���ƿ� */
/*-------------------------------*/
/* ��������                      */
/*-------------------------------*/
static void gnss_task(void *pdata);

/*****************************************************************************
 * ��  ��:    gnss_main_task                                                           
 * ��  ��:    �������ǲ�ѯ����                                                                 
 * ��  ��:    ��                          
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/ 
void gnss_main_task(void)
{
    OS_ERR err;


    /* ����������Ϣ��ѯ���� */
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
 * ��  ��:    gnss_task                                                           
 * ��  ��:    ������Ϣ��������                                                               
 * ��  ��:    ��                          
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/  
static void gnss_task(void *pdata)
{
    u8_t *pmsg = NULL;
	OS_ERR  err;
	OS_MSG_SIZE msg_size;

	pdata = pdata;

	while(1)
	{
       
		/* ����Ϣ������ȡ��GNSSģ�������������Ϣ */
		pmsg = OSQPend((OS_Q*        )&gusart1_msg,   
				(OS_TICK      )0,
				(OS_OPT       )OS_OPT_PEND_BLOCKING,
				(OS_MSG_SIZE* )&msg_size, 
				(CPU_TS*      )0,
				(OS_ERR*      )&err);

		if (pmsg != NULL)
		{  
                       
			/* ����������Ϣ */
			gnss_receiver_msg_parser(pmsg);
            
            /* �黹��ȡ�����ڴ�� */
           OSMemPut((OS_MEM  *)&gusart1_mem, pmsg, &err);            

		}
        BSP_Sleep(100);
        
	}
   
}



