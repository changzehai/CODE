//
#include <includes.h>
#include "Sensor\dht11.h"
#define HIGH  1
#define LOW   0
#define NUMBER 9620
#define DHT11_DATA_OUT(a)	if (a)	\
					GPIO_SetBits(GPIOE,GPIO_Pin_1);\
					else		\
					GPIO_ResetBits(GPIOE,GPIO_Pin_1)
#define  DHT11_DATA_IN()	   GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_1)
//#define dht11      GPIOE->IDR  & GPIO_Pin_1
//#define dht11_H    GPIOE->BSRRL = GPIO_Pin_1
//#define dht11_L    GPIOE->BSRRH  = GPIO_Pin_1
//
////防止在与硬件通信时发生死循环的计数范围
//#define NUMBER 20
#define SIZE 5
//static unsigned char status;
////存放五字节数据的数组
static unsigned char value_array[SIZE];

///*可在其他的文件引用温湿度值,实际是温度的整数的10 倍
//如dht11 读回的温度是26,则temp_value = 260, 湿度同理*/
int temp_value;
int humi_value;


//
//
////初始DHT11 GPIO
//void DHT11_GPIO_CONFIGURATION(void)
//{
//    GPIO_InitTypeDef  GPIO_InitStructure; 
//  /* Configure dht11 pins: PE1 */
//  //
//  //!DHT11 gpio clock enable.
//  //
//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
//  //
//  //DHT11 gpio initilize.
//  //
//  dht11_H;
//  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
//  GPIO_Init(GPIOE, &GPIO_InitStructure);
//}
///*
//   Fuc name: DHT11_Mode_IPU
//Fuc descripte:  the  io configuaration of input .
//
// */
//static void DHT11_Mode_IPU(void)
//{
// 	  GPIO_InitTypeDef GPIO_InitStructure;
//	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
//	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN ; 
//	  GPIO_Init(GPIOE, &GPIO_InitStructure);	 
//}
///*
//   Fuc name: DHT11_Mode_Out_PP
//Fuc descripte:  the  io configuaration of output. 
//
// */
//static void DHT11_Mode_Out_PP(void)
//{
// 	  GPIO_InitTypeDef GPIO_InitStructure;
//	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
//	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT ; 
//	  GPIO_Init(GPIOE, &GPIO_InitStructure);	 
//}
////
////! dht11 delay 1 ms Fuc.
////
//static void Delay_1mst(unsigned int ms)
//{
//   OS_ERR err;
//   OSTimeDlyHMSM(0, 0,0, ms, OS_OPT_TIME_HMSM_STRICT, &err);
//}
////
////! dht11 delay 10 us Fuc.
////
//static void DHT11_Delay_10us(void)
//{
//        uint32_t ticks;
//        uint32_t told,tnow,tcnt=0;
//        uint32_t reload=SysTick->LOAD;        //LOAD的值                     
//        ticks=1200;                         //需要的节拍数                           
//        tcnt=0;
//        told=SysTick->VAL;                //刚进入时的计数器值
//        while(1)
//        {
//                tnow=SysTick->VAL;        
//                if(tnow!=told)
//                {            
//                        if(tnow<told)tcnt+=told-tnow;//这里注意一下SYSTICK是一个递减的计数器就可以了.
//                        else tcnt+=reload-tnow+told;            
//                        told=tnow;
//                        if(tcnt>=ticks)break;//时间超过/等于要延迟的时间,则退出.
//                }  
//        }       
//}
///*******Fuc name :DHT11_ReadValue   ********/
///******Return value : dht11 value   ********/
///*****                              *******/
//static unsigned char DHT11_ReadValue(void)
//{
//unsigned char count, value = 0, i;
//status = OK; //设定标志为正常状态
//for(i = 8; i > 0; i--)
//{
//    //高位在先
//    value <<= 1;
//    count = 0;
//    //每一位数据前会有一个50us 的低电平时间.等待50us 低电平结束
//    while(dht11 == 0 && count++ < NUMBER);
//    if(count >= NUMBER)
//    {
//    status = ERROR; //设定错误标志
//    return 0; //函数执行过程发生错误就退出函数
//    }
//    //26-28us 的高电平表示该位是0,为70us 高电平表该位1
//    DHT11_Delay_10us();
//    DHT11_Delay_10us();
//    DHT11_Delay_10us();
//    
//    //延时30us 后检测数据线是否还是高电平
//    if(dht11 != 0)
//    {
//    //进入这里表示该位是1
//    value++;
//    //等待剩余(约40us)的高电平结束
//    while(dht11 != 0 && count++ < NUMBER)
//    {
//      NULL;
//    }
//    if(count >= NUMBER)
//    {
//    status = ERROR; //设定错误标志
//    return 0;
//    }
//    }
//    }
//    return (value);
//}
//
////读温度和湿度函数，读一次的数据,共五字节，读出成功函数返回OK, 错误返回ERROR
//unsigned char DHT11_ReadTempAndHumi(void)
//{
//    unsigned char i = 0, check_value = 0,count = 0;
//   // CPU_SR_ALLOC();
//   // CPU_CRITICAL_ENTER();                                 /* Tell uC/OS-III that we are starting an ISR             */
//    dht11_L; //拉低数据线大于18ms 发送开始信号
//    Delay_1mst(20); //需大于18 毫秒
//    dht11_H; //释放数据线,用于检测低电平的应答信号
//    //延时20-40us,等待一段时间后检测应答信号,应答信号是从机拉低数据线80us
//    DHT11_Delay_10us();
//    DHT11_Delay_10us();
//    DHT11_Delay_10us();
//    //DHT11_Delay_10us();
//    DHT11_Mode_IPU();
//    if(dht11) //检测应答信号,应答信号是低电平
//    {
//        //没应答信号
//       // CPU_CRITICAL_EXIT(); 
//        DHT11_Mode_Out_PP();
//        dht11_H;
//        return ERROR;
//    }
//    else
//    {
//        //有应答信号
//        while(dht11 == 0 && count++ < NUMBER); //等待应答信号结束
//        if(count >= NUMBER) //检测计数器是否超过了设定的范围
//        {  
//            DHT11_Mode_Out_PP();
//            dht11_H;
//            //CPU_CRITICAL_EXIT(); 
//            return ERROR; //读数据出错,退出函数
//        }
//        count = 0;
//       
//        //dht11_H;//释放数据线
//        //应答信号后会有一个80us 的高电平，等待高电平结束
//        while(dht11 != 0 && count++ < NUMBER);
//        if(count >= NUMBER)
//        {
//            DHT11_Mode_Out_PP();
//            dht11_H;
//            //CPU_CRITICAL_EXIT(); 
//            return ERROR; //退出函数
//        }
//        //读出湿.温度值
//        for(i = 0; i < SIZE; i++)
//        {
//            value_array[i] = DHT11_ReadValue();
//            if(status == ERROR)//调用ReadValue()读数据出错会设定status 为ERROR
//            {
//                 DHT11_Mode_Out_PP();
//                 dht11_H;
//                //CPU_CRITICAL_EXIT(); 
//                return ERROR;
//            }
//            //读出的最后一个值是校验值不需加上去
//            if(i != SIZE - 1)
//            {
//                //读出的五字节数据中的前四字节数据和等于第五字节数据表示成功
//                check_value += value_array[i];
//            }
//        }//end for
//        //在没用发生函数调用失败时进行校验
//        if(check_value == value_array[SIZE - 1])
//        {
//            //将温湿度扩大10 倍方便分离出每一位
//            humi_value = value_array[0] * 10;
//            temp_value = value_array[2] * 10;
//            DHT11_Mode_Out_PP();
//            dht11_H;
//            //CPU_CRITICAL_EXIT(); 
//            return OK; //正确的读出dht11 输出的数据
//        }
//        else
//        {
//            //校验数据出错
//            //CPU_CRITICAL_EXIT(); 
//             DHT11_Mode_Out_PP();
//             dht11_H;
//            return ERROR;
//        }
//    }
//}

