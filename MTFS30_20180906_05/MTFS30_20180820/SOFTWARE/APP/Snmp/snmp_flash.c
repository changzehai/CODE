/*****************************************************************************/
/* �ļ���:    snmp_flash.c                                                   */
/* ˵  ��:    flash��д����                                                  */
/* ��  ��:    2018-06-22 changzehai(DTT)                                     */
/* ��  ��:    ��                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include "flash.h"
#include "string.h"
#include "stm32f4xx_flash.h"
#include "stdlib.h"

/*-----------------------------*/
/* ��������                    */
/*-----------------------------*/
static uint16_t Flash_GetSector( uint32_t Address );
static void Snmp_Flash_WriteSecondarySector( uint32_t primary_address, uint32_t secondary_address, int8_t* data, uint16_t length, uint8_t position, uint16_t byte_position );
static void Snmp_Flash_Write_Byte( uint32_t address, uint16_t length, int8_t* data );
void Snmp_Flash_Read_Byte( uint32_t address, uint16_t length, int8_t* data );
void Snmp_Flash_Read(int8_t* data, uint16_t length, uint8_t position, uint16_t byte_position);
void Snmp_Flash_Write( int8_t* data, uint16_t length, uint8_t position, uint16_t byte_position );
uint32_t Snmp_Flash_GetDataAddr(void);
/****************************************************************************
* ��    ��: Flash_GetSector
* ��    ��: ��ȡ��ַAddress��Ӧ��sector���
* ��ڲ�������ַ
* ���ڲ�����sector���
* �� �� ֵ: ��                                                             
* ��    ��: 2018-06-20 changzehai(DTT)                                     
* ��    ��: ��                                                             
****************************************************************************/
static uint16_t Flash_GetSector( uint32_t Address )
{
    uint16_t sector = 0;
    if( ( Address < ADDR_FLASH_SECTOR_1 ) && ( Address >= ADDR_FLASH_SECTOR_0 ) )
    {
        sector = FLASH_Sector_0;
    }
    else if( ( Address < ADDR_FLASH_SECTOR_2 ) && ( Address >= ADDR_FLASH_SECTOR_1 ) )
    {
        sector = FLASH_Sector_1;
    }
    else if( ( Address < ADDR_FLASH_SECTOR_3 ) && ( Address >= ADDR_FLASH_SECTOR_2 ) )
    {
        sector = FLASH_Sector_2;
    }
    else if( ( Address < ADDR_FLASH_SECTOR_4 ) && ( Address >= ADDR_FLASH_SECTOR_3 ) )
    {
        sector = FLASH_Sector_3;
    }
    else if( ( Address < ADDR_FLASH_SECTOR_5 ) && ( Address >= ADDR_FLASH_SECTOR_4 ) )
    {
        sector = FLASH_Sector_4;
    }
    else if( ( Address < ADDR_FLASH_SECTOR_6 ) && ( Address >= ADDR_FLASH_SECTOR_5 ) )
    {
        sector = FLASH_Sector_5;
    }
    else if( ( Address < ADDR_FLASH_SECTOR_7 ) && ( Address >= ADDR_FLASH_SECTOR_6 ) )
    {
        sector = FLASH_Sector_6;
    }
    else if( ( Address < ADDR_FLASH_SECTOR_8 ) && ( Address >= ADDR_FLASH_SECTOR_7 ) )
    {
        sector = FLASH_Sector_7;
    }
    else if( ( Address < ADDR_FLASH_SECTOR_9 ) && ( Address >= ADDR_FLASH_SECTOR_8 ) )
    {
        sector = FLASH_Sector_8;
    }
    else if( ( Address < ADDR_FLASH_SECTOR_10 ) && ( Address >= ADDR_FLASH_SECTOR_9 ) )
    {
        sector = FLASH_Sector_9;
    }
    else if( ( Address < ADDR_FLASH_SECTOR_11 ) && ( Address >= ADDR_FLASH_SECTOR_10 ) )
    {
        sector = FLASH_Sector_10;
    }
    else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11))*/
    {
        sector = FLASH_Sector_11;
    }
    return sector;
}

