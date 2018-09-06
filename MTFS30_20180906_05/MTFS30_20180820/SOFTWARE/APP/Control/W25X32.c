//*****************************************************************************
//
// ssiflash.c - Driver for the Winbond Serial Flash on the development board.
//
// Copyright (c) 2009-2011 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 7611 of the DK-LM3S9B96 Firmware Package.
//
//*****************************************************************************
#include <string.h>
#include <includes.h>
#include "arch/cc.h"
#include "config.h"
#include "W25X32.h"
#define W25X16A_SECTOR_SIZE   4096
#define SPI_FLASH_CS_HIGH()   GPIOC->BSRRL  = GPIO_Pin_6//GPIOD->BSRRH  = GPIO_PIN_0GPIO_PIN_10
#define SPI_FLASH_CS_LOW()    GPIOC->BSRRH  = GPIO_Pin_6


/* Private typedef -----------------------------------------------------------*/

#define W25X16A_PAGE_SIZE        256
#define SPI_FLASH_PerWritePageSize     256
#define SPI_FLASH_PageSize      256

/* Private define ------------------------------------------------------------*/
#define W25X_WriteEnable		      0x06 
#define W25X_WriteDisable		      0x04 
#define W25X_ReadStatusReg		    0x05 
#define W25X_WriteStatusReg		    0x01 
#define W25X_ReadData			        0x03 
#define W25X_FastReadData		      0x0B 
#define W25X_FastReadDual		      0x3B 
#define W25X_PageProgram		      0x02 
#define W25X_BlockErase			      0xD8 
#define W25X_SectorErase		      0x20 
#define W25X_ChipErase			      0xC7 
#define W25X_PowerDown			      0xB9 
#define W25X_ReleasePowerDown	    0xAB 
#define W25X_DeviceID			        0xAB 
#define W25X_ManufactDeviceID   	0x90 
#define W25X_JedecDeviceID		    0x9F 

#define WIP_Flag                  0x01  /* Write In Progress (WIP) flag */

#define Dummy_Byte                0xFF


void SPI_FLASH_WaitForWriteEnd(void);
uint32_t SPI_FLASH_ReadID(void);
void SPI_FLASH_WriteEnable(void);

void SPI_FLASH_SendByte(uint8_t byte);
uint8_t SPI_FLASH_ReadByte(void);
uint32_t SPI_FLASH_ReadDeviceID(void);

extern tTimeParameters  CurrentTime;

extern unsigned char LOGWARNING_NUM[2];
unsigned char Log_Buffer[2048]={0};
unsigned char Log_Buffer1[2048]={0};

uint32_t DeviceID;
/*******************************************************************************
* Function Name  : SPI_FLASH_Init
* Description    : Initializes the peripherals used by the SPI FLASH driver.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_Flash_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; 
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC, ENABLE);
      
  // PC10 ---SPI SCK    PC12 ---SPI MODI
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10|GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
  // RM_CS PC6
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOC,GPIO_Pin_6,Bit_SET);
  // SPI_CS 
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOC,GPIO_Pin_7,Bit_SET);
  // SPI_MISO PC11
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    
    GPIO_Init(GPIOC, &GPIO_InitStructure);   
    SPI_FLASH_CS_HIGH();	
	
	/* Get SPI Flash Device ID */
    DeviceID = SPI_FLASH_ReadDeviceID();
	
    if(DEVICE_ID_W25X16A != DeviceID)
    {
		  while(1);// W25X32 Hard error
    }
}
/*******************************************************************************
* Function Name  : SPI_FLASH_SectorErase
* Description    : Erases the specified FLASH sector.
* Input          : SectorAddr: address of the sector to erase.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_SectorErase(uint32_t SectorAddr)
{
  /* Send write enable instruction */
  SPI_FLASH_WriteEnable();
  SPI_FLASH_WaitForWriteEnd();
  /* Sector Erase */
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();
  /* Send Sector Erase instruction */
  SPI_FLASH_SendByte(W25X_SectorErase);
  /* Send SectorAddr high nibble address byte */
  SPI_FLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
  /* Send SectorAddr medium nibble address byte */
  SPI_FLASH_SendByte((SectorAddr & 0xFF00) >> 8);
  /* Send SectorAddr low nibble address byte */
  SPI_FLASH_SendByte(SectorAddr & 0xFF);
  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
  /* Wait the end of Flash writing */
  SPI_FLASH_WaitForWriteEnd();
}

/*******************************************************************************
* Function Name  : SPI_FLASH_BulkErase
* Description    : Erases the entire FLASH.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_BulkErase(void)
{
  /* Send write enable instruction */
  SPI_FLASH_WriteEnable();

  /* Bulk Erase */
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();
  /* Send Bulk Erase instruction  */
  SPI_FLASH_SendByte(W25X_ChipErase);
  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();

  /* Wait the end of Flash writing */
  SPI_FLASH_WaitForWriteEnd();
}

