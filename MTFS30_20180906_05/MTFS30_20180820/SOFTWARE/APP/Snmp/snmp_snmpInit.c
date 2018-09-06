#include <includes.h>
#include "arch/cc.h"
#include "Snmp\snmp_SNMP.h"
#include "Snmp\snmp_mib.h"
#include "config.h"
/* 2018-06-13 changzehai(DTT) ----------------------------------- ADD Start -*/
#include "string.h"
/* 2018-06-13 changzehai(DTT) ----------------------------------- ADD End   -*/
//-------------------------------------------------------------
extern   MIB mib_ace;

/* The following MIBs must be in lexicographical order */
static   const MIB* mibs[] =
{
    &mib_ace,
};

/* This structure is defined as external to SNMPAgentTask() */
AGENT_CONTEXT snmp_ac =
{
    mibs, ( sizeof( mibs ) / sizeof( MIB* ) ), 0
};
//------------------------------------------------------------

u8_t  snmp_Context[16];                         //SNMP������
u32_t Snmp_Enterprise = 40172;                  //��ҵ��ʶ
u8_t* Snmp_Context = snmp_Context;          //SNMP������
//
//! analogProperty table parameters.
//
/* 2018-06-13 changzehai(DTT) ----------------------------------- MOD Start -*/
AnalogPropertyTable analogPropertyTable[SNMP_ANALOGIPTABLE_IP_MAX];

/*---------------------------*/
/* ��������                  */
/*---------------------------*/
static u8_t snmp_ipmac_init();
void Snmp_Flash_Read(int8_t* data, uint16_t length, uint8_t position, uint16_t byte_position);
void Snmp_Flash_Write( int8_t* data, uint16_t length, uint8_t position, uint16_t byte_position );
uint8_t Snmp_Flash_IS_Writed_Check();
/* 2018-06-13 changzehai(DTT) ----------------------------------- MOD End   -*/


/*********************************************************************/
/* the common groud paramters.*/
/*********************************************************************/
//
//! logical id.
//
u8_t    commonNELogicalID[16];
//
//! the manufature of device.
//
u8_t    commonNEVendor[DEVICE_INFO_LEN];
//
//! the model of device.
//
u8_t    commonNEModelNumber[DEVICE_INFO_LEN];
//
//! the serial of device.
//
u8_t    commonNESerialNumber[16];
//
//! the alarm detection control .
//
s16_t   commonAlarmDetectionControl = 2;
//
//! the network address of the device.
//
u8_t    commonNetworkAddress[4];
//
//! the port of the device.
//
u16_t   commonNetworkPort;
//
//! the physical address.
//
u8_t    commonPhysicalAddress[6];
//
//! the safe status of device.
//
s16_t   commonTamperStatus = 1;
//
//! the temperature of conditon.
//
s16_t   commonInternalTemperature = 26;
//
//! the humidity of the conditon.
//
u16_t   commonInternalHumidity = 0;
//------------------------------------------------------------------------------
u8_t fnSntpServiceAddr[4];
//
////! sntp inter  time.
////
//u16_t fnSntpInter;
//------------------------------------------------------------------------------------------
//s16_t analogPropertyNO = 3;                     //ģ�����Ա���Ŀ
s16_t analogPropertyNO = 64;                      //ģ�����Ա���Ŀ
//s16_t currentAlarmNO = 0;                        //��ǰ��������Ŀ
//s16_t discretePropertyNO = 7;                   //��ɢ���Ա���Ŀ
////------------------------------------------------------------------------------------------
//const char* Str_fnChannelName[4] =
//{
//    "CH1",
//    "CH2",
//    "CH3",
//    "CH4"
//};
////
////! extern defintion from config.c.
//extern tConfigParameters g_sParameters;
//--------------------------------


/* 2018-06-13 changzehai(DTT) ----------------------------------- ADD Start -*/


//}
/* 2018-06-13 changzehai(DTT) ----------------------------------- ADD End   -*/

extern u8_t Oid_Len;

