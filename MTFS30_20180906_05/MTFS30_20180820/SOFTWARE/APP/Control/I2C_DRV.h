#ifndef __I2C_Driver_H
#define __I2C_Driver_H

/* Includes ------------------------------------------------------------------*/



#define SCL_H         GPIOB->BSRRL  = GPIO_Pin_6
#define SCL_L         GPIOB->BSRRH  = GPIO_Pin_6 
   
#define SDA_H         GPIOB->BSRRL = GPIO_Pin_9
#define SDA_L         GPIOB->BSRRH  = GPIO_Pin_9

#define SCL_read      GPIOB->IDR  & GPIO_Pin_6
#define SDA_read      GPIOB->IDR  & GPIO_Pin_9

//#define KEY_read      GPIOF->IDR  & GPIO_PIN_9

#define I2C_PageSize  8  //24C02Ã¿Ò³8×Ö½Ú

void I2C_GPIO_Config(void);
//bool I2C_WriteByte(u8 SendByte, u16 WriteAddress, u8 DeviceAddress);
tBoolean I2C_BufferWrite(u8_t* pBuffer, u8_t length,     u16_t WriteAddress, u8_t DeviceAddress);
void I2C_PageWrite(  u8_t* pBuffer, u8_t length,     u16_t WriteAddress, u8_t DeviceAddress);
tBoolean I2C_ReadByte(u8_t* pBuffer,   u8_t length,     u16_t ReadAddress,  u8_t DeviceAddress);

#endif 



