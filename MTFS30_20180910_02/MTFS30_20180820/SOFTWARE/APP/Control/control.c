
#include <includes.h>
#include "arch/cc.h"
#include "config.h"
#include "control.h"
#include "ATT7053.h"
//
//! max led indicate alram enum.
//
#define MAX_ALRMLED_ENUM   11
#define MAX_CHANNEL_NUM    4
#define FLASH_ALARM_BIT    0x62
#define CHOUPT_START       7


#define KEY_TYPE           1
#define OPT_TYPE           0

#define LEAKKEY_DELAY      10


//
//! system alarm flag: 
//_INDEX_UNEARTH: 
//INDEX_LNORDER:
//INDEX_LPROFAIL:
//INDEX_OVCURRA:
//INDEX_OVVOLA:
//INDEX_UVLOA:
//INDEX_LEAKCA:
//INDEX_INVCH1:
//INDEX_INVCH2:
//INDEX_INVCH3:
//INDEX_INVCH4:
extern unsigned short SysAlarm_Flag;
//*****************************************************************************
//
//! This structure instance contains the run-time set of configuration
//! parameters for S2E module.  This is the active parameter set and may
//! contain changes that are not to be committed to flash.
//
//*****************************************************************************
extern tConfigParameters g_sParameters;
//
//! extern config.c http ssi parameters.
//
extern tTimeParameters  CurrentTime;
/*Channel Line on-off status*/
extern unsigned char  ChannelStatus;
/* 7 discrete var */
extern s16_t discreteValue[8];
//! analog value.
extern u16_t analogValue[3];  //0: AC VOL 1:LEAKAGE CURRENT. 2: LOAD CURRENT.

//! sys parameters Irm1offset Irm2offset VK Lk Ik
extern tSysCALParameters g_sSyscalparaters;
extern tSysCALParameters g_FactorySyscalparameters;
//! regate_timeout_cnt
static u8_t Regate_Timeout_CNT =0;
//! test overcurrent key flag.
tBoolean Testing_Leakage = false;
//! test overcurrent cnt.
static u8_t Test_Leakage_Cnt = 0;
//! serial lamp status.
extern uint8_t lamp_status;

//static u8_t Regate_Start = false;
////
////!ch1 -ch4 output, earth,lineorder,lingtprotect.
//extern u16_t system_status;
//
//! Light corresponding to the alarm BIT bit.
static const u16_t Alarm_Bits[MAX_ALRMLED_ENUM] = {
  0x01,
  0x02,
  0x04,
  0x08,
  0x10,
  0x20,
  0x40,
  0x80,
  0x100,
  0x200,
  0x400
};
//
//! alarm corresponing to led pin.
static const u16_t ulMap_Alarm_LED_PIN[MAX_ALRMLED_ENUM] = 
{ 
  REVERSE_LED_PIN,
  REVERSE_LED_PIN,
  LIGHTPRO_LED_PIN,
  LEAKAGE_LED_PIN,
  VOL_LED_PIN,
  VOL_LED_PIN,
  LEAKAGE_LED_PIN,
  CH1O_LED_PIN,
  CH2O_LED_PIN,
  CH3O_LED_PIN,
  CH4O_LED_PIN
};
//
//! alrm corresponing  to led port.
 GPIO_TypeDef*  ulMap_Alarm_LED_PORT[MAX_ALRMLED_ENUM] =
 { 
   LED_PORTA,
   LED_PORTA,
   LED_PORTD,
   LED_PORTD,
   LED_PORTD,
   LED_PORTD,
   LED_PORTD,
   LED_PORTD,
   LED_PORTD,
   LED_PORTD,
   LED_PORTA
 };