/*******************************************************************************
* Function Name  : SPI_FLASH_PageWrite
* Description    : Writes more than one byte to the FLASH with a single WRITE
*                  cycle(Page WRITE sequence). The number of byte can't exceed
*                  the FLASH page size.
* Input          : - pBuffer : pointer to the buffer  containing the data to be
*                    written to the FLASH.
*                  - WriteAddr : FLASH's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the FLASH,
*                    must be equal or less than "SPI_FLASH_PageSize" value.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  /* Enable the write access to the FLASH */
  SPI_FLASH_WriteEnable();

  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();
  /* Send "Write to Memory " instruction */
  SPI_FLASH_SendByte(W25X_PageProgram);
  /* Send WriteAddr high nibble address byte to write to */
  SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
  /* Send WriteAddr medium nibble address byte to write to */
  SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);
  /* Send WriteAddr low nibble address byte to write to */
  SPI_FLASH_SendByte(WriteAddr & 0xFF);

  if(NumByteToWrite > SPI_FLASH_PerWritePageSize)
  {
     NumByteToWrite = SPI_FLASH_PerWritePageSize;
     //printf("\n\r Err: SPI_FLASH_PageWrite too large!");
  }

  /* while there is data to be written on the FLASH */
  while (NumByteToWrite--)
  {
    /* Send the current byte */
    SPI_FLASH_SendByte(*pBuffer);
    /* Point on the next byte to be written */
    pBuffer++;
  }

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();

  /* Wait the end of Flash writing */
  SPI_FLASH_WaitForWriteEnd();
}

/*******************************************************************************
* Function Name  : SPI_FLASH_BufferWrite
* Description    : Writes block of data to the FLASH. In this function, the
*                  number of WRITE cycles are reduced, using Page WRITE sequence.
* Input          : - pBuffer : pointer to the buffer  containing the data to be
*                    written to the FLASH.
*                  - WriteAddr : FLASH's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the FLASH.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % SPI_FLASH_PageSize;
  count = SPI_FLASH_PageSize - Addr;
  NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
  NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

  if (Addr == 0) /* WriteAddr is SPI_FLASH_PageSize aligned  */
  {
    if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
    {
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }

      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
    }
  }
  else /* WriteAddr is not SPI_FLASH_PageSize aligned  */
  {
    if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
    {
      if (NumOfSingle > count) /* (NumByteToWrite + WriteAddr) > SPI_FLASH_PageSize */
      {
        temp = NumOfSingle - count;

        SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;

        SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
      }
      else
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
      NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

      SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;

      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }

      if (NumOfSingle != 0)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}

/*******************************************************************************
* Function Name  : SPI_FLASH_BufferRead
* Description    : Reads a block of data from the FLASH.
* Input          : - pBuffer : pointer to the buffer that receives the data read
*                    from the FLASH.
*                  - ReadAddr : FLASH's internal address to read from.
*                  - NumByteToRead : number of bytes to read from the FLASH.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Read from Memory " instruction */
  SPI_FLASH_SendByte(W25X_ReadData);

  /* Send ReadAddr high nibble address byte to read from */
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  /* Send ReadAddr medium nibble address byte to read from */
  SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  /* Send ReadAddr low nibble address byte to read from */
  SPI_FLASH_SendByte(ReadAddr & 0xFF);

  while (NumByteToRead--) /* while there is data to be read */
  {

    //SPI_FLASH_SendByte(Dummy_Byte);
    *pBuffer = SPI_FLASH_ReadByte();//??SPI ??

    /* Point to the next location where the byte read will be saved */
    pBuffer++;
  }

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
}