void snmpInit( void )
{

//    u8_t* buffzz;
    u16_t i;
    char buf[64][16];
//    u16_t temp;
//    u16_t configTemp;
//    u8_t i_dc = 1;
//    u8_t i_op = 1;
//    u8_t temp_bit = 0;
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR cpu_sr;
#endif
    static u8_t s_Init_flag = 0;
    u8_t ret;

    //
    //!snmp_context.
    //
    memcpy( snmp_Context, &g_sParameters.CommunityStr, 16 );                    //������
    //
    //! commonNESerialNumber.
    //
    memcpy( commonNESerialNumber, &g_sParameters.Serial, 12 );              //�豸���к�
    //
    //! manufacture.
    //
    memcpy( commonNEVendor, &g_sParameters.Manufacturer, DEVICE_INFO_LEN );             //������
    //
    //! model.
    memcpy( commonNEModelNumber, &g_sParameters.Model, DEVICE_INFO_LEN );               //�豸�ͺ�
    //
    //! logical ID /MODEL.
    //
    memcpy( commonNELogicalID, &g_sParameters.Model, 16 ); //�߼�ID
    //
    //! common network address.
    //
    commonNetworkAddress[0] = ( g_sParameters.ulStaticIP >> 24 ) & 0xff;
    commonNetworkAddress[1] = ( g_sParameters.ulStaticIP >> 16 ) & 0xff;
    commonNetworkAddress[2] = ( g_sParameters.ulStaticIP >> 8 ) & 0xff;
    commonNetworkAddress[3] =   g_sParameters.ulStaticIP & 0xff;
    //
    //! alarm detection control enable
    //
    commonAlarmDetectionControl = 2;
    //
    //  OSTimeDlyHMSM(0,0,1,500);//�ȴ���֤������һ����������ѯ����
    //
    commonNetworkPort = g_sParameters.TrapPort;

    //  //��ʼ��ģ�����Ա���ɢ���Ա�--------------------------
    /* 2018-06-13 changzehai(DTT) ----------------------------------- MOD Start -*/



    
    /* ���ɳ�ʼ��IP���� */
    if (s_Init_flag == 0)
    {
        /* ���flash�Ƿ��Ѿ�д������ */
        s_Init_flag = Snmp_Flash_IS_Writed_Check();
        if (s_Init_flag == 0) 
        {
            /* ����IP��MAC��ʼ�����ݲ����浽flash�� */
            ret = snmp_ipmac_init();
            if (ret == SNMP_NG)
            {
                //        exit(-1);
            }  
        }
            
    }

    
//    for (i = 3; i < 4; i++)
//    { 
//        Snmp_Flash_Read((int8_t *)p, 4096, i, 0);
//        for (j = 0; j < 256; j++)
//        {
//            p = p + 16;
//        }
//        
//    }
    
//    uint16_t j;
//    char buf[16];
//    char *p = &buf[0];
//    for (i = 0; i < 4; i++)
//    {
//        for (j = 0; j < 256; j++)
//        {
//            memset(buf, 0x00, sizeof(p));
//            Snmp_Flash_Read((int8_t *)buf, 16, i, j * 16);
//            p = &buf[0];
//        }
//    
//    }
    

    /* ��flashȡ����1�е��������ڳ�ʼ�� */
    Snmp_Flash_Read((int8_t *)buf, 64*16, 0, 0);
    
    
    
    /* ��ʼ��ģ��������Ա����� */
    for( i = 0; i < analogPropertyNO; i++ )
    {
 
        /* ���� */
        analogPropertyTable[i].analogIpIndex = i + 1;
        
        /* ��ָʾ */
        analogPropertyTable[i].analogIpColIndication = 1;
        
        /* IP��Ϣ */
        strncpy(analogPropertyTable[i].analogIp, buf[i], sizeof(analogPropertyTable[i].analogIp));
          
    }

    

    /* 2018-06-13 changzehai(DTT) ----------------------------------- MOD End   -*/
    //-------------------------------------------------------
    Oid_Len = 3;
    ussSNMPAgentInit();

}


/* 2018-06-13 changzehai(DTT) ----------------------------------- ADD Start -*/
/*****************************************************************************/
/* ��  ��:    snmp_ipmac_init                                                */
/* ��  ��:    ����IP��MAC��ʼ�����ݲ����浽flash��                           */
/* ��  ��:    ��                                                             */
/* ��  ��:    ��                                                             */
/* ����ֵ:    ��                                                             */
/* ��  ��:    2018-06-13 changzehai(DTT)                                     */
/* ��  ��:    ��                                                             */
/*****************************************************************************/
static u8_t snmp_ipmac_init()
{

  u16_t cnt = 0;
  u8_t i = 0;
  u8_t j = 0;
  u8_t wcnt = 0; 
  char ipmac_buf[SNMP_ANALOG_IP_SIZE];
  char *pflash_wbuf;
  char *pflash_wbuf_start; /* д��������ʼ��ַ */
  
  
  /* ����4K��д������ */
  pflash_wbuf = calloc(4096, 1);
  pflash_wbuf_start = pflash_wbuf;
  
  /*---------------------------------*/
  /* д��1024��IP                    */
  /*---------------------------------*/
  for (i = 1; i < 6; i++)  /* 1024 = 4 * 254 + 8�� ����i���ֵΪ5 */
  {
      for (j = 1; j < 255; j++)  /* ��ЧIP��ΧΪ1~254 */
      {
          
         /* ����IP��ַ */
          memset(ipmac_buf, 0x00, sizeof(ipmac_buf));
          sprintf(ipmac_buf, "172.18.%d.%d", i, j);
          
          /* д��IP��ַ */
          strcpy(pflash_wbuf, ipmac_buf);
          
          /* ÿ��ƫ��һ��IP���ݳ��� */
          pflash_wbuf = pflash_wbuf + SNMP_ANALOG_IP_SIZE;
          
          cnt++;  
          
//          /* ����������IP*/
//          if (cnt >= SNMP_ANALOG_IP_MAX)
//          {
//              /* ��flash_wbuf������д��flash */
//              Snmp_Flash_Write(flash_wbuf, (uint16_t)(pflash_wbuf - flash_wbuf), wcnt, 0);
//              
//              break;
//          }
          
          /* 4K��С��flash_wbuf�������Ѿ�д�� */
          if (cnt % 256 == 0) /* 256 = 4096 / 16 */
          {
              /* ��flash_wbuf������д��flash */
              Snmp_Flash_Write((int8_t *)pflash_wbuf_start, 4096, wcnt, 0);
              
              /* д4K�ļ�����1 */
              wcnt++;
              if (wcnt == 4) /* 4 = 1024 / 256 */
              {
                  break;
              }
              
              
              /* pflash_wbuf����ָ��д��������ʼ��ַ */
              pflash_wbuf = pflash_wbuf_start;
          }
          

          
      }
  
  }
  
  /* �ͷ�д�������ռ� */
  free(pflash_wbuf_start);
  
  return SNMP_OK;
}


/* 2018-06-13 changzehai(DTT) ----------------------------------- ADD End   -*/