//
//! relay control to pin.
static const u16_t ulMap_CTR_Relay_PIN[MAX_CHANNEL_NUM] =
{
   CH1_RELAY_PIN,
   CH2_RELAY_PIN,
   CH3_RELAY_PIN,
   CH4_RELAY_PIN
};
//
//! relay channel Optical check pin.
static const u16_t ulMap_CHStatus_Check_PIN[MAX_CHANNEL_NUM] =
{
  CH1_NOR_PIN,
  CH2_NOR_PIN,
  CH3_NOR_PIN,
  CH4_NOR_PIN
};
//
//! relay channel optical check port.
GPIO_TypeDef* ulMap_CHStatus_Check_PORT[MAX_CHANNEL_NUM] =
{
  GPIOD,
  GPIOD,
  GPIOD,
  GPIOC
};
//
//! 3 KEY and 2 optical pin.
//
static const u16_t ulMap_KeyOrOpt_PIN[MAX_KEYOROPT_NUM] =
{
  OVERLOAD_KEY_PIN,
  LEAKAGE_KEY_PIN,
  RESTORE_KEY_PIN,
  LORDER_CHECK_PIN,
  NORDER_CHECK_PIN
};
//
//! 3 key and 2 optical port.
//
GPIO_TypeDef* ulMap_KeyOrOpt_PORT[MAX_KEYOROPT_NUM] =
{
  GPIOE,
  GPIOB,
  GPIOB,
  GPIOE,
  GPIOE
};
//! old CH1OUPT Status.
static u8_t guChannel_Status = 0;
static void RELAY_CTR(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, u8_t State);
/*****************************************************************************************************/
/*LED ON OFF:GPIOx as IO port,GPIO_Pin as IO pin,LedState as LED_ON/LED_OFF                          */
/*****************************************************************************************************/
static void LED_ON_OFF(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, u8_t LedState)
{
     //!if led on ,pin od seted as close to ground.
     if(LedState != OFF)
     {
         GPIOx->BSRRH = GPIO_Pin;
         
     }
     else
     {
         GPIOx->BSRRL = GPIO_Pin;
     }
}
/*****************************************************************************************************/
/*                                  ALL_LED_CLEAR : no parameters                                    */
/*****************************************************************************************************/
static void ALL_LED_CLEAR(void)
{
    //! all led should clear to off.
    uint16_t ALL_GPIO_Pin =  0;
    //! all pin bit . 
    ALL_GPIO_Pin = ACIN_LED_PIN | WORK_LED_PIN | CH4O_LED_PIN | REVERSE_LED_PIN;
    //! write command to pin.
    GPIO_WriteBit(LED_PORTA,ALL_GPIO_Pin,Bit_SET);
    //! all pin bit .
    ALL_GPIO_Pin = 0;
    ALL_GPIO_Pin = LEAKAGE_LED_PIN | VOL_LED_PIN | LIGHTPRO_LED_PIN | CH1O_LED_PIN | CH2O_LED_PIN |CH3O_LED_PIN;
    //! write command to pin.
    GPIO_WriteBit(LED_PORTD,ALL_GPIO_Pin,Bit_SET);
}

/*****************************************************************************************************/
/*                                  controlledn io initiliztion                                      */
/*****************************************************************************************************/
void ControlLed_Io_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 
  /* LED CONTROL IO */
  //
  //!PA9-PA12 PD0-PD4 PD7 led control gpio clock enable .
  //
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA |RCC_AHB1Periph_GPIOB |RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE, ENABLE);
  //
  //PA9-12 io  initilize.
  //
  GPIO_InitStructure.GPIO_Pin =  ACIN_LED_PIN|WORK_LED_PIN|CH4O_LED_PIN|REVERSE_LED_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_Init(LED_PORTA, &GPIO_InitStructure);
 //
 //PD0-PD4 PD7 initilize.
 //
  GPIO_InitStructure.GPIO_Pin =  LEAKAGE_LED_PIN|VOL_LED_PIN|LIGHTPRO_LED_PIN|CH1O_LED_PIN|CH2O_LED_PIN|CH3O_LED_PIN;
  GPIO_Init(LED_PORTD, &GPIO_InitStructure);
 //
 //ALL_LED_CLEAR 
 //
  ALL_LED_CLEAR();
  
 //
 // DELAY CONTROL IO INIT.  PE7 PD8 PD9 PD11 PD13 PD15
 //
  GPIO_InitStructure.GPIO_Pin =  LINKER_RELAY_PIN | CH1_RELAY_PIN | CH2_RELAY_PIN | CH3_RELAY_PIN | CH4_RELAY_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
//GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = LEAKAGE_RELAY_PIN | GPIO_Pin_11;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
 //CONNECT TO EARTH 
  RELAY_CTR(GPIOE,GPIO_Pin_11,ON); 
