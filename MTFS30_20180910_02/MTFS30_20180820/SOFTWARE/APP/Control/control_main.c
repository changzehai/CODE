
#include <includes.h>
#include "arch/cc.h"
#include "lwip/api.h"
#include "config.h"
#include "Control/PCF8563.h"
#include "Control/control.h"
//
//! OS_TCB  control defintion.
//
static   OS_TCB  ControlTaskTCB;
//
//! control task stack.
//
static   CPU_STK ControlTaskStk[CTR_TASK_START_STK_SIZE];
//
//! extern config.c http ssi parameters.
//
extern tTimeParameters  CurrentTime;
//
//! define the local rtc time .
//
 RTC_T    PCF8563RTC_TIME;            //rtc time

//
//! AC parameters :voltage ,leakage current ,and load current.
//

u16_t system_status;
/*IWDOG INIT*/
static void DogInit(void)
{

	RCC_LSICmd(ENABLE);
	while(RCC_GetFlagStatus( RCC_FLAG_LSIRDY)==RESET);
	
	/* Enable write access to IWDG_PR and IWDG_RLR registers */
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	
	/* IWDG counter clock: 40KHz(LSI) / 32 = 1.25 KHz */  // /64 =625
  	IWDG_SetPrescaler(IWDG_Prescaler_256);//IWDG_Prescaler_64

 	/* Set counter reload value to 3 (3+1)*1/1.25=5ms,¨°????¨²??1ms *//*625 1s*/
  	IWDG_SetReload(625);//625

	/* Reload IWDG counter */
	IWDG_ReloadCounter();

	/* Enable IWDG (the LSI oscillator will be enabled by hardware) */
	IWDG_Enable();
};

/*Feed DOG*/
static void FeedDog(void)
{
	IWDG_ReloadCounter();
};
//
//! get rtc time and transmite to http ssi.
//
static void Pre_Sec_Feeddog(void)
{
     FeedDog();
     //
    // sys_timeout get_time per 1seconds.
    //
    sys_timeout(100000,Pre_Sec_Feeddog,NULL);
}
static void rtc_pcf8563_get(void)
{
   //
   //
   //
   
    sys_untimeout(rtc_pcf8563_get, NULL);
   //rtc time get.
    pcf8563_GetDateTime(&PCF8563RTC_TIME);
   // run time recored  
   g_sParameters.RunSecond++;
       
    CurrentTime.year=PCF8563RTC_TIME.year;
    CurrentTime.month=PCF8563RTC_TIME.month;
    CurrentTime.day=PCF8563RTC_TIME.day;
    CurrentTime.hour=PCF8563RTC_TIME.hour;
    CurrentTime.minute=PCF8563RTC_TIME.minute;
    if(PCF8563RTC_TIME.second>59)
    CurrentTime.second=59;
    else
    CurrentTime.second=PCF8563RTC_TIME.second;
   
    //
    // sys_timeout get_time per 1seconds.
    //
    sys_timeout(100000,rtc_pcf8563_get,NULL);
    
}
//
//!main control init.
//
static void Main_Control_Init(void)
{
   //
  //! PCF8563 Io initilize.
  //
  pcf8563_InitI2c();
  //
  //! control Io initiliztion.
  ControlLed_Io_Init();
  //
  //! the power on check the L N EARTH IS ODER.
  Line_Status_Check();
  //
  // sys_timeout get_time per 1seconds.
  //
  sys_timeout(100000,rtc_pcf8563_get,NULL);
  
  DogInit();
  sys_timeout(100000,Pre_Sec_Feeddog,NULL);
}
//
//! main handle
//
extern void SnmpDataRefresh(void);
static void Main_Hanlder(void)
{
     //
     //!get rtc time .
     //
     //rtc_pcf8563_get();g_sParameters.RunSecond
     //
     //
     //!main local control
       Loop_Line_Check();
     //
     //!snmp data refresh
     //
     // SnmpDataRefresh();
     //
     //! led indicated.
       Led_indicate();
     
     //  FeedDog();
}
//
//! main control handler.
//

void main_control(void)
{ 
  OS_ERR err;
  Main_Control_Init();
  while(1)
  {  
     Main_Hanlder();
     OSTimeDlyHMSM(0, 0,0, 100, OS_OPT_TIME_HMSM_STRICT, &err); 
  }
}
//
//! create main control task.
//
void Main_Ctr_Task(void)
{
    OS_ERR err;
   //sys_thread_new("HTTP", Web_Thread, NULL, DEFAULT_THREAD_STACKSIZE, WEBSERVER_THREAD_PRIO);
   OSTaskCreate( (OS_TCB        *)	&ControlTaskTCB,
				  (CPU_CHAR      *)	"task control",
				  (OS_TASK_PTR    )	 main_control,
				  (void          *) 0,
				  (OS_PRIO        )	CTR_TASK_START_PRIO,
				  (CPU_STK       *)	&ControlTaskStk[0],
				  (CPU_STK   *)	        &ControlTaskStk[CTR_TASK_START_STK_SIZE / 10u],
				  (OS_STK_SIZE   )	CTR_TASK_START_STK_SIZE,
				  (OS_MSG_QTY     ) 0,
				  (OS_TICK        ) 0,
				  (void          *) 0,
				  (OS_OPT         ) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				  (OS_ERR        *) &err);
}