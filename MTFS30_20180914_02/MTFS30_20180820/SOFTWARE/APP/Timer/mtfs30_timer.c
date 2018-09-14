/*****************************************************************************/
/* �ļ���:    mtfs30_timer.c                                                 */
/* ��  ��:    ��ʱ��������                                                 */
/* ��  ��:    2018-07-20 changzehai(DTT)                                     */
/* ��  ��:    ��                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include "mtfs30_timer.h"
#include "os.h"
#include "mtfs30_debug.h"
#include "mtfs30.h"






/*-------------------------------*/
/* ��������                      */
/*-------------------------------*/
static  OS_TCB   AppTaskTmrTCB;
static  CPU_STK  AppTaskTmrStk [ APP_TASK_TMR_STK_SIZE ];
CPU_TS             ts_start;       //ʱ�������
CPU_TS             ts_end; 

extern sate_status_t    gsate_status;
OS_TMR      my_tmr;   //���������ʱ������

static  void  AppTaskTmr  ( void * p_arg );


/*****************************************************************************
 * ��  ��:    mtfs30_timer_task                                                           
 * ��  ��:    ������ʱ������                                                                
 * ��  ��:    str     : Ҫ����У������ַ���             
              len     : �ַ����ĳ���
 * ��  ��:    ��                                                    
 * ����ֵ:    �µ�CRC32У����                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/
void mtfs30_timer_task(void)
{
    OS_ERR      err;    
    
		/* ���� AppTaskTmr ���� */
    OSTaskCreate((OS_TCB     *)&AppTaskTmrTCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Tmr",                             //��������
                 (OS_TASK_PTR ) AppTaskTmr,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_TMR_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppTaskTmrStk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK    *)&AppTaskTmrStk[APP_TASK_TMR_STK_SIZE / 10],                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (OS_STK_SIZE) APP_TASK_TMR_STK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������

}

void TmrCallback (OS_TMR *p_tmr, void *p_arg) //�����ʱ��MyTmr�Ļص�����
{
//	CPU_INT32U       cpu_clk_freq;	
//	CPU_SR_ALLOC();      //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú������Ͷ���һ���ֲ���
											 //�������ڱ�����ж�ǰ�� CPU ״̬�Ĵ��� SR���ٽ�ι��ж�ֻ�豣��SR��
											 //�����ж�ʱ����ֵ��ԭ��  
	
//	cpu_clk_freq = BSP_CPU_ClkFreq();                   //��ȡCPUʱ�ӣ�ʱ������Ը�ʱ�Ӽ���
	

	
//  ts_end = OS_TS_GET() - ts_start;     //��ȡ��ʱ���ʱ�������CPUʱ�ӽ��м�����һ������ֵ��
	                                     //�������㶨ʱʱ�䡣
//	OS_CRITICAL_ENTER();                 //�����ٽ�Σ���ϣ�����洮�ڴ�ӡ�⵽�ж�
	
//#if MTFS30_DEBUG_EN
//              MTFS30_DEBUG("1234567890!\n");
////            MTFS30_DEBUG("��ʱ %d us���� %d ms\n", ts_end / ( cpu_clk_freq / 1000000 ),ts_end / ( cpu_clk_freq / 1000 ));     
//#endif      
	
//	OS_CRITICAL_EXIT();                               

//	ts_start = OS_TS_GET();                            //��ȡ��ʱǰʱ���
	
    gsate_status.status_msg[0] = '\0';
    gsate_status.msg_len = 0;

}


static  void  AppTaskTmr ( void * p_arg )
{
	OS_ERR      err;


	
	(void)p_arg;


  /* ���������ʱ�� */
  OSTmrCreate ((OS_TMR              *)&my_tmr,             //�����ʱ������
               (CPU_CHAR            *)"MySoftTimer",       //���������ʱ��
               (OS_TICK              )0,                  //��ʱ����ʼֵ����10Hzʱ�����㣬��Ϊ1s
               (OS_TICK              )5,                  //��ʱ����������ֵ����10Hzʱ�����㣬��Ϊ1s
               (OS_OPT               )OS_OPT_TMR_PERIODIC, //�����Զ�ʱ
               (OS_TMR_CALLBACK_PTR  )TmrCallback,         //�ص�����
               (void                *)"Timer Over!",       //����ʵ�θ��ص�����
               (OS_ERR              *)err);                //���ش�������
							 
	/* ���������ʱ�� */						 
  OSTmrStart ((OS_TMR   *)&my_tmr, //�����ʱ������
              (OS_ERR   *)err);    //���ش�������
					 
//	ts_start = OS_TS_GET();                       //��ȡ��ʱǰʱ���
							 
//	while (DEF_TRUE) {                            //�����壬ͨ��д��һ����ѭ��
//
//		OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err ); //��������������
//        //BSP_Sleep(1000);
//	}
  
    OSTaskDel((OS_TCB*)0,&err);
	
}










