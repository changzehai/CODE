#ifndef CONTROL_HEADER
#define CONTROL_HEADER
//
//!AC IN LED indicate io.
//
#define  ACIN_LED_PIN  GPIO_Pin_9  
//
//!work led indicate io.
//
#define  WORK_LED_PIN  GPIO_Pin_10
//
//!ch4 on led indicate io.
//
#define  CH4O_LED_PIN  GPIO_Pin_11
//
//! AC L/N order indicate io.
//
#define  REVERSE_LED_PIN  GPIO_Pin_12

//
//! leakage current led pin.
//
#define LEAKAGE_LED_PIN  GPIO_Pin_0
//
//! vol indicated led pin.
//
#define VOL_LED_PIN      GPIO_Pin_1
//
//! light protected indicate led io pin.
//
#define LIGHTPRO_LED_PIN GPIO_Pin_2
//
//! channel 1 AC ouputed indicated led pin.
//
#define CH1O_LED_PIN     GPIO_Pin_3
//
//! channel 2 AC outputed indicated led pin.
//
#define CH2O_LED_PIN     GPIO_Pin_4
//
//! channel 3 AC outputed indicated led pin.
#define CH3O_LED_PIN     GPIO_Pin_7


//
//! led1 port indicate.
//
#define LED_PORTA      GPIOA
//
//! led portd define.
#define LED_PORTD      GPIOD

/*****************************************/
/*          Relay control io             */
#define LEAKAGE_RELAY_PIN       GPIO_Pin_7
#define EARTH_RELAY_PIN         GPIO_Pin_11
#define LINKER_RELAY_PIN        GPIO_Pin_8
#define CH1_RELAY_PIN           GPIO_Pin_9
#define CH2_RELAY_PIN           GPIO_Pin_11
#define CH3_RELAY_PIN           GPIO_Pin_13
#define CH4_RELAY_PIN           GPIO_Pin_15

#define CHANNEL_PORT            GPIOD
/*****************************************/
/*            ALARM  CHECK               */
#define NORDER_CHECK_PIN        GPIO_Pin_8
#define LORDER_CHECK_PIN        GPIO_Pin_9
#define LIGHTPRE_CHECK_PIN      GPIO_Pin_10
#define CH1_NOR_PIN             GPIO_Pin_10
#define CH2_NOR_PIN             GPIO_Pin_12
#define CH3_NOR_PIN             GPIO_Pin_14
#define CH4_NOR_PIN             GPIO_Pin_9
/*****************************************/
/*            KEY  CHECK               */
#define OVERLOAD_KEY_PIN            GPIO_Pin_0
#define RESTORE_KEY_PIN             GPIO_Pin_5
#define LEAKAGE_KEY_PIN             GPIO_Pin_8

#define HIGH_RISK_BIT      0x7B
#define HIGH_CURR_BIT      0x48
//! GPIO_PinState defintion.
typedef enum
{
  ON = 0,
  OFF
}State;

//! ERR ENUM defintion.
typedef enum
{
  UNERATH = 0,
  UNORDERLN,
  UNPROLGIHT,
  OVERCUR,
  OVERVOL,
  UVLO,
  LEAKAGE
}ERR_Status;
//! Key scan index defintion.
typedef enum
{
  OVERLOAD_KEY = 0,
  LEAKAGE_KEY,
  RESTORE_KEY,
  ACL_OPT,
  ACN_OPT,
  MAX_KEYOROPT_NUM
}KeyorOpt_Scan_Index;

//
//
//! led io init Fuc.
void ControlLed_Io_Init(void);
//
//! led indicate Fuc.
void Led_indicate(void);
//
void Loop_Line_Check(void);
//
//! initial check the L N ODER.
void Line_Status_Check(void);
#endif