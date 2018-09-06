#include "flash.h"

/****************************************************************************
* ��    ��: ��ȡ��ַAddress��Ӧ��sector���
* ��ڲ�������ַ
* ���ڲ�����sector���
* ˵    ������
* ���÷�������
****************************************************************************/
uint16_t Flash_GetSector( uint32_t Address )
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
* ��    ��: ��ȡָ��������ʼ��ַ
* ��ڲ�����SectorNum ������
* ���ڲ�������
* ˵    ������
* ���÷�������
****************************************************************************/
uint32_t Flash_Get_Sector_Start_Addr(uint16_t sector)
{
    uint32_t addr = 0;
    
 
    switch(sector)
    {
    case FLASH_Sector_0:  
         addr = ADDR_FLASH_SECTOR_0;
        break;
    case FLASH_Sector_1:
         addr = ADDR_FLASH_SECTOR_1;
        break;
    case FLASH_Sector_2:
         addr = ADDR_FLASH_SECTOR_2;
        break;
    case FLASH_Sector_3:
         addr = ADDR_FLASH_SECTOR_3;
        break;
    case FLASH_Sector_4:
         addr = ADDR_FLASH_SECTOR_4;
        break;
    case FLASH_Sector_5:
         addr = ADDR_FLASH_SECTOR_5;
        break;
    case FLASH_Sector_6:
         addr = ADDR_FLASH_SECTOR_6;
        break;
    case FLASH_Sector_7:
         addr = ADDR_FLASH_SECTOR_7;
        break;
    case FLASH_Sector_8:
         addr = ADDR_FLASH_SECTOR_8;
        break;
    case FLASH_Sector_9:
         addr = ADDR_FLASH_SECTOR_9;
        break;
    case FLASH_Sector_10:
         addr = ADDR_FLASH_SECTOR_10;
        break;
    case FLASH_Sector_11:
         addr = ADDR_FLASH_SECTOR_11;
        break;
    default:
        break;
    }
    
    return addr;

}

/****************************************************************************
* ��    ��: ����ָ������
* ��ڲ�����SectorNum ������
* ���ڲ�������
* ˵    ������
* ���÷�������
****************************************************************************/
void Flash_EraseSector( uint16_t SectorNum )
{
    FLASH_Unlock();
    FLASH_ClearFlag( FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                     FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR );
    if( FLASH_EraseSector( SectorNum, VoltageRange_3 ) != FLASH_COMPLETE ) while( 1 );
    FLASH_Lock();
}

/****************************************************************************
* ��    ��: д�볤��Ϊlength��32λ����
* ��ڲ�����address����ַ
            length�� ���ݳ���
            data_32��Ҫд�������ָ��
* ���ڲ�������
* ˵    ������
* ���÷�������
****************************************************************************/
void Flash_Write32BitDatas( uint32_t address, uint16_t length, int32_t* data_32 )
{
    uint16_t StartSector, EndSector, i;
    FLASH_Unlock(); //����FLASH�������FLASH��д���ݡ�
    FLASH_ClearFlag( FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                     FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR );
    StartSector = Flash_GetSector( address ); //��ȡFLASH��Sector���
    EndSector = Flash_GetSector( address + 4 * length );
    for( i = StartSector; i < EndSector; i += 8 ) //ÿ��FLASH�������8���ɲο��ϱ�FLASH Sector�Ķ��塣
    {
        if( FLASH_EraseSector( i, VoltageRange_3 ) != FLASH_COMPLETE ) while( 1 );
    }
    for( i = 0; i < length; i++ )
    {
        if( FLASH_ProgramWord( address, data_32[i] ) == FLASH_COMPLETE ) //��DATA_32д����Ӧ�ĵ�ַ��
        {
            address = address + 4;
        }
        else
        {
            while( 1 );
        }
    }
    FLASH_Lock();  //��FLASH����ҪFLASH���ڽ���״̬��
}

/****************************************************************************
* ��    ��: ��ȡ����Ϊlength��32λ����
* ��ڲ�����address����ַ
            length�� ���ݳ���
            data_32  ָ�����������
* ���ڲ�������
* ˵    ������
* ���÷�������
****************************************************************************/
void Flash_Read32BitDatas( uint32_t address, uint16_t length, int32_t* data_32 )
{
    uint8_t i;
    for( i = 0; i < length; i++ )
    {
        data_32[i] = *( __IO int32_t* )address;
        address = address + 4;
    }
}