/****************************************************************************
* ��    ��: Flash_Get_Sector_Start_Addr
* ��    ��: ��ȡָ��������ʼ��ַ
* ��ڲ�����SectorNum ������
* ���ڲ�������
* �� �� ֵ: ��                                                             
* ��    ��: 2018-06-20 changzehai(DTT)                                     
* ��    ��: ��  
****************************************************************************/
//uint32_t Flash_Get_Sector_Start_Addr(uint16_t sector)
//{
//    uint32_t addr = 0;
//
//
//    switch(sector)
//    {
//    case FLASH_Sector_0:
//         addr = ADDR_FLASH_SECTOR_0;
//        break;
//    case FLASH_Sector_1:
//         addr = ADDR_FLASH_SECTOR_1;
//        break;
//    case FLASH_Sector_2:
//         addr = ADDR_FLASH_SECTOR_2;
//        break;
//    case FLASH_Sector_3:
//         addr = ADDR_FLASH_SECTOR_3;
//        break;
//    case FLASH_Sector_4:
//         addr = ADDR_FLASH_SECTOR_4;
//        break;
//    case FLASH_Sector_5:
//         addr = ADDR_FLASH_SECTOR_5;
//        break;
//    case FLASH_Sector_6:
//         addr = ADDR_FLASH_SECTOR_6;
//        break;
//    case FLASH_Sector_7:
//         addr = ADDR_FLASH_SECTOR_7;
//        break;
//    case FLASH_Sector_8:
//         addr = ADDR_FLASH_SECTOR_8;
//        break;
//    case FLASH_Sector_9:
//         addr = ADDR_FLASH_SECTOR_9;
//        break;
//    case FLASH_Sector_10:
//         addr = ADDR_FLASH_SECTOR_10;
//        break;
//    case FLASH_Sector_11:
//         addr = ADDR_FLASH_SECTOR_11;
//        break;
//    default:
//        break;
//    }
//
//    return addr;
//
//}

/****************************************************************************
* ��    ��: Snmp_Flash_EraseSector
* ��    ��: ����ָ������
* ��ڲ�����SectorNum ������
* ���ڲ�������
* �� �� ֵ: ��                                                             
* ��    ��: 2018-06-20 changzehai(DTT)                                     
* ��    ��: ��  
****************************************************************************/
//static void Snmp_Flash_EraseSector( uint16_t SectorNum )
//{
//    FLASH_Unlock();
//    FLASH_ClearFlag( FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
//                     FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR );
//    if( FLASH_EraseSector( SectorNum, VoltageRange_3 ) != FLASH_COMPLETE ) while( 1 );
//    FLASH_Lock();
//}


/****************************************************************************
* ��    ��: Snmp_Flash_Read
* ��    ��: ����length�ֽ�����
* ��ڲ�����data          ��ָ�����������
*           length        �����ݳ���
*           position      ��Ҫ�����������ڵڼ���4K
*           byte_position : Ҫ������λ������4K�ĵڼ�byte
* ���ڲ�������
* �� �� ֵ: ��                                                             
* ��    ��: 2018-06-20 changzehai(DTT)                                     
* ��    ��: ��  
****************************************************************************/
void Snmp_Flash_Read(int8_t* data, uint16_t length, uint8_t position, uint16_t byte_position)
{
    
    uint32_t address;
    uint16_t Flash_Sector_Flag1 = 0; /* ����Sector��־1 */
    uint16_t Flash_Sector_Flag2 = 0; /* ����Sector��־2 */    
    
    
    
    /* ��ȡ����Sector��־  */
    Flash_Sector_Flag1 = *( uint16_t* )ADDR_FLASH_SECTOR_6;
    Flash_Sector_Flag2 = *( uint16_t* )ADDR_FLASH_SECTOR_7;
    
    /* �ж���sector */
    if (Flash_Sector_Flag1 == 0x8000 && Flash_Sector_Flag2 == 0xFFFF)
    { 
         address = ADDR_FLASH_SECTOR_6; /* SECTOR_6Ϊsector */
    } 
    else if (Flash_Sector_Flag1 == 0xFFFF && Flash_Sector_Flag2 == 0x8000)
    {
         
         address = ADDR_FLASH_SECTOR_7; /* SECTOR_7Ϊsector */
    } 
    else
    {
        while(1);
    
    }
        
    /*  ��ַ����2K��ǰ��2K���ڴ������Sector��־ */
    address = address + 2048;
    
    /* �������ʼ��ַ */
    address = address + position * 4096 + byte_position;

    /* ��ȡ���� */
    Snmp_Flash_Read_Byte( address, length,  data);

        
}