//LINK ON
 // RELAY_CTR(GPIOD,LINKER_RELAY_PIN,ON);
 //
 // ALARM CHECK.
 //
  GPIO_InitStructure.GPIO_Pin =  CH1_NOR_PIN | CH2_NOR_PIN | CH3_NOR_PIN ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = CH4_NOR_PIN;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin =  NORDER_CHECK_PIN | LORDER_CHECK_PIN | LIGHTPRE_CHECK_PIN ;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  
 //
 // KEY CHECK.
 //
  GPIO_InitStructure.GPIO_Pin =  RESTORE_KEY_PIN | LEAKAGE_KEY_PIN ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin =  OVERLOAD_KEY_PIN ;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
 
  
 //RESET ATT7053
  ATT7053_RESET();
  
  
}
/*****************************************************************************************************/
/*                                    controlled indicate                                            */
/*****************************************************************************************************/
u8_t flash_led[3] ={ON,OFF,ON};
u8_t flash_work = ON;

void Led_indicate(void)
{
  u8_t i ;
  u8_t ucRemote_cmd_index;
  State flash_chled = OFF;
  u8_t x = 0 ;
  //
  //! check eleven alarm bit and indicated led flash/light.
  //
  for(i = 0; i < 11; i++)
  {
      if(SysAlarm_Flag & Alarm_Bits[i])
      {
              //
              // if led flashbit alarm happed ,so flash led.
              if(Alarm_Bits[i] & FLASH_ALARM_BIT)
              {
                 x++;
                 flash_led[x] = !flash_led[x];
                 LED_ON_OFF(ulMap_Alarm_LED_PORT[i],ulMap_Alarm_LED_PIN[i],flash_led[x]);
              }
              
              //
              // other alarms ,so light the led.
              else
              {
                 LED_ON_OFF(ulMap_Alarm_LED_PORT[i],ulMap_Alarm_LED_PIN[i],ON);//if(g_sParameters.remote_cmd & 0x0F)
                
              }
      }
      else
      {   if(i >= CHOUPT_START)
          {
              ucRemote_cmd_index = i-CHOUPT_START;
              //
              //! if remote_cmd is on ,corresponing led lignht ,or not off.
              flash_chled = (Alarm_Bits[ucRemote_cmd_index] & ChannelStatus)? ON : OFF;
              LED_ON_OFF(ulMap_Alarm_LED_PORT[i],ulMap_Alarm_LED_PIN[i],flash_chled);
          }
          else
              LED_ON_OFF(ulMap_Alarm_LED_PORT[i],ulMap_Alarm_LED_PIN[i],OFF); 
      }
  }
  
     LED_ON_OFF(LED_PORTA,ACIN_LED_PIN,ON); 
     flash_work = ! flash_work;
     LED_ON_OFF(LED_PORTA,WORK_LED_PIN,flash_work);
}

/*****************************************************************************************************/
/*relay_on/off:GPIOx as IO port,GPIO_Pin as IO pin,LedState as relay_ON/LED_OFF                      */
/*****************************************************************************************************/
static void RELAY_CTR(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, u8_t State)
{
  
   //!if Relay on ,pin od seted as close to vcc.
     if(State != OFF)
     {
         GPIOx->BSRRL = GPIO_Pin;
         
     }
     else
     {
         GPIOx->BSRRH = GPIO_Pin;
     }
  
}
/*****************************************************************************************************/
/*Read_ReyStatus:GPIOx as IO port,GPIO_Pin as IO pin,LedState as relay_ON/LED_OFF                    */
/*****************************************************************************************************/
static State Read_ReyStatus(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
   
    State  ucReystatus ; 
   //!read 4 channel status.
    if( GPIOx->IDR  & GPIO_Pin)
         ucReystatus = ON;
    else
         ucReystatus = OFF;
    return ucReystatus;
}
//
// timeout regate Fuc.
static void regate_retry(void* arg)
{
   u8_t i =0;
   u32_t Detimeout = 0;
   for(i = 0; i < MAX_CHANNEL_NUM; i++)
     
   {    // the overcurrent happened, regate as the pre remote cmd to regate the relay on/off.
        if(g_sParameters.remote_cmd & Alarm_Bits[i])
        {
            //reopen the relay.
            RELAY_CTR(CHANNEL_PORT,ulMap_CTR_Relay_PIN[i],ON);
            //g_sParameters.remote_cmd |= Alarm_Bits[i];
        }
           
   }
   sys_untimeout(regate_retry, NULL);
   if(Regate_Timeout_CNT < g_sParameters.rgcnt)
   {
            Regate_Timeout_CNT++;
           
            if(Regate_Timeout_CNT == 1)
            {
                Detimeout = g_sParameters.delay2;
                Detimeout *= 100000;
                sys_timeout(Detimeout,regate_retry,NULL);
            }
            else if(Regate_Timeout_CNT == 2)
            {
                Detimeout = g_sParameters.delay3;
                Detimeout *= 100000;
                sys_timeout(Detimeout,regate_retry,NULL); 
            }
     } 
   //! stop the regate timer.
   //! realse the regate_retry timer.
   
   
   
}
//
//! CH_Regate Fuc,start condition if the system normal.
  
