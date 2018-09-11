#include <includes.h>
#include "arch/cc.h"
#include "lwip/api.h"
#include "Sensor\dht11.h"
#include "Sensor\serial.h"
#define  INTER_COUNT  5
//
//! OS_TCB  control defintion.
//
static   OS_TCB  SensorTaskTCB;
//
//! control task stack.
//
static   CPU_STK SensorTaskStk[SENSOR_TASK_STK_SIZE];
static   unsigned char sensor_count;
void Sensor_Init(void)
{
  serial_init();
  DHT11_GPIO_Config();
}
void Sensor_main(void)
{
  OS_ERR err;
  Sensor_Init();
  while(1)
  {
    if(sensor_count < INTER_COUNT)
       sensor_count++;
    else
    {
       Read_DHT11(); 
       sensor_count = 0;
    }
    OSTimeDlyHMSM(0, 0,0, 100, OS_OPT_TIME_HMSM_STRICT, &err);
  }
}
//
//! create main control task.
//
void Main_Sensor_Task(void)
{
    OS_ERR err;
   //sys_thread_new("HTTP", Web_Thread, NULL, DEFAULT_THREAD_STACKSIZE, WEBSERVER_THREAD_PRIO);
   OSTaskCreate( (OS_TCB        *)	&SensorTaskTCB,
				  (CPU_CHAR      *)	"task sensor",
				  (OS_TASK_PTR    )	 Sensor_main,
				  (void          *) 0,
				  (OS_PRIO        )	SENSOR_TASK_PRIO,
				  (CPU_STK       *)	&SensorTaskStk[0],
				  (CPU_STK   *)	        &SensorTaskStk[SENSOR_TASK_STK_SIZE / 10u],
				  (OS_STK_SIZE   )	SENSOR_TASK_STK_SIZE,
				  (OS_MSG_QTY     ) 0,
				  (OS_TICK        ) 0,
				  (void          *) 0,
				  (OS_OPT         ) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				  (OS_ERR        *) &err);
}