/****************************************************************************
* ��    ��: Snmp_Flash_Write
* ��    ��: д��length�ֽ�����
* ��ڲ�����data          ��Ҫд���IP����ָ��
*           length        �����ݳ���
*           position      ��Ҫд��������ڵڼ���4K
*           byte_position : д�������λ������4K�ĵڼ�byte
* ���ڲ�������
* �� �� ֵ: ��                                                             
* ��    ��: 2018-06-20 changzehai(DTT)                                     
* ��    ��: ��  
****************************************************************************/
void Snmp_Flash_Write( int8_t* data, uint16_t length, uint8_t position, uint16_t byte_position )
{
    uint32_t address;
    uint16_t Flash_Sector_Flag1 = 0; /* ����Sector��־1 */
    uint16_t Flash_Sector_Flag2 = 0; /* ����Sector��־2 */
//    uint16_t i;

    
    
    FLASH_Unlock(); //����FLASH�������FLASH��д���ݡ�
    FLASH_ClearFlag( FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                     FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR );
    
    /* ��ȡ����Sector��־  */
    Flash_Sector_Flag1 = *( uint16_t* )ADDR_FLASH_SECTOR_6;
    Flash_Sector_Flag2 = *( uint16_t* )ADDR_FLASH_SECTOR_7;
 

    if( Flash_Sector_Flag1 == 0x8000 && Flash_Sector_Flag2 == 0xFFFF ) /* SECTOR_6Ϊ��Sector��SECTOR_7Ϊ��Sector */
    {
        /* ������д�뵽SECTOR_7������SECTOR_7����Ϊ��Sector��SECTOR_6����Ϊ��Sector */
        Snmp_Flash_WriteSecondarySector( ADDR_FLASH_SECTOR_6, ADDR_FLASH_SECTOR_7,
                                           data, length, position, byte_position );
    }
    else if( Flash_Sector_Flag1 == 0xFFFF && Flash_Sector_Flag2 == 0x8000 ) /* SECTOR_7Ϊ��Sector��SECTOR_6Ϊ��Sector */
    {
        /* ������д�뵽SECTOR_6������SECTOR_6����Ϊ��Sector��SECTOR_7����Ϊ��Sector */
        Snmp_Flash_WriteSecondarySector( ADDR_FLASH_SECTOR_7, ADDR_FLASH_SECTOR_6,
                                           data, length, position, byte_position );
    }
    else /* ��һ��дflash���߲���ȷ��ʱ��λ���� */
    {
        /* ����SECTOR_6 */
        FLASH_EraseSector(FLASH_Sector_6, VoltageRange_3);
        
        /* ����SECTOR_7 */
        FLASH_EraseSector(FLASH_Sector_7, VoltageRange_3);        
        

       
        /*  ��ַ����2K��ǰ��2K���ڴ������Sector��־ */
        address = ADDR_FLASH_SECTOR_6 + 2048;

        /* д������ */
        Snmp_Flash_Write_Byte(address, length, data);
//        for( i = 0; i < length; i++ )
//        {
//            if( FLASH_ProgramByte( address, data[i] ) == FLASH_COMPLETE )
//            {
//                address++;
//            }
//            else
//            {
//                
//                while( 1 );
//            }
//        }
        
        /* ������Sector��־ */
        if (FLASH_ProgramHalfWord( ADDR_FLASH_SECTOR_6, 0x8000) != FLASH_COMPLETE)
        {
            while(1);
        }             
    }
    
    /* ���� */
    FLASH_Lock();
}