static void CH_Regate(void)
{   u32_t Detimeout = 0;
    // regatecnt set nozero.
    if(g_sParameters.rgcnt)
    {
       
       if(Regate_Timeout_CNT < g_sParameters.rgcnt)
       {
            if(Regate_Timeout_CNT == 0)
            {
                Detimeout = g_sParameters.delay1;
                Detimeout *= 100000;
                sys_timeout(Detimeout,regate_retry,NULL);
            }
            
       }  
    }
}

/*****************************************************************************************************/
/*                                    main_relay_control                                             */
/*****************************************************************************************************/
u8_t ulChannel =0; //继电器依次开通和依次关闭
static void Main_Relay_Control(void)
{
  //
  //! LOOP index.
  u8_t i =0;
  u16_t ucbit =0;
  //
  //!check statue delay
  u16_t ucCheckCHStatus = 3000;
   //
  //
  State  ucReystatus = OFF;
//it  process as command fuc,if remote cmd haved changed.
// if(ulChannel < MAX_CHANNEL_NUM)
//     ulChannel++;
//  else
//     ulChannel = 0;
  if(ulChannel < MAX_CHANNEL_NUM)
                 ulChannel++;
  if(ulChannel >= MAX_CHANNEL_NUM)
                 ulChannel = 0;
  if((!(SysAlarm_Flag & HIGH_RISK_BIT)) && (!Testing_Leakage))
  {
       for(i =0; i <  MAX_CHANNEL_NUM;i++)
       {
      //!see if the fixed time enable .or not manuaul. Alarm_bit High 4 bit :channel fixed time enable bit. low 4 bit : channel on/off bit.
             if(g_sParameters.remote_cmd & Alarm_Bits[i + MAX_CHANNEL_NUM])
             {
                   // the relay set to on status untill time recheive the boot time.
                    if((CurrentTime.hour == g_sParameters.boottime[i].hour)&&(CurrentTime.minute == g_sParameters.boottime[i].minute))
                    {
                          //RELAY_CTR(CHANNEL_PORT,ulMap_CTR_Relay_PIN[i],ON);
                          // set g_sParameters bit to chx on flag.
                          g_sParameters.remote_cmd |= Alarm_Bits[i];
                          //guChannel_Status = (g_sParameters.remote_cmd &0x0f);
                    }
                   // the relay set to off status untill time recheive the shut time.
                    if((CurrentTime.hour == g_sParameters.shuttime[i].hour)&&(CurrentTime.minute == g_sParameters.shuttime[i].minute))
                    {   
                         
                         //RELAY_CTR(CHANNEL_PORT,ulMap_CTR_Relay_PIN[i],OFF);
                         // set g_sParameters bit to chx off flag.
                         ucbit = Alarm_Bits[i];
                         g_sParameters.remote_cmd &= ~ucbit;
                         
                         //guChannel_Status = (g_sParameters.remote_cmd &0x0f);
                    }
            }
      }
      if(guChannel_Status != (g_sParameters.remote_cmd &0x0f))
      {               
                   //! see if remote_cmd bit high,corresponing to set on.or not off.
                   if(g_sParameters.remote_cmd & Alarm_Bits[ulChannel] )
                   {
                        // corresponing channel relay set to on status. 
                         RELAY_CTR(CHANNEL_PORT,ulMap_CTR_Relay_PIN[ulChannel],ON);
                         guChannel_Status |= Alarm_Bits[ulChannel];//(g_sParameters.remote_cmd &0x0f);
                   }
                   else
                   {
                       // corresponing channel relay set to off status.
                         RELAY_CTR(CHANNEL_PORT,ulMap_CTR_Relay_PIN[ulChannel],OFF);
                         guChannel_Status &= ~Alarm_Bits[ulChannel];//(g_sParameters.remote_cmd &0x0f);
                   }
      }
  }
                   //
                   //!channel Status Checked. 4 time checked.
                   for(i =0; i < (MAX_CHANNEL_NUM*2);i++)
                   {
                      ucbit = Alarm_Bits[ulChannel] &0xff;
                      if(ON == Read_ReyStatus(ulMap_CHStatus_Check_PORT[ulChannel],ulMap_CHStatus_Check_PIN[ulChannel]))
                      {
                                 // set corresponing bit.
                                    ChannelStatus |= ucbit ;
                                    if(g_sParameters.remote_cmd &Alarm_Bits[ulChannel])
                                      break;
                      }
                      else
                      {
                                //reset corresponing bit.
                                    ChannelStatus &= ~ucbit;
                                    if(!(g_sParameters.remote_cmd &Alarm_Bits[ulChannel]))
                                      break;
                      }
                      while(ucCheckCHStatus--);
                   }
                   //
                   //!  4 channel alarm bit. 
                    ucbit = Alarm_Bits[ulChannel+CHOUPT_START];
                   //! compare remote command and Channel status.
                    if((!(SysAlarm_Flag & HIGH_RISK_BIT)) && (!Testing_Leakage))
                    {
                        if((ChannelStatus &Alarm_Bits[ulChannel]) != (g_sParameters.remote_cmd &Alarm_Bits[ulChannel]))
                        {
                                     SysAlarm_Flag |= ucbit;
                                     discreteValue[ulChannel] = 0;
                        }
                        else
                        {
                                     SysAlarm_Flag &= ~ucbit; 
                                     discreteValue[ulChannel] = 1;
                        }
                    } 
//  for(i = 0; i < MAX_CHANNEL_NUM; i++)//Read_ReyStatus
//  {
//        //! check 4 chanel optical is light: AC haven outputed.
//          ucbit = Alarm_Bits[i] &0xff;
//          if(ON == Read_ReyStatus(ulMap_CHStatus_Check_PORT[i],ulMap_CHStatus_Check_PIN[i]))
//          {
//                     // set corresponing bit.
//                        ChannelStatus |= ucbit ;
//          }
//          else
//          {
//                    //reset corresponing bit.
//                        ChannelStatus &= ~ucbit;
//          }
//         //
//         //!  4 channel alarm bit. 
//          ucbit = Alarm_Bits[i+CHOUPT_START];
//         //! compare remote command and Channel status.
//          if((!(SysAlarm_Flag & HIGH_RISK_BIT)) && (!Testing_Leakage))
//          {
//              if((ChannelStatus &Alarm_Bits[i]) != (g_sParameters.remote_cmd &Alarm_Bits[i]))
//              {
//                           SysAlarm_Flag |= ucbit;
//                           discreteValue[i] = 0;
//              }
//              else
//              {
//                           SysAlarm_Flag &= ~ucbit; 
//                           discreteValue[i] = 1;
//              }
//          }
//          
//  }
  discreteValue[7] =  lamp_status;
}
/*****************************************************************************************************/
/*      Line_Status check :return 0x01 unearth,0x02 L N unorder     0x04 light unprotected 0x08                                 */
/*****************************************************************************************************/
void Line_Status_Check(void)
{
   u8_t ulAlarmFlag =0x03;// predefine alarm as unearth ,L N unorder,light unprotected.
   u8_t ulLlineST = 0;
   u8_t ulNlineST = 0;
   
   u8_t ubit =0;
   discreteValue[4] = 0;
   discreteValue[5] = 0;
   u8_t i = 0;
   u16_t ucDelayCnt = 3000;
   //
   //   open the unearth relay contected to earth,check if L N order and contected earth.
  
   RELAY_CTR(GPIOE,EARTH_RELAY_PIN,ON);
   
   for(i = 0; i< 5;i++)
   {
          // delay
         while(ucDelayCnt--);
         //   read N to earth if not haved voltage.
         ulNlineST  = Read_ReyStatus(ulMap_KeyOrOpt_PORT[ACN_OPT],ulMap_KeyOrOpt_PIN[ACN_OPT]);//KeyorOpti_Scan(ACN_OPT,OPT_TYPE);//Read_ReyStatus(GPIOE,NORDER_CHECK_PIN);
         //   read L to earth if not have voltage.
         ulLlineST =  Read_ReyStatus(ulMap_KeyOrOpt_PORT[ACL_OPT],ulMap_KeyOrOpt_PIN[ACL_OPT]);//KeyorOpti_Scan(ACL_OPT,OPT_TYPE);
         if(ulLlineST == ON)
             break;
   }
   //   
   //  close the unearth relay contected to earth,check if L N order and contected earth.
   RELAY_CTR(GPIOE,EARTH_RELAY_PIN,OFF);
   //   L N check success if the L check have vol,N check have no vol.  
   if(ulLlineST != ulNlineST)
   {
        if(ON == ulLlineST)
        {   // Release   the L N ounrder  alarm.
            ubit = Alarm_Bits[UNORDERLN] & 0xff; 
            ulAlarmFlag &= ~ubit;
            discreteValue[5] = 1;
           // Release   the unearth  alarm.
            ubit = Alarm_Bits[UNERATH] & 0xff; 
            ulAlarmFlag &= ~ubit;
            discreteValue[4] = 1;
        }
        else
        {
            // Release the unearth alarm.
            ubit = Alarm_Bits[UNERATH] & 0xff; 
            ulAlarmFlag &= ~ubit;
            discreteValue[4] = 1;
        }
   }
   // SysAlarm_Flag
   SysAlarm_Flag |= ulAlarmFlag;
   if(SysAlarm_Flag & HIGH_RISK_BIT)
   {
                                                                   //AC OUTPUT CLOSE.
        RELAY_CTR(GPIOD,CH1_RELAY_PIN,OFF);
        RELAY_CTR(GPIOD,CH2_RELAY_PIN,OFF);
        RELAY_CTR(GPIOD,CH3_RELAY_PIN,OFF);
        RELAY_CTR(GPIOD,CH4_RELAY_PIN,OFF);
        //RELAY_CTR(GPIOD,LINKER_RELAY_PIN,OFF);                     //close linker
   }
   else
   {
        //RELAY_CTR(GPIOD,LINKER_RELAY_PIN,ON);                     //open linker
   }
   
}

