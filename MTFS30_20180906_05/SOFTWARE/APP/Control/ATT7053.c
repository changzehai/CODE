
#define ATT7053_GLOBALS

#include <includes.h>
#include "arch/cc.h"
#include "Control\ATT7053.h"
#include "config.h"

uint8_t miso_val;
extern tSysCALParameters g_sSyscalparaters;
void delay_us(void) 
{  
	__NOP();__NOP();__NOP();__NOP();__NOP();
 	__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();
} 

void Delay(uint32_t t) 
{  
	uint32_t i; 
	for(i=0;i<t;i++) 
		delay_us();
} 

uint32_t SPI_Read(uint8_t address)
{
	int8_t i;					
	uint32_t temp_data;
	temp_data = 0;			
	ATT7053_SCLK_L;	
	Delay(10);	
	ATT7053_CS_H;	
	Delay(20);		
	ATT7053_CS_L;		
	Delay(50);							
	ATT7053_SCLK_H;	
	Delay(10);
	ATT7053_DIN_L;	
	Delay(10);
	ATT7053_SCLK_L;	
	Delay(10);
				
	for(i=6; i>=0; i--)
	{
		ATT7053_SCLK_H;		
		Delay(10);
		if((address & (0x01<<i))>>i)
                {
                      ATT7053_DIN_H;	
                }
		else 
                      ATT7053_DIN_L;
		Delay(10);
		ATT7053_SCLK_L;	
		Delay(10);
	}
						
	Delay(20);	
	ATT7053_SCLK_H;	
	Delay(10);
	ATT7053_SCLK_L;
	if(ATT7053_DOUT) temp_data=temp_data|1;
	Delay(10);	
	for(i=0; i<23; i++)
	{
		ATT7053_SCLK_H;	
		Delay(10);
		ATT7053_SCLK_L;
		if(ATT7053_DOUT)miso_val=1;
		else miso_val=0;
		temp_data = (temp_data<<1) | miso_val;
		Delay(10);
	}
	ATT7053_CS_H;
	return temp_data;
}

void	SPI_Write(uint8_t address, uint32_t write_data)
{
	int8_t i;					
	uint32_t temp_data;
	temp_data = 1;			
	ATT7053_SCLK_L;		
	Delay(10);	
	ATT7053_CS_H;	
	Delay(50);		
	ATT7053_CS_L;		
	Delay(10);		
					
	ATT7053_SCLK_H;		
	Delay(10);	
	ATT7053_DIN_H;	
	Delay(10);	
	ATT7053_SCLK_L;	
	Delay(10);	
				
	for(i=6; i>=0; i--)
	{
		ATT7053_SCLK_H;		
		Delay(10);	
		if((address & (0x01<<i))>>i)	
		{
			ATT7053_DIN_H;
		}
		else ATT7053_DIN_L;
		Delay(10);	
        ATT7053_SCLK_L;	
		Delay(10);	
	}
						
	Delay(40);	

	for(i=23; i>=0; i--)
	{
		ATT7053_SCLK_H;	
		Delay(10);	
		if((write_data & (temp_data<<i))>>i)	
		{
			ATT7053_DIN_H;
		}
		else ATT7053_DIN_L;
		Delay(10);	
		ATT7053_SCLK_L;	
		Delay(10);	
	}
	ATT7053_CS_H;
}
//
//reset the att7053
void ATT7053_RESET(void)
{
  //SPI_Write(0x33, 0x0055);
  SPI_Write(0x32, 0x00BC); //open write 0x40 --0x45 data reg.
  SPI_Write(0x43, 0xFF);   //I1 I2  U ADC ,TPS BOR Enable
  SPI_Write(0x42, 0x01FF); //I1Angle Enable, I2Angle Enable
  
  SPI_Write(0x32, 0xA6);	//Open written-protect
  SPI_Write(0x59, 0x000C);	//I1*16	
  SPI_Write(0x69, g_sSyscalparaters.leakoffset);	//I1rmsoffset	
  SPI_Write(0x6A, g_sSyscalparaters.curroffset);	//I1rmsoffset	
  
}

