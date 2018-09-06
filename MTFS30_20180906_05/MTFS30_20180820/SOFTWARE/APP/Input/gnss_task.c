/*****************************************************************************/
/* 文件名:    gnss_task.c                                                    */
/* 描  述:    GNSS子任务                                                     */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include "mtfs30_com.h"
#include "gnss_receiver.h"




/*****************************************************************************
 * 函  数:    gnss_task                                                           
 * 功  能:    GNSS子任务(卫星信息解析任务)                                                                 
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/  
void gnss_task(void *pdata)
{
	 INT8U *pmsg = NULL;
	 INT8U err;
	 
	 pdata = pdata;
	 
	 while(1)
	 {
		 /* 从消息队列中取出GNSS模块输出的卫星信息 */
#ifdef TODO
		 pmsg = (INT8U *)OSQPend(gpUSART1_MsgQeue, 0, &err); 
#endif
		 
		 if (pmsg != NULL)
		 {
			 /* 解析卫星信息 */
			 gnss_receiver_msg_parser(pmsg);

		 }
		  
	 }
	 	 
}

