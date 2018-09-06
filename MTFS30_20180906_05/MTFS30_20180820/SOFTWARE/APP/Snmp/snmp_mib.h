#ifndef __SNMP_MIB_H__
#define __SNMP_MIB_H__



#define XsysDescr 0
#define XsysObjectID 1
#define XsysUptime 2
#define XsysContact 3
#define XsysName 4
#define XsysLocation 5
#define XsysServices 6
#define XanalogIpIndex 7
#define XanalogIpColIndication 8
#define XanalogIp 9

#define FLASH_SNMP_START_ADDR ADDR_FLASH_SECTOR_6   //�û���ʼ��ַ���ݶ�Ϊ��ʮ����������ʼ��ַ

/* IP���������� */
#define SNMP_ANALOG_IP_MAX    1024

/* ÿ��IP���ݳ��� */
#define SNMP_ANALOG_IP_SIZE   16

/* ���д�ŵ�IP���ݸ��� */
#define SNMP_ANALOGIPTABLE_IP_MAX 64


/* ������� */
#define SNMP_OK    1
/* ������ */
#define SNMP_NG   0


/*---------------------------*/
/* ����MIB���ṹ           */
/*---------------------------*/
/* ģ��������Ա� */
typedef struct
{
    u8_t    analogIpIndex;                  /* ����(��ָʾ)��1~64 */
    u16_t    analogIpColIndication;         /* ��ָʾ      : 1~16 */
    char    analogIp[SNMP_ANALOG_IP_SIZE];  /* IP��Ϣ             */
} AnalogPropertyTable;





#endif