/****************************************************************************
* ��    ��: Snmp_Flash_Write_Secondary_Sector
* ��    ��: ������д��ԭ��Sector������ԭ��Sector��Ϊ��Sector��ԭ��Sector��Ϊ��Sector
* ��ڲ�����primary_address   ��ԭ��������ʼ��ַ
*           secondary_address ��ԭ�η�����ʼ��ַ
*           data              ��Ҫд�������ָ��
*           length            �����ݳ���
*           position      ��Ҫд��������ڵڼ���4K
*           byte_position : д�������λ������4K�ĵڼ�byte
* ���ڲ�������
* �� �� ֵ: ��                                                             
* ��    ��: 2018-06-20 changzehai(DTT)                                     
* ��    ��: ��  
****************************************************************************/
static void Snmp_Flash_WriteSecondarySector
(
    uint32_t     primary_address,
    uint32_t     secondary_address,
    int8_t*      data,
    uint16_t     length,
    uint8_t      position,
    uint16_t     byte_position

)
{
    uint8_t      i;
    uint16_t     sector;
    int8_t*      pBuf;
    uint32_t     pri_address;
    uint32_t     sec_address;
    
    pri_address = primary_address;
    sec_address = secondary_address;
    
    /* ������д������ */
    pBuf = calloc(4096, 1);
    if (pBuf == NULL)
    {
        while(1);
    }
    
       
    /*  ��ַ����2K��ǰ��2K���ڴ������Sector��־ */
    primary_address   += 2048;
    secondary_address += 2048;
    
    /* ��ԭ��Sector��дλ��ǰ�������д����Sector */
    for( i = 0; i < position; i++ )
    {
        /* ��д���������� */
        memset(pBuf, 0x00, 4096);
        
        /* ��ԭ��Sector����4K���� */
        Snmp_Flash_Read_Byte( primary_address, 4096, pBuf );
        
        /* ��������4K����д�뵽��Sector */
        Snmp_Flash_Write_Byte( secondary_address, 4096, pBuf );
        
        /* ��ַ����4K */
        primary_address   += 4096;
        secondary_address += 4096;
    }
    
    /* ��д���������� */
    memset(pBuf, 0x00, 4096);
    /* ����Ҫ�޸ĵ�IP����4K������ */
    Snmp_Flash_Read_Byte( primary_address, 4096, pBuf );

    
    /* �޸Ķ�ӦIP */
    memcpy( &pBuf[byte_position], data, length );
//    char *ip = &pBuf[0];
//    uint16_t j;
//    for (j = 0; j < 256; j++)
//    {
//        ip = ip + 16;
//    }
    
    /* ���Ž���4K����д����Sector�� */
    Snmp_Flash_Write_Byte( secondary_address, 4096, pBuf );
    

    /* ��ȡԭ��Sector��� */
    sector = Flash_GetSector( pri_address );
    
    /* ����ԭ��Sector */
    FLASH_EraseSector(sector, VoltageRange_3);
    
    
    /* ������Sector��־������Sector��Ϊ��Sector */
    if (FLASH_ProgramHalfWord( sec_address, 0x8000) != FLASH_COMPLETE)
    {
        while(1);
    }
        
    /* �ͷŶ�д������ */
    free(pBuf);
    
}

/****************************************************************************
* ��    ��: Snmp_Flash_Write_Byte
* ��    ��: д��length�ֽڵ�����
* ��ڲ�����address����ַ
*           length�� ���ݳ���
*           data_8  ָ��д�������
* ���ڲ�������
* �� �� ֵ: ��                                                             
* ��    ��: 2018-06-20 changzehai(DTT)                                     
* ��    ��: ��  
****************************************************************************/
static void Snmp_Flash_Write_Byte( uint32_t address, uint16_t length, int8_t* data )
{
    uint16_t i;

        
    for( i = 0; i < length; i++ )
    {
        
        if( FLASH_ProgramByte( address, data[i] ) == FLASH_COMPLETE )
        {
            address++;
        }
        else
        {
            while( 1 );
        }
    }
}