/*******************************************************************************
* Function Name  : SPI_FLASH_ReadID
* Description    : Reads FLASH identification.
* Input          : None
* Output         : None
* Return         : FLASH identification
*******************************************************************************/
uint32_t SPI_FLASH_ReadID(void)
{
  uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "RDID " instruction */
  SPI_FLASH_SendByte(W25X_JedecDeviceID);

  /* Read a byte from the FLASH */
  //SPI_FLASH_SendByte(Dummy_Byte);
    Temp0 = SPI_FLASH_ReadByte();//??SPI ??
  
  /* Read a byte from the FLASH */
    //SPI_FLASH_SendByte(Dummy_Byte);
    Temp1 = SPI_FLASH_ReadByte();//??SPI ??
    
  /* Read a byte from the FLASH */
    //SPI_FLASH_SendByte(Dummy_Byte);
    Temp2 = SPI_FLASH_ReadByte();//??SPI ??


  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();

  Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

  return Temp;
}
/*******************************************************************************
* Function Name  : SPI_FLASH_ReadID
* Description    : Reads FLASH identification.
* Input          : None
* Output         : None
* Return         : FLASH identification
*******************************************************************************/
uint32_t SPI_FLASH_ReadDeviceID(void)
{
  uint32_t Temp = 0;

  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "RDID " instruction */
  SPI_FLASH_SendByte(W25X_DeviceID);
  SPI_FLASH_SendByte(Dummy_Byte);
  SPI_FLASH_SendByte(Dummy_Byte);
  SPI_FLASH_SendByte(Dummy_Byte);

  //SPI_FLASH_SendByte(Dummy_Byte);
  Temp = SPI_FLASH_ReadByte();
  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();

  return Temp;
}
/*******************************************************************************
* Function Name  : SPI_FLASH_StartReadSequence
* Description    : Initiates a read data byte (READ) sequence from the Flash.
*                  This is done by driving the /CS line low to select the device,
*                  then the READ instruction is transmitted followed by 3 bytes
*                  address. This function exit and keep the /CS line low, so the
*                  Flash still being selected. With this technique the whole
*                  content of the Flash is read with a single READ instruction.
* Input          : - ReadAddr : FLASH's internal address to read from.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_StartReadSequence(uint32_t ReadAddr)
{
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Read from Memory " instruction */
  SPI_FLASH_SendByte(W25X_ReadData);

  /* Send the 24-bit address of the address to read from -----------------------*/
  /* Send ReadAddr high nibble address byte */
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  /* Send ReadAddr medium nibble address byte */
  SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  /* Send ReadAddr low nibble address byte */
  SPI_FLASH_SendByte(ReadAddr & 0xFF);
}

/******************************************************************
            SPI_FLASH_SendByte
 ******************************************************************/

void SPI_FLASH_SendByte(uint8_t byte)
{
      unsigned char i;

    for(i=0; i<8; i++)
    {
        if(byte & 0x80)//((byte & 0x80)== 0x80)
        {
         SPI_FLASH_MOSI_S_1;
        }
        else
        {
            SPI_FLASH_MOSI_S_0;
        }
            SPI_FLASH_SCK_S_1;     //toggle clock high
            byte <<= 1;
          
            __NOP();__NOP();__NOP();__NOP();
            SPI_FLASH_SCK_S_0;
    }
    
}

/******************************************************************
              SPI_FLASH_ReadByte
 ******************************************************************/

uint8_t SPI_FLASH_ReadByte(void) //SPI?????
{ 
  uint8_t i;
    uint8_t byte;
   
    byte=0;
    for (i=0; i<8; i++)
    {
        byte <<= 1;
        SPI_FLASH_SCK_S_1;

        if(SPI_FLASH_MISO_S_G)
        {
            byte |= 1;	//Wait SDO to go Hi
        }
        SPI_FLASH_SCK_S_0;
    }
    
    return byte;

}


/*******************************************************************************
* Function Name  : SPI_FLASH_WriteEnable
* Description    : Enables the write access to the FLASH.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_WriteEnable(void)
{
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Write Enable" instruction */
  SPI_FLASH_SendByte(W25X_WriteEnable);

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
}

/*******************************************************************************
* Function Name  : SPI_FLASH_WaitForWriteEnd
* Description    : Polls the status of the Write In Progress (WIP) flag in the
*                  FLASH's status  register  and  loop  until write  opertaion
*                  has completed.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_WaitForWriteEnd(void)
{
  uint8_t FLASH_Status = 0;

  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Read Status Register" instruction */
  SPI_FLASH_SendByte(W25X_ReadStatusReg);

  /* Loop as long as the memory is busy with a write cycle */
  do
  {

    //SPI_FLASH_SendByte(Dummy_Byte);
    FLASH_Status = SPI_FLASH_ReadByte();//??SPI ??

  }
  while ((FLASH_Status & WIP_Flag) == SET); /* Write in progress */

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
}


//SPI_Flash_PowerDown
void SPI_Flash_PowerDown(void)   
{ 
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Power Down" instruction */
  SPI_FLASH_SendByte(W25X_PowerDown);

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
}   

//SPI_Flash_WAKEUP
void SPI_Flash_WAKEUP(void)   
{
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Power Down" instruction */
  SPI_FLASH_SendByte(W25X_ReleasePowerDown);

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();                  
}   

/*********************************************************************************************************
**  End Of File
*********************************************************************************************************/











