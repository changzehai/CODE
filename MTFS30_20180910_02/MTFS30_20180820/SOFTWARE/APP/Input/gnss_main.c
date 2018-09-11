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
#include "mtfs30_debug.h"
#include "util.h"
#include "dev.h"




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
extern ringbuffer_t pgusart1_rb;                  /* 串口1消息缓冲区 */

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
//static void gnss_task(void *pdata)
//{
//      u8_t *pmsg = NULL;
//	OS_ERR  err;
//	//OS_MSG_SIZE msg_size;
//    u16_t read_byte;
//    u16_t len = 0;;
//    u8_t buf[256];
//    u8_t flag = 0;
//    u8_t ch;  
//    
//        while(1)
//        {
//            
////            ch = util_ringbuffer_get(&pgusart1_rb);
//////#if MTFS30_DEBUG_EN
//////            MTFS30_DEBUG("ch: %c\n", ch);          
//////#endif               
////            if (ch == 0)
////            {
////                continue;
////            }
////            
////            if (ch == '$' || ch == '#')
////            {
////                flag = 1;
////            }
////            
////            if (flag == 1)
////            {
////                buf[len] = ch;
////                len++;
////                if (ch == '\n')
////                {
////                    buf[len] = '\0';
//////#if MTFS30_DEBUG_EN
//////            MTFS30_DEBUG("buf: %s\n",buf);          
//////#endif                        
////                    /* 解析卫星信息 */  
//////                    gnss_receiver_msg_parser(buf);
////                    
////                    
////                    len = 0;
////                    flag = 0;
////                    memset(buf, 0x00, sizeof(buf));
////                }
////            }
//
//            BSP_Sleep(10); 
//        }
//
//}

static void gnss_task(void *pdata)
{

    u16_t i = 0; 
	OS_ERR  err;
    u16_t read_size = 0; /* 读Index到字符"\n"之间的数据大小 */
    u16_t read_byte = 0; /* 从形缓冲区读出的数据大小        */
    u8_t read_buf[256];  /* 存放从环形缓冲区读出的数据      */
    u8_t *pmsg = NULL;   /* 指向环形缓冲区开头位置          */
    
    
	pdata = pdata;
    
    
    /*-------------------------------------*/
    /* 计算读Index到字符"\n"之间的数据大小 */
    /*-------------------------------------*/
	while(1)
	{
      
        read_size = 0;
        i = pgusart1_rb.rb_read;    /* 读index        */
        pmsg = pgusart1_rb.prb_buf; /* 环形缓冲区指针 */
          
        while(1)
        {
            
            if (pmsg[i] == '\n')
            {
                read_size++;
                break;
            }
            
            read_size++;
            
            i++;
            /* 达到缓冲区末尾，接着从缓冲区开头计数 */
            i = i % pgusart1_rb.rb_size;

        }
       
          
        memset(read_buf, 0x00, sizeof(read_buf));
        /* 读出【读index到字符"\n"】之间的数据 */
        read_byte =  util_ringbuffer_read(&pgusart1_rb, read_buf, read_size);
        if (read_byte > 0)
        {
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("read_size = %d, read_byte = %d, read_buf: %s\n", read_size, read_byte, read_buf);          
#endif                
             /* 解析卫星信息 */           
            gnss_receiver_msg_parser(read_buf);
        }

        
        BSP_Sleep(10);
        
	}
   
}



