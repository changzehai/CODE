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
////��ֹ����Ӳ��ͨ��ʱ������ѭ���ļ�����Χ
//#define NUMBER 20
#define SIZE 5
//static unsigned char status;
////������ֽ����ݵ�����
static unsigned char value_array[SIZE];

///*�����������ļ�������ʪ��ֵ,ʵ�����¶ȵ�������10 ��
//��dht11 ���ص��¶���26,��temp_value = 260, ʪ��ͬ��*/
int temp_value;
int humi_value;


//
//
////��ʼDHT11 GPIO
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
//        uint32_t reload=SysTick->LOAD;        //LOAD��ֵ                     
//        ticks=1200;                         //��Ҫ�Ľ�����                           
//        tcnt=0;
//        told=SysTick->VAL;                //�ս���ʱ�ļ�����ֵ
//        while(1)
//        {
//                tnow=SysTick->VAL;        
//                if(tnow!=told)
//                {            
//                        if(tnow<told)tcnt+=told-tnow;//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
//                        else tcnt+=reload-tnow+told;            
//                        told=tnow;
//                        if(tcnt>=ticks)break;//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
//                }  
//        }       
//}
///*******Fuc name :DHT11_ReadValue   ********/
///******Return value : dht11 value   ********/
///*****                              *******/
//static unsigned char DHT11_ReadValue(void)
//{
//unsigned char count, value = 0, i;
//status = OK; //�趨��־Ϊ����״̬
//for(i = 8; i > 0; i--)
//{
//    //��λ����
//    value <<= 1;
//    count = 0;
//    //ÿһλ����ǰ����һ��50us �ĵ͵�ƽʱ��.�ȴ�50us �͵�ƽ����
//    while(dht11 == 0 && count++ < NUMBER);
//    if(count >= NUMBER)
//    {
//    status = ERROR; //�趨�����־
//    return 0; //����ִ�й��̷���������˳�����
//    }
//    //26-28us �ĸߵ�ƽ��ʾ��λ��0,Ϊ70us �ߵ�ƽ���λ1
//    DHT11_Delay_10us();
//    DHT11_Delay_10us();
//    DHT11_Delay_10us();
//    
//    //��ʱ30us �����������Ƿ��Ǹߵ�ƽ
//    if(dht11 != 0)
//    {
//    //���������ʾ��λ��1
//    value++;
//    //�ȴ�ʣ��(Լ40us)�ĸߵ�ƽ����
//    while(dht11 != 0 && count++ < NUMBER)
//    {
//      NULL;
//    }
//    if(count >= NUMBER)
//    {
//    status = ERROR; //�趨�����־
//    return 0;
//    }
//    }
//    }
//    return (value);
//}
//
////���¶Ⱥ�ʪ�Ⱥ�������һ�ε�����,�����ֽڣ������ɹ���������OK, ���󷵻�ERROR
//unsigned char DHT11_ReadTempAndHumi(void)
//{
//    unsigned char i = 0, check_value = 0,count = 0;
//   // CPU_SR_ALLOC();
//   // CPU_CRITICAL_ENTER();                                 /* Tell uC/OS-III that we are starting an ISR             */
//    dht11_L; //���������ߴ���18ms ���Ϳ�ʼ�ź�
//    Delay_1mst(20); //�����18 ����
//    dht11_H; //�ͷ�������,���ڼ��͵�ƽ��Ӧ���ź�
//    //��ʱ20-40us,�ȴ�һ��ʱ�����Ӧ���ź�,Ӧ���ź��Ǵӻ�����������80us
//    DHT11_Delay_10us();
//    DHT11_Delay_10us();
//    DHT11_Delay_10us();
//    //DHT11_Delay_10us();
//    DHT11_Mode_IPU();
//    if(dht11) //���Ӧ���ź�,Ӧ���ź��ǵ͵�ƽ
//    {
//        //ûӦ���ź�
//       // CPU_CRITICAL_EXIT(); 
//        DHT11_Mode_Out_PP();
//        dht11_H;
//        return ERROR;
//    }
//    else
//    {
//        //��Ӧ���ź�
//        while(dht11 == 0 && count++ < NUMBER); //�ȴ�Ӧ���źŽ���
//        if(count >= NUMBER) //���������Ƿ񳬹����趨�ķ�Χ
//        {  
//            DHT11_Mode_Out_PP();
//            dht11_H;
//            //CPU_CRITICAL_EXIT(); 
//            return ERROR; //�����ݳ���,�˳�����
//        }
//        count = 0;
//       
//        //dht11_H;//�ͷ�������
//        //Ӧ���źź����һ��80us �ĸߵ�ƽ���ȴ��ߵ�ƽ����
//        while(dht11 != 0 && count++ < NUMBER);
//        if(count >= NUMBER)
//        {
//            DHT11_Mode_Out_PP();
//            dht11_H;
//            //CPU_CRITICAL_EXIT(); 
//            return ERROR; //�˳�����
//        }
//        //����ʪ.�¶�ֵ
//        for(i = 0; i < SIZE; i++)
//        {
//            value_array[i] = DHT11_ReadValue();
//            if(status == ERROR)//����ReadValue()�����ݳ�����趨status ΪERROR
//            {
//                 DHT11_Mode_Out_PP();
//                 dht11_H;
//                //CPU_CRITICAL_EXIT(); 
//                return ERROR;
//            }
//            //���������һ��ֵ��У��ֵ�������ȥ
//            if(i != SIZE - 1)
//            {
//                //���������ֽ������е�ǰ���ֽ����ݺ͵��ڵ����ֽ����ݱ�ʾ�ɹ�
//                check_value += value_array[i];
//            }
//        }//end for
//        //��û�÷�����������ʧ��ʱ����У��
//        if(check_value == value_array[SIZE - 1])
//        {
//            //����ʪ������10 ����������ÿһλ
//            humi_value = value_array[0] * 10;
//            temp_value = value_array[2] * 10;
//            DHT11_Mode_Out_PP();
//            dht11_H;
//            //CPU_CRITICAL_EXIT(); 
//            return OK; //��ȷ�Ķ���dht11 ���������
//        }
//        else
//        {
//            //У�����ݳ���
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
        uint32_t reload=SysTick->LOAD;        //LOAD��ֵ                     
        ticks=1200;                         //��Ҫ�Ľ�����                           
        tcnt=0;
        told=SysTick->VAL;                //�ս���ʱ�ļ�����ֵ
        while(1)
        {
                tnow=SysTick->VAL;        
                if(tnow!=told)
                {            
                        if(tnow<told)tcnt+=told-tnow;//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
                        else tcnt+=reload-tnow+told;            
                        told=tnow;
                        if(tcnt>=ticks)break;//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
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