/****************************************************************************
* ��    ��: д�볤��Ϊlength��16λ����
* ��ڲ�����address����ַ
            length�� ���ݳ���
            data_16��Ҫд�������ָ��
* ���ڲ�������
* ˵    ������
* ���÷�������
****************************************************************************/
void Flash_Write16BitDatas( uint32_t address, uint16_t length, int16_t* data_16 )
{
    uint16_t StartSector, EndSector, i;
    FLASH_Unlock(); //����FLASH�������FLASH��д���ݡ�
    FLASH_ClearFlag( FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                     FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR );
    StartSector = Flash_GetSector( address ); //��ȡFLASH��Sector���
    EndSector = Flash_GetSector( address + 2 * length );
    for( i = StartSector; i < EndSector; i += 8 ) //ÿ��FLASH�������8���ɲο��ϱ�FLASH Sector�Ķ��塣
    {
        if( FLASH_EraseSector( i, VoltageRange_3 ) != FLASH_COMPLETE ) while( 1 );
    }
    for( i = 0; i < length; i++ )
    {
        if( FLASH_ProgramHalfWord( address, data_16[i] ) == FLASH_COMPLETE )
        {
            address = address + 2;
        }
        else
        {            while( 1 );
        }
    }
    FLASH_Lock();  //��FLASH����ҪFLASH���ڽ���״̬��
}

/****************************************************************************
* ��    ��: ��ȡ����Ϊlength��16λ����
* ��ڲ�����address����ַ
            length�� ���ݳ���
            data_16  ָ�����������
* ���ڲ�������
* ˵    ������
* ���÷�������
****************************************************************************/
void Flash_Read16BitDatas( uint32_t address, uint16_t length, int16_t* data_16 )
{
    uint8_t i;
    for( i = 0; i < length; i++ )
    {
        data_16[i] = *( __IO int16_t* )address;
        address = address + 2;
    }
}

/****************************************************************************
* ��    ��: д��length�ֽ�����
* ��ڲ�����address����ַ
*           length�� ���ݳ���
*           data_8��Ҫд�������ָ��
* ���ڲ�������
* ˵    ������
* ���÷�������
****************************************************************************/
uint8_t Snmp_Flash_Write_Byte(int8_t* data_8, uint16_t length)
{
    uint16_t StartSector, EndSector, i;
    uint32_t Current;
    uint32_t Flash_Primary_Addr;
    uint32_t Flash_Secondary_Addr;
    uint8_t  Flash_Use_Flag1;
    uint8_t  Flash_Use_Flag2;
    AnalogPropertyTable *data;
    uint8_t  index;
    uint8_t  ipIndex;
    uint8_t  ipLength;
    uint8_t i;
    char ip[256][16];
    uint32_t address;
    
    uint16_t sector;
    
    
    FLASH_Unlock(); //����FLASH�������FLASH��д���ݡ�
    FLASH_ClearFlag( FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                     FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR );
    
    
    /* ʹ�ñ�ʶ  */
    Flash_Use_Flag1 = *(uint8_t *)ADDR_FLASH_SECTOR_6;
    Flash_Use_Flag2 = *(uint8_t *)ADDR_FLASH_SECTOR_7;
    
    if (Flash_Use_Flag1 != 0x80 && Flash_Use_Flag1 != 0x01)
    {
        /* ��ȡFLASH��Sector��� */
        sector = Flash_GetSector( ADDR_FLASH_SECTOR_6 );
        /* ������Sector��� */
        FLASH_EraseSector(sector, VoltageRange_3); 
        
        /* ����IPָʾ����*/
        data =  (AnalogPropertyTable *)data_8;
        index = data-> analogIpIndication / 256; /* �ڼ���4K */
        ipIndex = data-> analogIpIndication % 256;     /* ��4K���еĵڼ���IP */
        
        address = ADDR_FLASH_SECTOR_6;
        
        ipLength = length /sizeof(AnalogPropertyTable) * 16;
               
        for (i = 0; i < ipLength; i++)
        {
            if ( FLASH_ProgramByte( address, data->ip)== FLASH_COMPLETE)
            {
               address++;  
            }
            else
            {
                while(1);
            }
        }
      
        *(uint8_t *)ADDR_FLASH_SECTOR_6 = 0x80;
        *(uint8_t *)ADDR_FLASH_SECTOR_7 = 0x01;
         
        
    } 
    else if(Flash_Use_Flag1 == 0x80 && Flash_Use_Flag2 == 0x01)
    {
        
        /* ��ȡFLASH��Sector��� */
        sector = Flash_GetSector( ADDR_FLASH_SECTOR_7 );
        /* ������Sector��� */
        FLASH_EraseSector(sector, VoltageRange_3);         
        
        /* ����IPָʾ����*/
        data =  (AnalogPropertyTable *)data_8;
        index = data-> analogIpIndication / 256; /* �ڼ���4K */
        ipIndex = data-> analogIpIndication % 256;     /* ��4K���еĵڼ���IP */

        address = ADDR_FLASH_SECTOR_7;
         
        /* ��ǰ������ݱ��浽��Sector */
        for (i = 0; i < index; i++)
        {
            Flash_Read_Byte(address, 4096, ip);
            for (i = 0; i < 4096; i++)
            {
                if ( FLASH_ProgramByte( address, ip)== FLASH_COMPLETE)
                {
                   address++;  
                }
                else
                {
                    while(1);
                }
            }
            
        }
        
        
        /* �޸�IP */
        Flash_Read_Byte(address, 4096, ip);
        for(i = 0; i < length; i++)
        
        
        
        
        
        
        
    
    }
    else if(Flash_Use_Flag1 == 0 && Flash_Use_Flag2 == 1)
    {
        
    
    }
    else
    {
    
    }
    
    

    FLASH_Lock();  //��FLASH����ҪFLASH���ڽ���״̬��
    
    return SNMP_OK;
}


