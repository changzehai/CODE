#include <includes.h>
#include "arch/cc.h"
#include "Extent INT.h"
//
//!EXTI INPUT PIN  == GPIO_Pin_5
#define PULSE_IN_PIN      GPIO_Pin_5
#define PULSE_EXTI_CLOCK  RCC_AHB1Periph_GPIOB

static  const u16_t PULSE_INPUT_PIN =
{
   PULSE_IN_PIN
};

//
//!Define EXTI INPUT PORT.
GPIO_TypeDef* ulMap_PulseIn_Port =
{
   GPIOB,
};

//
//!Initial the GPIOB5 AS input setting. 
//
static void Extent_Int5IO_Initial(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;                 //����ṹ��
   
   
   RCC_AHB1PeriphClockCmd(PULSE_EXTI_CLOCK, ENABLE);//ʹ��ʱ��
   GPIO_InitStructure.GPIO_Pin  = PULSE_INPUT_PIN;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;         //���ó���������
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
   GPIO_Init(ulMap_PulseIn_Port, &GPIO_InitStructure);  
}

//
//!SET EXTI9_5 Priority as 1:0
//
static void NVIC_EXTI_Initial(void)
{
   NVIC_InitTypeDef NVIC_InitStructure;                 //����ṹ��
   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
   NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn; //ʹ���ⲿ�ж����ڵ�ͨ��
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01; //��ռ���ȼ� 2�� 
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00; //�����ȼ� 2
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;   //ʹ���ⲿ�ж�ͨ�� 
   NVIC_Init(&NVIC_InitStructure);                   //���ݽṹ����Ϣ�������ȼ���ʼ��
}

//
//!EXTI SOURCE SET.
//
static void EXTI_SOURCE_Initial(void)
{
   EXTI_InitTypeDef EXTI_InitStructure;                 //�����ʼ���ṹ��
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
   
   SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB,GPIO_PinSource5);
   EXTI_DeInit();
   EXTI_InitStructure.EXTI_Line=EXTI_Line5;               //�ж��ߵı�� ȡֵ��ΧΪEXTI_Line0~EXTI_Line15
   EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    //�ж�ģʽ����ѡֵΪ�ж� EXTI_Mode_Interrupt ���¼� EXTI_Mode_Event��
   EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//������ʽ���������½��ش��� EXTI_Trigger_Falling�������ش��� EXTI_Trigger_Rising�����������ƽ�������غ��½��أ�����EXTI_Trigger_Rising_Falling
   EXTI_InitStructure.EXTI_LineCmd = ENABLE;
   EXTI_Init(&EXTI_InitStructure);                        //���ݽṹ����Ϣ���г�ʼ��
   EXTI_ClearITPendingBit(EXTI_Line5);
}
static void App_IntHandlerEXTI9_5(void)
{
   OS_ERR  err;
   if(EXTI_GetFlagStatus(EXTI_Line5)==SET)
   {
     EXTI_ClearITPendingBit(EXTI_Line5);               /* Clear the ext5 Update interrupt                    */        
        
     //OSTaskSemPost(&AppTask_EXTI15_10_TCB, OS_OPT_POST_NO_SCHED, &err);
   }        
}
void Extent_Int5_Set(void)
{
  Extent_Int5IO_Initial();                            //initial PB5 as input.
  NVIC_EXTI_Initial();                                //initial EXTI_NVIC.  
  EXTI_SOURCE_Initial();                              //initial Set
  BSP_IntVectSet(BSP_INT_ID_EXTI9_5,App_IntHandlerEXTI9_5);
  BSP_IntPrioSet(BSP_INT_ID_EXTI9_5,2);    
  BSP_IntEn(BSP_INT_ID_EXTI9_5); 
}