#include <includes.h>
#include "arch/cc.h"
#include "config.h"
#include "I2C_DRV.h"
/********************************************************/              


void Delay_1ms(uint16_t time)
{	
   u16_t i=1000; //这里可以优化速度	，经测试最低到5还能写入
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
   u8_t i=100; //这里可以优化速度	，经测试最低到5还能写入
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
	if(!SDA_read)return false;	//SDA线为低电平则总线忙,退出
	SDA_L;
	I2C_delay();
	if(SDA_read) return false;	//SDA线为高电平则总线出错,退出
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

tBoolean I2C_WaitAck(void) 	 //返回为:=1有ACK,=0无ACK
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

void I2C_SendByte(u8_t SendByte) //数据从高位到低位//
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

uint8_t I2C_ReceiveByte(void)  //数据从高位到低位//
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

 
//写入1字节数据       待写入数据    待写入地址       器件类型(24c16或SD2403)
tBoolean I2C_WriteByte(u8_t SendByte, u16_t WriteAddress, u8_t DeviceAddress)
{		
    if(!I2C_Start())return false;
    I2C_SendByte(((WriteAddress & 0x0700) >>7) | DeviceAddress & 0xFFFE);//设置高起始地址+器件地址 
    if(!I2C_WaitAck()){I2C_Stop(); return false;}
    I2C_SendByte((uint8_t)(WriteAddress & 0x00FF));   //设置低起始地址      
    I2C_WaitAck();	
    I2C_SendByte(SendByte);
    I2C_WaitAck();   
    I2C_Stop(); 
	//注意：因为这里要等待EEPROM写完，可以采用查询或延时方式(10ms)
    //Systick_Delay_1ms(10);
    return true;
}
 
//注意不能跨页写
//写入1串数据      待写入数组地址    待写入长度      待写入地址       器件类型(24c16或SD2403)
tBoolean I2C_BufferWrite(u8_t* pBuffer, u8_t length,     u16_t WriteAddress, u8_t DeviceAddress) 
{
    if(!I2C_Start())return false;
    I2C_SendByte(((WriteAddress & 0x0700) >>7) | DeviceAddress & 0xFFFE);//设置高起始地址+器件地址 
    if(!I2C_WaitAck()){I2C_Stop(); return false;}
    I2C_SendByte((uint8_t)(WriteAddress & 0x00FF));   //设置低起始地址      
	  I2C_WaitAck();	
	  
		while(length--)
		{
		  I2C_SendByte(* pBuffer);
		  I2C_WaitAck();
          pBuffer++;
		}
	  I2C_Stop();
	  //注意：因为这里要等待EEPROM写完，可以采用查询或延时方式(10ms)
	  //Systick_Delay_1ms(10);
	  return true;
}


//跨页写入1串数据  待写入数组地址    待写入长度      待写入地址       器件类型(24c16或SD2403)
void I2C_PageWrite(  u8_t* pBuffer, u8_t length,     u16_t WriteAddress, u8_t DeviceAddress)
{
    u8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;
    Addr  = WriteAddress % I2C_PageSize;      //写入地址是开始页的第几位
    count = I2C_PageSize - Addr;					    //在开始页要写入的个数
    NumOfPage   =  length / I2C_PageSize;     //要写入的页数
    NumOfSingle =  length % I2C_PageSize;     //不足一页的个数

    if(Addr == 0)         //写入地址是页的开始 
    {
      if(NumOfPage == 0)  //数据小于一页 
      {
        I2C_BufferWrite(pBuffer,NumOfSingle,WriteAddress,DeviceAddress);   //写少于一页的数据
      }
      else		            //数据大于等于一页  
      {
        while(NumOfPage)//要写入的页数
        {
          I2C_BufferWrite(pBuffer,I2C_PageSize,WriteAddress,DeviceAddress);//写一页的数据
          WriteAddress +=  I2C_PageSize;
          pBuffer      +=  I2C_PageSize;
					NumOfPage--;
					Delay_1ms(10);
         }
        if(NumOfSingle!=0)//剩余数据小于一页
        {
          I2C_BufferWrite(pBuffer,NumOfSingle,WriteAddress,DeviceAddress); //写少于一页的数据
					Delay_1ms(10);
        }
       }
    }

    else                  //写入地址不是页的开始 
    {
      if(NumOfPage== 0)   //数据小于一页 
      {
        I2C_BufferWrite(pBuffer,NumOfSingle,WriteAddress,DeviceAddress);   //写少于一页的数据
      }
      else                //数据大于等于一页
      {
        length       -= count;
        NumOfPage     = length / I2C_PageSize;  //重新计算要写入的页数
        NumOfSingle   = length % I2C_PageSize;  //重新计算不足一页的个数	
      
        if(count != 0)
        {  
				  I2C_BufferWrite(pBuffer,count,WriteAddress,DeviceAddress);      //将开始的空间写满一页
          WriteAddress += count;
          pBuffer      += count;
         } 
      
        while(NumOfPage--)  //要写入的页数
        {
          I2C_BufferWrite(pBuffer,I2C_PageSize,WriteAddress,DeviceAddress);//写一页的数据
          WriteAddress +=  I2C_PageSize;
          pBuffer      +=  I2C_PageSize; 
        }
        if(NumOfSingle != 0)//剩余数据小于一页
        {
          I2C_BufferWrite(pBuffer,NumOfSingle,WriteAddress,DeviceAddress); //写少于一页的数据 
        }
      }
    } 
}

//读出1串数据         存放读出数据  待读出长度      待读出地址       器件类型(24c16或SD2403)	
tBoolean I2C_ReadByte(uint8_t* pBuffer,   uint8_t length,     uint16_t ReadAddress,  uint8_t DeviceAddress)
{		
    if(!I2C_Start())return false;
    I2C_SendByte(((ReadAddress & 0x0700) >>7) | DeviceAddress & 0xFFFE);//设置高起始地址+器件地址 
    if(!I2C_WaitAck()){I2C_Stop(); return false;}
    I2C_SendByte((uint8_t)(ReadAddress & 0x00FF));   //设置低起始地址      
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