/****************************************************************************
* ��    ��: д��length�ֽ�����
* ��ڲ�����data��Ҫд���IP����ָ��
*           length�� ���ݳ���
*           index�� Ҫд��ĵ�һ��IP�ڵڼ���4K
*           Indication: д��ĵ�һ��IPΪ����4K��256��IP�еĵڼ���IP
* ���ڲ�������
* ˵    ������
* ���÷�������
****************************************************************************/
uint8_t Snmp_Flash_Write(int8_t* data_8, uint16_t length, uint8_t index, uint8_t Indication)
{

    char ip[256][16];
    uint32_t address;
    
    uint16_t sector;
    
    
    FLASH_Unlock(); //����FLASH�������FLASH��д���ݡ�
    FLASH_ClearFlag( FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                     FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR );
    
    
    /* ʹ�ñ�ʶ  */
    Flash_Use_Flag1 = *(uint8_t *)ADDR_FLASH_SECTOR_6;
    Flash_Use_Flag2 = *(uint8_t *)ADDR_FLASH_SECTOR_7;
    
    if (Flash_Use_Flag1 != 0x80 && Flash_Use_Flag1 != 0x01) /* ϵͳ��һ��дflash */
    {
        /* ��ȡFLASH��Sector��� */
        sector = Flash_GetSector( ADDR_FLASH_SECTOR_6 );
        /* ������Sector��� */
        FLASH_EraseSector(sector, VoltageRange_3); 
    
        address = ADDR_FLASH_SECTOR_6;
        
        for (i = 0; i < ipLength; i++)
        {
            if ( FLASH_ProgramByte( address, data_8)== FLASH_COMPLETE)
            {
               address++;  
            }
            else
            {
                while(1);
            }
        }

        /* ��������Sector��־ */        
        *(uint8_t *)ADDR_FLASH_SECTOR_6 = 0x80; /* ��Sector */
        *(uint8_t *)ADDR_FLASH_SECTOR_7 = 0x01; /* ��Sector */      
        
    }
    else if (Flash_Use_Flag1 == 0x80 && Flash_Use_Flag2 == 0x01) 
    {
    
        /* ��ȡFLASH��Sector��� */
        sector = Flash_GetSector( ADDR_FLASH_SECTOR_7 );
        /* ������Sector��� */
        FLASH_EraseSector(sector, VoltageRange_3);     
        
        address = ADDR_FLASH_SECTOR_7;
         
        /* ��ǰ������ݱ��浽��Sector */
        for (i = 0; i < index; i++)
        {
            Flash_Read_Byte(address, 4096, ip);
            Flash_Write_Byte(address, 4096, ip);
            address += 4096��
            
        }
        
                
        /* ����Ҫ�޸ĵ�IP����4K������ */
        Flash_Read_Byte(ADDR_FLASH_SECTOR_7 + index * 4096, 4096, ip);
        /* �޸Ķ�ӦIP */
        memcpy(&ip[Indication][0], data_8, length);
        
        /* ����4K���ݽ���д����Sector�� */
        Flash_Write_Byte(address, 4096, ip);

        /* ��������Sector��־ */ 
        *(uint8_t *)ADDR_FLASH_SECTOR_6 = 0x01; /* ��Sector */
        *(uint8_t *)ADDR_FLASH_SECTOR_7 = 0x80; /* ��Sector */  
        
    
    }
    else if(Flash_Use_Flag1 == 0x01 && Flash_Use_Flag2 == 0x80)
    {
        /* ��ȡFLASH��Sector��� */
        sector = Flash_GetSector( ADDR_FLASH_SECTOR_6 );
        /* ������Sector��� */
        FLASH_EraseSector(sector, VoltageRange_3);
        
        address = ADDR_FLASH_SECTOR_6;
        
        /* ��ǰ������ݱ��浽��Sector */
        for (i = 0; i < index; i++)
        {
            Flash_Read_Byte(address, 4096, ip);
            Flash_Write_Byte(address, 4096, ip);
            address += 4096��
        }  
        
        /* ����Ҫ�޸ĵ�IP����4K������ */
        Flash_Read_Byte(ADDR_FLASH_SECTOR_7 + index * 4096, 4096, ip);
        /* �޸Ķ�ӦIP */
        memcpy(&ip[Indication][0], data_8, length);        
        /* ����4K���ݽ���д����Sector�� */
        Flash_Write_Byte(address, 4096, ip);


        /* ��������Sector��־ */ 
        *(uint8_t *)ADDR_FLASH_SECTOR_6 = 0x80; /* ��Sector */
        *(uint8_t *)ADDR_FLASH_SECTOR_7 = 0x01; /* ��Sector */          
         
    } 
    else
    {
       while(1);
    }




}


