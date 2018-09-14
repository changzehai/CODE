/*****************************************************************************/
/* 文件名:    mtfs30_timer.c                                                 */
/* 描  述:    定时器处理处理                                                 */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include "mtfs30_timer.h"
#include "os.h"
#include "mtfs30_debug.h"
#include "mtfs30.h"






/*-------------------------------*/
/* 变量定义                      */
/*-------------------------------*/
static  OS_TCB   AppTaskTmrTCB;
static  CPU_STK  AppTaskTmrStk [ APP_TASK_TMR_STK_SIZE ];
CPU_TS             ts_start;       //时间戳变量
CPU_TS             ts_end; 

extern sate_status_t    gsate_status;
OS_TMR      my_tmr;   //声明软件定时器对象

static  void  AppTaskTmr  ( void * p_arg );


/*****************************************************************************
 * 函  数:    mtfs30_timer_task                                                           
 * 功  能:    创建定时器任务                                                                
 * 输  入:    str     : 要计算校验码的字符串             
              len     : 字符串的长度
 * 输  出:    无                                                    
 * 返回值:    新的CRC32校验码                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void mtfs30_timer_task(void)
{
    OS_ERR      err;    
    
		/* 创建 AppTaskTmr 任务 */
    OSTaskCreate((OS_TCB     *)&AppTaskTmrTCB,                             //任务控制块地址
                 (CPU_CHAR   *)"App Task Tmr",                             //任务名称
                 (OS_TASK_PTR ) AppTaskTmr,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_TMR_PRIO,                         //任务的优先级
                 (CPU_STK    *)&AppTaskTmrStk[0],                          //任务堆栈的基地址
                 (CPU_STK    *)&AppTaskTmrStk[APP_TASK_TMR_STK_SIZE / 10],                //任务堆栈空间剩下1/10时限制其增长
                 (OS_STK_SIZE) APP_TASK_TMR_STK_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型

}

void TmrCallback (OS_TMR *p_tmr, void *p_arg) //软件定时器MyTmr的回调函数
{
//	CPU_INT32U       cpu_clk_freq;	
//	CPU_SR_ALLOC();      //使用到临界段（在关/开中断时）时必需该宏，该宏声明和定义一个局部变
											 //量，用于保存关中断前的 CPU 状态寄存器 SR（临界段关中断只需保存SR）
											 //，开中断时将该值还原。  
	
//	cpu_clk_freq = BSP_CPU_ClkFreq();                   //获取CPU时钟，时间戳是以该时钟计数
	

	
//  ts_end = OS_TS_GET() - ts_start;     //获取定时后的时间戳（以CPU时钟进行计数的一个计数值）
	                                     //，并计算定时时间。
//	OS_CRITICAL_ENTER();                 //进入临界段，不希望下面串口打印遭到中断
	
//#if MTFS30_DEBUG_EN
//              MTFS30_DEBUG("1234567890!\n");
////            MTFS30_DEBUG("定时 %d us，即 %d ms\n", ts_end / ( cpu_clk_freq / 1000000 ),ts_end / ( cpu_clk_freq / 1000 ));     
//#endif      
	
//	OS_CRITICAL_EXIT();                               

//	ts_start = OS_TS_GET();                            //获取定时前时间戳
	
    gsate_status.status_msg[0] = '\0';
    gsate_status.msg_len = 0;

}


static  void  AppTaskTmr ( void * p_arg )
{
	OS_ERR      err;


	
	(void)p_arg;


  /* 创建软件定时器 */
  OSTmrCreate ((OS_TMR              *)&my_tmr,             //软件定时器对象
               (CPU_CHAR            *)"MySoftTimer",       //命名软件定时器
               (OS_TICK              )0,                  //定时器初始值，依10Hz时基计算，即为1s
               (OS_TICK              )5,                  //定时器周期重载值，依10Hz时基计算，即为1s
               (OS_OPT               )OS_OPT_TMR_PERIODIC, //周期性定时
               (OS_TMR_CALLBACK_PTR  )TmrCallback,         //回调函数
               (void                *)"Timer Over!",       //传递实参给回调函数
               (OS_ERR              *)err);                //返回错误类型
							 
	/* 启动软件定时器 */						 
  OSTmrStart ((OS_TMR   *)&my_tmr, //软件定时器对象
              (OS_ERR   *)err);    //返回错误类型
					 
//	ts_start = OS_TS_GET();                       //获取定时前时间戳
							 
//	while (DEF_TRUE) {                            //任务体，通常写成一个死循环
//
//		OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err ); //不断阻塞该任务
//        //BSP_Sleep(1000);
//	}
  
    OSTaskDel((OS_TCB*)0,&err);
	
}










