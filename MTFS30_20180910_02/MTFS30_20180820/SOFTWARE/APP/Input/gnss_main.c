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
#include "mtfs30_debug.h"
#include "util.h"
#include "dev.h"




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
extern ringbuffer_t pgusart1_rb;                  /* ����1��Ϣ������ */

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
////                    /* ����������Ϣ */  
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
    u16_t read_size = 0; /* ��Index���ַ�"\n"֮������ݴ�С */
    u16_t read_byte = 0; /* ���λ��������������ݴ�С        */
    u8_t read_buf[256];  /* ��Ŵӻ��λ���������������      */
    u8_t *pmsg = NULL;   /* ָ���λ�������ͷλ��          */
    
    
	pdata = pdata;
    
    
    /*-------------------------------------*/
    /* �����Index���ַ�"\n"֮������ݴ�С */
    /*-------------------------------------*/
	while(1)
	{
      
        read_size = 0;
        i = pgusart1_rb.rb_read;    /* ��index        */
        pmsg = pgusart1_rb.prb_buf; /* ���λ�����ָ�� */
          
        while(1)
        {
            
            if (pmsg[i] == '\n')
            {
                read_size++;
                break;
            }
            
            read_size++;
            
            i++;
            /* �ﵽ������ĩβ�����Ŵӻ�������ͷ���� */
            i = i % pgusart1_rb.rb_size;

        }
       
          
        memset(read_buf, 0x00, sizeof(read_buf));
        /* ��������index���ַ�"\n"��֮������� */
        read_byte =  util_ringbuffer_read(&pgusart1_rb, read_buf, read_size);
        if (read_byte > 0)
        {
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("read_size = %d, read_byte = %d, read_buf: %s\n", read_size, read_byte, read_buf);          
#endif                
             /* ����������Ϣ */           
            gnss_receiver_msg_parser(read_buf);
        }

        
        BSP_Sleep(10);
        
	}
   
}