/****************************************************************************
* ��    ��: Snmp_Flash_Write_Secondary_Sector
* ��    ��: ������д��ԭ��Sector������ԭ��Sector��Ϊ��Sector��ԭ��Sector��Ϊ��Sector
* ��ڲ�����primary_address   ��ԭ��������ʼ��ַ
*           secondary_address ��ԭ�η�����ʼ��ַ
*           data              ��Ҫд���IP����ָ��
*           length            �����ݳ��� 
*           index             �� Ҫд��ĵ�һ��IP�ڵڼ���4K
*           Indication        : д��ĵ�һ��IPΪ����4K��256��IP�еĵڼ���IP
* ���ڲ�������
* ˵    ������
* ���÷�������
****************************************************************************/
uint8_t Snmp_Flash_Write_Secondary_Sector
(
    uint32_t     primary_address, 
    uint32_t     secondary_address, 
    int8_t*      data,
    uint8_t      index,
    uint8_t      Indication,
    
)
{
        
        uint16_t     sector;
        char         ip[256][16];
        
    
        /* ��ȡԭ��Sector��� */
        sector = Flash_GetSector( secondary_address );
        /* ����ԭ��Sector */
        FLASH_EraseSector(sector, VoltageRange_3);
        
        
        /* ��ԭ��Sector��дλ��ǰ�������д����Sector */
        for (i = 0; i < index; i++)
        {
            /* ��ԭ��Sector����4K���� */
            Flash_Read_Byte(primary_address, 4096, ip);
            
            /* ��������4K����д�뵽��Sector */
            Flash_Write_Byte(secondary_address, 4096, ip);
            
            /* ��ַ����4K */
            primary_address += 4096��
            secondary_address += 4096��
        }  
        
        /* ����Ҫ�޸ĵ�IP����4K������ */
        Flash_Read_Byte(primary_address, 4096, ip);
        
        /* �޸Ķ�ӦIP */
        memcpy(&ip[Indication][0], data_8, length); 
        
        /* ���Ž���4K����д����Sector�� */
        Flash_Write_Byte(secondary_address, 4096, ip);


        /* ��������Sector��־ */ 
        *(uint8_t *)secondary_address = 0x80; /* ��Sector */
        *(uint8_t *)primary_address = 0x01;   /* ��Sector */ 


}

/****************************************************************************
* ��    ��: д��length�ֽڵ�����
* ��ڲ�����address����ַ
*           length�� ���ݳ���
*           data_8  ָ��д�������
* ���ڲ�������
* ˵    ������
* ���÷�������
****************************************************************************/
void Snmp_Flash_Write_Byte( uint32_t address, uint16_t length, int8_t* data )
{
    uint16_t i;
    
   
        for(i = 0; i < length; i++)
        {
            
                if ( FLASH_ProgramByte( address, data)== FLASH_COMPLETE)
                {
                   address++;  
                }
                else
                {
                    while(1);
                }
        
        }



}

/****************************************************************************
* ��    ��: ��ȡlength�ֽڵ�����
* ��ڲ�����address����ַ
*           length�� ���ݳ���
*           data_8  ָ�����������
* ���ڲ�������
* ˵    ������
* ���÷�������
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