/*****************************************************************************************************/
/*      Loop_Line_Check                                                                              */
/*****************************************************************************************************/
static u8_t Leakagekey_Delay = 0;
static u8_t Leakcheckkey_Delay = 0;
static u8_t LoadFactorykey_Delay = 0;
//static u8_t lightpro_delay = 0;
static u8_t HiAlarm_Lock = 0;
static u8_t HiCurr_Lock = 0;
static u8_t RegateStart = 0;
u32_t   ACVOTAGE = 0;
u32_t   LEAKAGECUR = 0;
u32_t   ACLOADCUR = 0;
void Loop_Line_Check(void)
{
    u16_t ubit_alarm = 0;
    u8_t  key_status = 0;
    u32_t ucanalog = 0;
    u8_t  i = 0;
    u16_t uThunderDelayCnt = 3000;
    u8_t  uThunderCheckST = 0;
    //float temp =0;
    //
    
    // check the light unprotect cirut alarm haven happened.
//    if(OFF == Read_ReyStatus(GPIOE,LIGHTPRE_CHECK_PIN))
//    {
//        if(lightpro_delay < (LEAKKEY_DELAY))
//            lightpro_delay++;
//        else
//        {
//          //
//          //! light unprotected alarm bit set.
//           ubit_alarm = Alarm_Bits[UNPROLGIHT];
//           SysAlarm_Flag |= ubit_alarm;
//           
//           discreteValue[6] = 0;
//        }
//    }
//    else
//    {
//          lightpro_delay = 0;
//    }
     for(i = 0; i < 5; i++)
     {
        if(ON == Read_ReyStatus(GPIOE,LIGHTPRE_CHECK_PIN))
        {
           uThunderCheckST = ON;
           break;
        }
        else
           uThunderCheckST = OFF;
       
          
         while(uThunderDelayCnt--);  
     }
     
     if(uThunderCheckST == ON)
     {
        //! release the light unprotected alarm.
        ubit_alarm = Alarm_Bits[UNPROLGIHT];
        //! web thunder protected check alarm.
        SysAlarm_Flag &= ~ubit_alarm;
        //! SNMP thunder protected.
        discreteValue[6] = 1;
     }
     else
     {
        //! release the light unprotected alarm.
        ubit_alarm = Alarm_Bits[UNPROLGIHT];
        //! web thunder protected check alarm.
        SysAlarm_Flag |= ubit_alarm;
        //! SNMP thunder protected.
        discreteValue[6] = 0;
     }
       
    // !get the ac vol,load current,leakage current from the att7053.
      
       CPU_SR_ALLOC();
       CPU_CRITICAL_ENTER();                                 /* Tell uC/OS-III that we are starting an ISR             */
    // !get the ac vol value.   
       ucanalog = SPI_Read(0x08);
    // !judge the value is or not negative.
       if(ucanalog > ATT7053_ZEROPOINT)
       {
           ucanalog = BIT24_VAL - ucanalog;
       }
       ACVOTAGE = ucanalog;
    // !the fact value equal to (measure value / K PARA.
       ucanalog = ucanalog *100 /g_sSyscalparaters.Vk;
    // !cache the value to the analogValue array.   
       analogValue[0] = ucanalog &0xffff;
       
    // !leakage current measure.  
       ucanalog = SPI_Read(0x06) ;  
       if(ucanalog > ATT7053_ZEROPOINT)
       {
            ucanalog = BIT24_VAL - ucanalog;
       }
       LEAKAGECUR = ucanalog;
       if(g_sSyscalparaters.leakoffset == 0)
       {
           g_sSyscalparaters.leakoffset =ucanalog*ucanalog/4/32768;
       }
       ucanalog = ucanalog*100/g_sSyscalparaters.Lk;
       
       analogValue[1] = ucanalog &0xffff;
    // !load current measure.   
       ucanalog = SPI_Read(0x07);  
       if(ucanalog > ATT7053_ZEROPOINT)
       {
           ucanalog = BIT24_VAL - ucanalog;
       }
       ACLOADCUR = ucanalog;
       if(g_sSyscalparaters.curroffset == 0)
       {
           g_sSyscalparaters.curroffset =ucanalog*ucanalog/4/32768;
           SystemSave();
       }
       ucanalog = ucanalog*100/ g_sSyscalparaters.Ik;
       analogValue[2] = ucanalog &0xffff;
      
       CPU_CRITICAL_EXIT(); 
    //! release the overvol and uvlo alarm.
        ubit_alarm = Alarm_Bits[OVERVOL];
        ubit_alarm |= Alarm_Bits[UVLO];
    //! web pre set the voltage alarm. 
        SysAlarm_Flag &= ~ubit_alarm;  
    // check ac overvol and uvlo haven happened.
    if(analogValue[0] >= g_sParameters.OverVoltage) 
    {
            ubit_alarm = Alarm_Bits[OVERVOL];
            SysAlarm_Flag |= ubit_alarm;
    }
    else if(analogValue[0] <= g_sParameters.UVLO)
    {
            // the uvlo alarm is set.
            ubit_alarm = Alarm_Bits[UVLO];
            SysAlarm_Flag |= ubit_alarm;     
    }
    
    //! release the Leakage current and over current alarm.
        ubit_alarm = Alarm_Bits[LEAKAGE];
        ubit_alarm |= Alarm_Bits[OVERCUR];
        SysAlarm_Flag &= ~ubit_alarm; 
    //! check the over leakage current alarm.  
     if(analogValue[1] >= g_sParameters.LeakCurrent)
     {
         ubit_alarm = Alarm_Bits[LEAKAGE];
         SysAlarm_Flag |= ubit_alarm; 
     }
    //! check the over load current alarm. 
     if(analogValue[2] >= g_sParameters.OverCurrent)
     {
         ubit_alarm = Alarm_Bits[OVERCUR];
         SysAlarm_Flag |= ubit_alarm; 
     }
     //
     //! alarm coresonping to tha action.
     if(SysAlarm_Flag & HIGH_RISK_BIT)
     {
//      if((Regate_Timeout_CNT >= g_sParameters.rgcnt)||!Regate_Timeout_CNT)  
//      {//AC OUTPUT CLOSE.
        RELAY_CTR(GPIOD,CH1_RELAY_PIN,OFF);
        RELAY_CTR(GPIOD,CH2_RELAY_PIN,OFF);
        RELAY_CTR(GPIOD,CH3_RELAY_PIN,OFF);
        RELAY_CTR(GPIOD,CH4_RELAY_PIN,OFF);
//      }
        if(SysAlarm_Flag & HIGH_CURR_BIT)
           HiCurr_Lock = 1;
          
        HiAlarm_Lock = 1; 
        
        //! the hight risk alert only contain the overcurrent,so if the regate count have seted nozero,so active. 
//         if(SysAlarm_Flag & HIGH_RISK_BIT  == Alarm_Bits[OVERCUR] )
//         {
        if(!RegateStart )
        {
              CH_Regate();
              RegateStart =1;
        }
//         }
         //g_sParameters.remote_cmd &= 0xf0;
         guChannel_Status = g_sParameters.remote_cmd;
        
     }
     else
     {
        // if the system have no high risk, it have regate cnt reset.
        
        
            if(HiAlarm_Lock)
            { 
              //故障后关闭，开关手动复位,恢复之前状态
               //因过流或漏电出现断开开关后，会自动开关闭，复位后启动
              if(HiCurr_Lock)
              {
                g_sParameters.remote_cmd &= 0xf0; 
                guChannel_Status = !g_sParameters.remote_cmd;
              }
              else  
                guChannel_Status = !g_sParameters.remote_cmd;
               HiAlarm_Lock = 0;
               HiCurr_Lock = 0;
              //如果重合闸完毕，不再报警，清合闸次数，准备下次
               if(Regate_Timeout_CNT >= g_sParameters.rgcnt)
               {
                 Regate_Timeout_CNT = 0;
                 RegateStart = 0;
               }
            }
        
     }   
     
     //
     //! check the overload key and corresponning action.
     
     if(OFF == Read_ReyStatus(ulMap_KeyOrOpt_PORT[OVERLOAD_KEY],ulMap_KeyOrOpt_PIN[OVERLOAD_KEY]))
     {
         //if the key is push down and is cnt reset .
          if(!Test_Leakage_Cnt)
              Testing_Leakage = true;
          
          if(Test_Leakage_Cnt < MAX_CHANNEL_NUM)
          {       
                if(Leakagekey_Delay < LEAKKEY_DELAY)
                {
                     Leakagekey_Delay++;
                }
                else
                {
                   RELAY_CTR(CHANNEL_PORT,ulMap_CTR_Relay_PIN[Test_Leakage_Cnt],ON);
                   Test_Leakage_Cnt++;
                   Leakagekey_Delay = 0;
                }
          }
          guChannel_Status = !g_sParameters.remote_cmd;
     }
     else
     {
          if(Test_Leakage_Cnt)
               Test_Leakage_Cnt = 0;
          if(Testing_Leakage)
              Testing_Leakage = false;
     }
       
     //
     //! check the leakage check key is push down.if push down so open the relay to check the leakage current.
     if( OFF == Read_ReyStatus(ulMap_KeyOrOpt_PORT[LEAKAGE_KEY],ulMap_KeyOrOpt_PIN[LEAKAGE_KEY]))
     {
            RELAY_CTR(GPIOE , LEAKAGE_RELAY_PIN , ON);
            if(Leakcheckkey_Delay < LEAKKEY_DELAY)
            {
              Leakcheckkey_Delay++;
            }
            else 
            {
//              if(g_sSyscalparaters.Lk == 100)
//              {
                  
                  g_sSyscalparaters.Lk = LEAKAGECUR/11;
                  SystemSave();
//              }
            }
            
     }
     else
     {
            Leakcheckkey_Delay = 0;
            RELAY_CTR(GPIOE , LEAKAGE_RELAY_PIN , OFF);
     }
     //
     //! check the restore check key is push down. if push down so change all parameters to the factory parameters.
      if(OFF == Read_ReyStatus(ulMap_KeyOrOpt_PORT[RESTORE_KEY],ulMap_KeyOrOpt_PIN[RESTORE_KEY]))
      {
          //LoadFactorykey_Delay
			// Update the working parameter set with the factory defaults.
			//
                       if(LoadFactorykey_Delay < LEAKKEY_DELAY)
                       {
                          LoadFactorykey_Delay++;
                       }
                       else
                       {
			ConfigLoadFactory();
                       //g_sSyscalparaters = g_FactorySyscalparameters;
			//
			// Save the new defaults to flash.
			//
			ConfigSave();
                        //SystemSave();
			//
			// Apply the various changes required as a result of changing back to
			// the default settings.
			//
			ConfigUpdateAllParameters();
                       }
		 
      }
      else
      {
        LoadFactorykey_Delay = 0;
      }
      // relay loop control;
      Main_Relay_Control();
     
}