////*****************************************************************************
////
////! Writes a block of log data to the serial flash device.
////!
////! \index index .
////! \ulAddress ulLength is the number of bytes of data to write.
//
////!
////! This function writes a block of data into the serial flash device at the
////! given address.  It is assumed that the area to be written has previously
////! been erased.
////!
////! \return Returns the number of bytes written.
////
//void log_ssiflash_write(unsigned char index)
//{
//	  unsigned int  loglen;
//	  unsigned char lognum[2];
//	  char LogWriteBuf[logarr_len];
//
//	  if((index>0)&&(index<=max_logrecord_len))//存储条目必须大于0，小于等于50
//    {
//							lognum[0]=index;               //存储条目,分两页当一次存储大于
//							lognum[1]=LOGWARNING_NUM[1];
//							
//						  if((LOGWARNING_NUM[1]==max_logrecord_len)&&(index==1))
//							{
//								 memset(Log_Buffer1,0,2048);
//								 
//								 loglen=logarr_len*max_logrecord_len;
//								 
//								 strncpy((char *)Log_Buffer1,(char *)Log_Buffer,loglen);
//								 
//								 memset(Log_Buffer,0,2048);
//								 
//								 SPI_FLASH_SectorErase(SSIFLASH_ADDR+(unsigned long)(2*W25X16A_SECTOR_SIZE));//SPI_FLASH_SectorErase
//								 SPI_FLASH_BufferWrite(Log_Buffer1,SSIFLASH_ADDR+(unsigned long)(2*W25X16A_SECTOR_SIZE),loglen);
//
//							}
//													
//							
//							
//
//							memset(LogWriteBuf,0,logarr_len);
//									
//							sprintf(LogWriteBuf,"<li>%04d-%02d-%02d %02d:%02d:%02d time out!",CurrentTime.year,
//																	 CurrentTime.month,
//																	 CurrentTime.day,
//																	 CurrentTime.hour,
//																	 CurrentTime.minute,
//																	 CurrentTime.second); //log warnning string
//							 strcat((char*)Log_Buffer,(char*)LogWriteBuf);
//							
//							 loglen=(unsigned int)logarr_len*index;		
//							 SPI_FLASH_SectorErase((unsigned long)(SSIFLASH_ADDR+W25X16A_SECTOR_SIZE));//W25X16A_SECTOR_SIZE
//							 SPI_FLASH_BufferWrite(Log_Buffer,(unsigned long)(SSIFLASH_ADDR+W25X16A_SECTOR_SIZE),loglen);//log warnning into ssiflash
//									
//							 SPI_FLASH_SectorErase(SSIFLASH_ADDR); //擦除第一个4K区
//							 SPI_FLASH_BufferWrite(lognum,SSIFLASH_ADDR,2);//存储条目和显示条目数			
//}	
//    else
//    {
//							LOGWARNING_NUM[0]=0;
//							LOGWARNING_NUM[1]=0;
//							memset(Log_Buffer,0,2048);
//							memset(Log_Buffer1,0,2048);
//    }	    			
//}
////*****************************************************************************
////
////! read a block of log data to the serial flash device.
////!
////! \index index .
////! \ulAddress ulLength is the number of bytes of data to write.
//
////!
////! This function writes a block of data into the serial flash device at the
////! given address.  It is assumed that the area to be written has previously
////! been erased.
////!
////! \return Returns the number of bytes written.
////
//
//unsigned char lognum_ssiflash_read(void)//读出LOG字符串到目的字符数组中
//{   
//	  
//	  unsigned int LogNum=0;
//	  
//	  
//	  SPI_FLASH_BufferRead(LOGWARNING_NUM,SSIFLASH_ADDR,2);//读出存储条目和显示条目数
//	
//						//故障出错
//	  if((LOGWARNING_NUM[0]==0)||(LOGWARNING_NUM[0]>max_logrecord_len)||((LOGWARNING_NUM[1]!=0)&&(LOGWARNING_NUM[1]!=max_logrecord_len)))
//		{
//					LOGWARNING_NUM[0]=0;
//					LOGWARNING_NUM[1]=0;
//					LogNum=0;
//			    memset(Log_Buffer,0,2048);
//					memset(Log_Buffer1,0,2048);
//    }
//		else    //读出整个数组
//		{
//			    LogNum=(unsigned int)(logarr_len*LOGWARNING_NUM[0]);
//			    SPI_FLASH_BufferRead(Log_Buffer,(unsigned long)(SSIFLASH_ADDR+W25X16A_SECTOR_SIZE),LogNum);
//          if(LOGWARNING_NUM[1]==max_logrecord_len)
//          {
//						    LogNum=(unsigned int)(logarr_len*LOGWARNING_NUM[1]); 
//						    SPI_FLASH_BufferRead(Log_Buffer1,SSIFLASH_ADDR+(unsigned long)(2*W25X16A_SECTOR_SIZE),LogNum);
//          }						
//			
//    }
//	  
//    return LogNum;
//}
//
//
////*****************************************************************************
////*****************************************************************************
////
//// Close the Doxygen group.
////! @}
////
////*****************************************************************************