static void Delay_1mst(unsigned int ms)
{
   OS_ERR err;
   OSTimeDlyHMSM(0, 0,0, ms, OS_OPT_TIME_HMSM_STRICT, &err);
}
//
//! dht11 delay 10 us Fuc.
//
static void DHT11_Delay_10us(void)
{
        uint32_t ticks;
        uint32_t told,tnow,tcnt=0;
        uint32_t reload=SysTick->LOAD;        //LOAD的值                     
        ticks=1200;                         //需要的节拍数                           
        tcnt=0;
        told=SysTick->VAL;                //刚进入时的计数器值
        while(1)
        {
                tnow=SysTick->VAL;        
                if(tnow!=told)
                {            
                        if(tnow<told)tcnt+=told-tnow;//这里注意一下SYSTICK是一个递减的计数器就可以了.
                        else tcnt+=reload-tnow+told;            
                        told=tnow;
                        if(tcnt>=ticks)break;//时间超过/等于要延迟的时间,则退出.
                }  
        }       
}
/*
Fuc name :DHT11_GPIO_Config
Fuc desc: DHT11 GPIO Initilize.
 */
void DHT11_GPIO_Config(void)
{		
	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
													   
  	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
        GPIO_Init(GPIOE, &GPIO_InitStructure);	  

	GPIO_SetBits(GPIOE, GPIO_Pin_1);	 
}

