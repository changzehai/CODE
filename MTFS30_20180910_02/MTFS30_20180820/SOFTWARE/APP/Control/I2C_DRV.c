#include <includes.h>
#include "arch/cc.h"
#include "config.h"
#include "I2C_DRV.h"
/********************************************************/              


void Delay_1ms(uint16_t time)
{	
   u16_t i=1000; //��������Ż��ٶ�	����������͵�5����д��
   for(;time>0;time--)
   {
	   while(i) 
	   { 
	     i--; 
	   }
	} 
}

void I2C_GPIO_Config(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 
  /* Configure I2C1 pins: SCL ----PB6 and SDA ----PB9 */
  //
  //!i2c gpio clock enable.
  //
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE);
  //
  //SCL--PB6 initilize.
  //
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
//  __GPIOF_CLK_ENABLE();
//  GPIO_InitStructure.Pin =  GPIO_PIN_9;
//  GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
//  GPIO_InitStructure.Pull = GPIO_PULLUP;
//  GPIO_InitStructure.Speed = GPIO_SPEED_MEDIUM;
//  HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);
	
}


void I2C_delay(void)
{	
   u8_t i=100; //��������Ż��ٶ�	����������͵�5����д��
   while(i) 
   { 
     i--; 
   } 
}

tBoolean I2C_Start(void)
{
	SDA_H;
	SCL_H;
	I2C_delay();
	if(!SDA_read)return false;	//SDA��Ϊ�͵�ƽ������æ,�˳�
	SDA_L;
	I2C_delay();
	if(SDA_read) return false;	//SDA��Ϊ�ߵ�ƽ�����߳���,�˳�
	SDA_L;
	I2C_delay();
	return true;
}

void I2C_Stop(void)
{
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SDA_H;
	I2C_delay();
}

void I2C_Ack(void)
{	
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}

void I2C_NoAck(void)
{	
	SCL_L;
	I2C_delay();
	SDA_H;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}

tBoolean I2C_WaitAck(void) 	 //����Ϊ:=1��ACK,=0��ACK
{
	SCL_L;
	I2C_delay();
	SDA_H;			
	I2C_delay();
	SCL_H;
	I2C_delay();
	if(SDA_read)
	{
      SCL_L;
      return false;
	}
	SCL_L;
	return true;
}

void I2C_SendByte(u8_t SendByte) //���ݴӸ�λ����λ//
{
    uint8_t i=8;
    while(i--)
    {
        SCL_L;
        I2C_delay();
      if(SendByte&0x80)
        SDA_H;  
      else 
        SDA_L;   
        SendByte<<=1;
        I2C_delay();
		SCL_H;
        I2C_delay();
    }
    SCL_L;
}

uint8_t I2C_ReceiveByte(void)  //���ݴӸ�λ����λ//
{ 
    u8_t i=8;
    u8_t ReceiveByte=0;

    SDA_H;				
    while(i--)
    {
      ReceiveByte<<=1;      
      SCL_L;
      I2C_delay();
      SCL_H;
      I2C_delay();	
      if(SDA_read)
      {
        ReceiveByte|=0x01;
      }
    }
    SCL_L;
    return ReceiveByte;
}

 
//д��1�ֽ�����       ��д������    ��д���ַ       ��������(24c16��SD2403)
tBoolean I2C_WriteByte(u8_t SendByte, u16_t WriteAddress, u8_t DeviceAddress)
{		
    if(!I2C_Start())return false;
    I2C_SendByte(((WriteAddress & 0x0700) >>7) | DeviceAddress & 0xFFFE);//���ø���ʼ��ַ+������ַ 
    if(!I2C_WaitAck()){I2C_Stop(); return false;}
    I2C_SendByte((uint8_t)(WriteAddress & 0x00FF));   //���õ���ʼ��ַ      
    I2C_WaitAck();	
    I2C_SendByte(SendByte);
    I2C_WaitAck();   
    I2C_Stop(); 
	//ע�⣺��Ϊ����Ҫ�ȴ�EEPROMд�꣬���Բ��ò�ѯ����ʱ��ʽ(10ms)
    //Systick_Delay_1ms(10);
    return true;
}
 
