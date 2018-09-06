//*****************************************************************************
//
// ssiflash.h - Header file for the Winbond Serial Flash driver for the
//              development boards.
//
// Copyright (c) 2008-2011 Texas Instruments Incorporated.  All rights reserved.
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

#ifndef __SSIFLASH_H__
#define __SSIFLASH_H__

#define SPI_FLASH_MOSI_S_1   {GPIOC->BSRRL = GPIO_Pin_12;}
#define SPI_FLASH_MOSI_S_0   {GPIOC->BSRRH = GPIO_Pin_12;}
#define SPI_FLASH_SCK_S_1    {GPIOC->BSRRL = GPIO_Pin_10;}
#define SPI_FLASH_SCK_S_0    {GPIOC->BSRRH = GPIO_Pin_10;}  
#define SPI_FLASH_MISO_S_G   (GPIOC->IDR & GPIO_Pin_11)

#define DEVICE_ID_W25X16A         0x15
#define SSIFLASH_ADDR             0x001000
#define SSICONFIG_ADDR            0x000000
#define logarr_len                33
#define max_logrecord_len         50
//*****************************************************************************
//
// Exported function prototypes.
//
//*****************************************************************************
void SPI_Flash_Init(void);
void log_ssiflash_write(unsigned char index);
unsigned char lognum_ssiflash_read(void);
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
void SPI_FLASH_SectorErase(uint32_t SectorAddr);
void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);


#endif // __SSIFLASH_H__