/*
*Fuc name: DHT11_Mode_IPU
*Fuc desc: DHT11 set to input.
 */
static void DHT11_Mode_IPU(void)
{
 	 GPIO_InitTypeDef GPIO_InitStructure;
	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN ; 

	 GPIO_Init(GPIOE, &GPIO_InitStructure);	 
}

/*
* Fuc name: DHT11_Mode_Out_OD
* Fuc desc: DHT11 SET TO OUT OD WITH UP.
 */
static void DHT11_Mode_Out_PP(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
															   
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;	
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  	GPIO_Init(GPIOE, &GPIO_InitStructure);	 	 
}

/* 
* Fuc name: Read_Byte
* Fuc desc: read one byte from the dht11.
* return  : one byte.
 */
static uint8_t Read_Byte(void)
{
	uint8_t i, temp=0;
        unsigned int count;
	for(i=0;i<8;i++)    
	{	 
                temp <<= 1;
                count = 0;
		while((DHT11_DATA_IN()==Bit_RESET)&&(count++ < NUMBER ));// start transmit that delay 50 us.
               
                if(count >= NUMBER)
                {
                  return 0;
                }
                // 26-28 us hi : 0  70 us hi: 1
		DHT11_Delay_10us(); 
                DHT11_Delay_10us();
                DHT11_Delay_10us();
                // delay 30 us ,see if not hi .hi as 1,low as 0.
		if(DHT11_DATA_IN()==Bit_SET)
		{       
                        temp++;
                        count = 0;
			while((DHT11_DATA_IN()==Bit_SET)&&(count++ < NUMBER));
                        if(count >= NUMBER)
                        {
                          return 0;
                        }
			 
		}
		
	}
	return temp;
}
/*
* Fuc name:Read_DHT11
* Fuc desc:read the humidity and temperature from the dht11.
* input para:
* return para:  sucess or err.
 */
uint8_t Read_DHT11(void)
{  
	uint16_t count;
	DHT11_Mode_Out_PP();
	
	DHT11_DATA_OUT(LOW);
	
	Delay_1mst(20);//must  need min 18 ms.
	DHT11_DATA_OUT(HIGH); 

	DHT11_Delay_10us();
        DHT11_Delay_10us();
        DHT11_Delay_10us();// delay 30 us.
 
	DHT11_Mode_IPU();

	if(DHT11_DATA_IN()==Bit_RESET)     
	{       
                count = 0;
		while((DHT11_DATA_IN()==Bit_RESET)&&(count++ < NUMBER ));
                if(count >= NUMBER)
                  return ERROR;
                
                count = 0;
		while((DHT11_DATA_IN()==Bit_SET)&&(count++ < NUMBER ));
                if(count >= NUMBER)
                  return ERROR;

		value_array[0]= Read_Byte();
		value_array[1]= Read_Byte();
		value_array[2]= Read_Byte();
		value_array[3]= Read_Byte();
		value_array[4]= Read_Byte();

		DHT11_Mode_Out_PP();
		DHT11_DATA_OUT(HIGH);

		
		if(value_array[4] == value_array[0] + value_array[1] + value_array[2]+ value_array[3])
                {
                      humi_value = value_array[0] * 10;
                      temp_value = value_array[2] * 10;
			return SUCCESS;
                }
		else 
			return ERROR;
	}
	else
	{		
		return ERROR;
	}   
}

	  


/*************************************END OF FILE******************************/