/****************************************************************************
* ��    ��: Snmp_Flash_Read_Byte
* ��    ��: ���ֽ�Ϊ��λ��ȡlength�ֽڵ�����
* ��ڲ�����address����ַ
*           length�� ���ݳ���
*           data_8  ָ�����������
* ���ڲ�������
* �� �� ֵ: ��                                                             
* ��    ��: 2018-06-20 changzehai(DTT)                                     
* ��    ��: ��  
****************************************************************************/
void Snmp_Flash_Read_Byte( uint32_t address, uint16_t length, int8_t* data )
{
    uint16_t i;
    
    
    for( i = 0; i < length; i++ )
    {
        data[i] = *( __IO int8_t* )address;
        address++;
    }
}

/****************************************************************************
* ��    ��: Snmp_Flash_IS_Writed_Check
* ��    ��: ���flash�Ƿ��Ѿ�д��
* ��ڲ�����address����ַ
*           length�� ���ݳ���
*           data_8  ָ�����������
* ���ڲ�������
* �� �� ֵ: ��                                                             
* ��    ��: 2018-06-20 changzehai(DTT)                                     
* ��    ��: ��  
****************************************************************************/
uint8_t Snmp_Flash_IS_Writed_Check()
{
    uint16_t Flash_Sector_Flag1 = 0; /* ����Sector��־1 */
    uint16_t Flash_Sector_Flag2 = 0; /* ����Sector��־2 */    
    
    
    
    /* ��ȡ����Sector��־  */
    Flash_Sector_Flag1 = *( uint16_t* )ADDR_FLASH_SECTOR_6;
    Flash_Sector_Flag2 = *( uint16_t* )ADDR_FLASH_SECTOR_7;
    
    
    if ((Flash_Sector_Flag1 == 0xFFFF) && (Flash_Sector_Flag2 == 0xFFFF)) /* δд�� */
    {
        return 0;
    } else /* ��д�� */
    {
        return 1;
    }
}

///****************************************************************************
//* ��    ��: Snmp_Flash_GetDataAddr
//* ��    ��: ��ȡFlash���ݱ���λ��
//* ��ڲ�������
//* ���ڲ�������
//* �� �� ֵ: ��Sector���׵�ַ                                                             
//* ��    ��: 2018-06-20 changzehai(DTT)                                     
//* ��    ��: ��  
//****************************************************************************/
//uint32_t Snmp_Flash_GetDataAddr(void)
//{
//
//    uint16_t Flash_Sector_Flag1 = 0; /* ����Sector��־1 */
//    uint16_t Flash_Sector_Flag2 = 0; /* ����Sector��־2 */
//    
//    
//    /* ��ȡ����Sector��־  */
//    Flash_Sector_Flag1 = *( uint16_t* )ADDR_FLASH_SECTOR_6;
//    Flash_Sector_Flag2 = *( uint16_t* )ADDR_FLASH_SECTOR_7;
//    
//    if ( Flash_Sector_Flag1 == 0x8000 && Flash_Sector_Flag2 == 0xFFFF ) /* SECTOR_6Ϊ��Sector��SECTOR_7Ϊ��Sector */
//    {
//        return ADDR_FLASH_SECTOR_6 + 2048;
//    }
//    else if ( Flash_Sector_Flag1 == 0xFFFF && Flash_Sector_Flag2 == 0x8000 ) /* SECTOR_6Ϊ��Sector��SECTOR_7Ϊ��Sector */
//    {
//        return ADDR_FLASH_SECTOR_7 + 2048;
//    }
//    else
//    {
//        return 0;
//    }
//
//}