//ע�ⲻ�ܿ�ҳд
//д��1������      ��д�������ַ    ��д�볤��      ��д���ַ       ��������(24c16��SD2403)
tBoolean I2C_BufferWrite(u8_t* pBuffer, u8_t length,     u16_t WriteAddress, u8_t DeviceAddress) 
{
    if(!I2C_Start())return false;
    I2C_SendByte(((WriteAddress & 0x0700) >>7) | DeviceAddress & 0xFFFE);//���ø���ʼ��ַ+������ַ 
    if(!I2C_WaitAck()){I2C_Stop(); return false;}
    I2C_SendByte((uint8_t)(WriteAddress & 0x00FF));   //���õ���ʼ��ַ      
	  I2C_WaitAck();	
	  
		while(length--)
		{
		  I2C_SendByte(* pBuffer);
		  I2C_WaitAck();
          pBuffer++;
		}
	  I2C_Stop();
	  //ע�⣺��Ϊ����Ҫ�ȴ�EEPROMд�꣬���Բ��ò�ѯ����ʱ��ʽ(10ms)
	  //Systick_Delay_1ms(10);
	  return true;
}


//��ҳд��1������  ��д�������ַ    ��д�볤��      ��д���ַ       ��������(24c16��SD2403)
void I2C_PageWrite(  u8_t* pBuffer, u8_t length,     u16_t WriteAddress, u8_t DeviceAddress)
{
    u8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;
    Addr  = WriteAddress % I2C_PageSize;      //д���ַ�ǿ�ʼҳ�ĵڼ�λ
    count = I2C_PageSize - Addr;					    //�ڿ�ʼҳҪд��ĸ���
    NumOfPage   =  length / I2C_PageSize;     //Ҫд���ҳ��
    NumOfSingle =  length % I2C_PageSize;     //����һҳ�ĸ���

    if(Addr == 0)         //д���ַ��ҳ�Ŀ�ʼ 
    {
      if(NumOfPage == 0)  //����С��һҳ 
      {
        I2C_BufferWrite(pBuffer,NumOfSingle,WriteAddress,DeviceAddress);   //д����һҳ������
      }
      else		            //���ݴ��ڵ���һҳ  
      {
        while(NumOfPage)//Ҫд���ҳ��
        {
          I2C_BufferWrite(pBuffer,I2C_PageSize,WriteAddress,DeviceAddress);//дһҳ������
          WriteAddress +=  I2C_PageSize;
          pBuffer      +=  I2C_PageSize;
					NumOfPage--;
					Delay_1ms(10);
         }
        if(NumOfSingle!=0)//ʣ������С��һҳ
        {
          I2C_BufferWrite(pBuffer,NumOfSingle,WriteAddress,DeviceAddress); //д����һҳ������
					Delay_1ms(10);
        }
       }
    }

    else                  //д���ַ����ҳ�Ŀ�ʼ 
    {
      if(NumOfPage== 0)   //����С��һҳ 
      {
        I2C_BufferWrite(pBuffer,NumOfSingle,WriteAddress,DeviceAddress);   //д����һҳ������
      }
      else                //���ݴ��ڵ���һҳ
      {
        length       -= count;
        NumOfPage     = length / I2C_PageSize;  //���¼���Ҫд���ҳ��
        NumOfSingle   = length % I2C_PageSize;  //���¼��㲻��һҳ�ĸ���	
      
        if(count != 0)
        {  
				  I2C_BufferWrite(pBuffer,count,WriteAddress,DeviceAddress);      //����ʼ�Ŀռ�д��һҳ
          WriteAddress += count;
          pBuffer      += count;
         } 
      
        while(NumOfPage--)  //Ҫд���ҳ��
        {
          I2C_BufferWrite(pBuffer,I2C_PageSize,WriteAddress,DeviceAddress);//дһҳ������
          WriteAddress +=  I2C_PageSize;
          pBuffer      +=  I2C_PageSize; 
        }
        if(NumOfSingle != 0)//ʣ������С��һҳ
        {
          I2C_BufferWrite(pBuffer,NumOfSingle,WriteAddress,DeviceAddress); //д����һҳ������ 
        }
      }
    } 
}

//����1������         ��Ŷ�������  ����������      ��������ַ       ��������(24c16��SD2403)	
tBoolean I2C_ReadByte(uint8_t* pBuffer,   uint8_t length,     uint16_t ReadAddress,  uint8_t DeviceAddress)
{		
    if(!I2C_Start())return false;
    I2C_SendByte(((ReadAddress & 0x0700) >>7) | DeviceAddress & 0xFFFE);//���ø���ʼ��ַ+������ַ 
    if(!I2C_WaitAck()){I2C_Stop(); return false;}
    I2C_SendByte((uint8_t)(ReadAddress & 0x00FF));   //���õ���ʼ��ַ      
    I2C_WaitAck();
    I2C_Start();
    I2C_SendByte(((ReadAddress & 0x0700) >>7) | DeviceAddress | 0x0001);
    I2C_WaitAck();
    while(length)
    {
      *pBuffer = I2C_ReceiveByte();
      if(length == 1)I2C_NoAck();
      else I2C_Ack(); 
      pBuffer++;
      length--;
    }
    I2C_Stop();
    return true;
}



