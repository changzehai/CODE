#include <includes.h>
#include "arch/cc.h"
#include "config.h"
#include "I2C_DRV.h"
#include "PCF8563.h"
#define I2C_Speed              		400000
/* 8563 I2C ���ߴӵ�ַ������0A3H��д��0A2H�� */
#define PCF8563_SLAVE_ADDRESS    0xA2




/****************************************************************************
* ������: pcf8563_InitI2c
* ��  ��: ��ʼ��I2CӲ���豸.
* ��  ��: 2������.
* ��  ��: ��.
* ��  ��: BCD��.
*/
void pcf8563_InitI2c(void)
{
	//RTC_T *_SRtc;
	I2C_GPIO_Config();
	//InitRtcStruct(_SRtc);
}

/****************************************************************************
* ������: ByteToPbcd
* ��  ��: ����������ת����BCD��.
* ��  ��: 2������.
* ��  ��: ��.
* ��  ��: BCD��.
*/
unsigned char pcf8563_ByteToPbcd
(
	unsigned char _ucHex						/* �������� */
)
{
	return ((_ucHex / 10) << 4) + (_ucHex % 10);
}

/****************************************************************************
* ������: PbcdToByte
* ��  ��: ��PBCD����ת����HEX������.
* ��  ��: bPbcd ��ת����PBCD����.
* ��  ��: ��.
* ��  ��: uint8 ת�����HEX����.
*/
unsigned char pcf8563_PbcdToByte
(
	unsigned char _ucPbcd						/* ��ת����PBCD���� */
)
{
   	return ((10 * (_ucPbcd >> 4)) + (_ucPbcd & 0x0f));
}

/****************************************************************************
* ������: IsLeapYear
* ��  ��: �ж�ʱ��ʱ���Ƿ���Ч
* ��  ��: year ����.
* ��  ��: ��.
* ��  ��: TRUE��ʾ����.
*/
static unsigned char IsLeapYear (unsigned short year)
{
	if (!(year%4) && (year%100) || !(year%400))
	{
		return true;
	}
	else
	{
		return false;
	}
}

/****************************************************************************
* ������: IsValidDateTime
* ��  ��: �ж�ʱ��ʱ���Ƿ���Ч
* ��  ��: RTC_T ����ʱ��ṹ��
* ��  ��: ��.
* ��  ��: TRUE��ʾ����.
*/
static unsigned char RTC_MonthVal[]={31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
unsigned char IsValidDateTime(RTC_T *_tRtc)
{
	/* ��ֵ���ܳ���LPC2220 Ӳ��RTC�ܹ�����Χ: 1901-2099 */
	if(_tRtc->year < RTC_YEAR_MIN || _tRtc->year > RTC_YEAR_MAX)
	{
		return false;
	}

	/* ���ڲ��ܴ���ָ��������������� */
	if (_tRtc->year == RTC_YEAR_MIN)
	{
		if (_tRtc->month < RTC_MONTH_MIN)
		{
			return false;
		}
		else if(_tRtc->day < RTC_DAY_MIN)
		{
			return false;
		}
	}

	/* ����·� */
	if((_tRtc->month < 1) || (_tRtc->month > 12))
	{
		return false;
	}

	/* ���day */
	if((_tRtc->day < 1) || (_tRtc->day > 31))
	{
		return false;
	}

	/* �жϴ�С�� */
	if( _tRtc->day > RTC_MonthVal[_tRtc->month - 1])
	{
		/* ����2�·�Ϊ29�� */
		if(_tRtc->month == 2)
		{
			if (IsLeapYear( _tRtc->year))
			{
				if (_tRtc->day > 29 )
				{
					return false;
				}
			}
			else if (_tRtc->day > 28 )
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	/* ���ʱ���� */
	if ((_tRtc->hour > 23) || (_tRtc->minute > 59) || (_tRtc->second > 59) )
	{
		return false;
	}

	return true;
}

/*************************************************************************
 * Function Name: GetDOY
 * Parameters: uint16 Year
 *			uint8 month
 *			uint8 day
 *
 * Return: int
 *
 * Description: Get the day of year according to the date
 *
 *************************************************************************/
static int GetDOY (unsigned short year, unsigned char month, unsigned char day)
{
	int DOY=0, i;

	for(i=1; i<month; i++)
		DOY+=RTC_MonthVal[i-1];
	if (month>2)
		if (IsLeapYear(year))
			DOY++;

	return (DOY+day);
}

/*************************************************************************
 * Function Name: GetDOW
 * Parameters: uint16 Year
 *			uint8 month
 *			uint8 day
 *
 * Return: int -- (0~6)
 *
 * Description: Get the day of week according to the date.
 *
 * NOTE: Year is not smaller than RTC_YEARMIN (1901).
 *
 *************************************************************************/
int GetDOW (unsigned short year, unsigned char month, unsigned char day)
{
	int i = RTC_BASEYEAR, DOW = 0;

	for (i = RTC_BASEYEAR, DOW = 0; i < year; i++)
	{
		DOW += 365;
		if  (IsLeapYear(i))
			DOW++;
	}

	DOW +=  GetDOY (year, month, day) - 1;
	DOW = (DOW + RTC_BASEDOW) % 7;

	return DOW;
}

/****************************************************************************
* ������: pcf8563_SetDateTime
* ��  ��: �趨ʱ�亯������
* ��  ��: RTC_T ����ʱ��ṹ�壬 _tRtc->week ������룬�ɱ������Զ�����
* ��  ��: ��.
* ��  ��: TRUE��ʾ���óɹ�.
*/
unsigned char pcf8563_SetDateTime(RTC_T *_tRtc)
{   
	unsigned char ucBuf[16];
	
	/* ���ʱ����Ч���������� */
	if (IsValidDateTime(_tRtc) == false)
	{
		return false;
	}
	
	/* ����������,�������� */
	_tRtc->week = GetDOW(_tRtc->year,_tRtc->month, _tRtc->day);
	
	ucBuf[0] = 0;	/* ���ƼĴ���1, TEST1=0;��ͨģʽ,STOP=0;оƬʱ������,TESTC=0;��Դ��λ����ʧЧ*/
	ucBuf[1] = 0;	/* ��ʱ�ͱ����ж���Ч */
	
	ucBuf[2] = pcf8563_ByteToPbcd(_tRtc->second);
	ucBuf[3] = pcf8563_ByteToPbcd(_tRtc->minute);
	ucBuf[4] = pcf8563_ByteToPbcd(_tRtc->hour);
	ucBuf[5] = pcf8563_ByteToPbcd(_tRtc->day);
	ucBuf[6] = pcf8563_ByteToPbcd(_tRtc->week);	/* 0 -6 */

	/*
	����λ��C=0 ָ��������Ϊ20������C=1	ָ��������Ϊ19����
	*/
	ucBuf[7] = pcf8563_ByteToPbcd(_tRtc->month);
	
	ucBuf[8] = pcf8563_ByteToPbcd((_tRtc->year - 2000) & 0xFF);	/* ���λ */
	
	ucBuf[9] = 0x00;	/* ���ӱ����ر� */
	ucBuf[10] = 0x00;	/* Сʱ�����ر� */
	ucBuf[11] = 0x01;	/* �ձ����ر� */
	ucBuf[12] = 0x00;	/* ���ڱ����ر� */
	
	ucBuf[13] = 0x00;	/* CLKOUT Ƶ�ʼĴ�����Ч */
	ucBuf[14] = 0x00;	/* ��������ʱ���Ĵ�����Ч */
	ucBuf[15] = 0x00;	/* ��ʱ����������ֵ�Ĵ��� */
	
	I2C_BufferWrite(ucBuf,16,0, PCF8563_SLAVE_ADDRESS );
	return true;
}       

/****************************************************************************
* ������: pcf8563_GetDateTime
* ��  ��: ��ȡPCF8563��ʱ��
* ��  ��: 
* ��  ��: RTC_T ����ʱ��ṹ�壬 _tRtc->week ������룬�ɱ������Զ�����.
* ��  ��: false��ʾ��ȡʧ��(Ӳ������)��true��ʾ��ȡ�ɹ�
*/
unsigned char pcf8563_GetDateTime(RTC_T *_tRtc)
{   
	unsigned char ucaBuf[16];
	
	if (I2C_ReadByte(ucaBuf,7,2,PCF8563_SLAVE_ADDRESS) == false)
	{
		return false;
	}

	_tRtc->second = pcf8563_PbcdToByte(ucaBuf[0]);
	_tRtc->minute = pcf8563_PbcdToByte(ucaBuf[1] & 0x7F);
	_tRtc->hour = pcf8563_PbcdToByte(ucaBuf[2] & 0x3F);
	_tRtc->day = pcf8563_PbcdToByte(ucaBuf[3] & 0x3F);
	//_tRtc->week = pcf8563_PbcdToByte(ucBuf[4]);
	_tRtc->month = pcf8563_PbcdToByte(ucaBuf[5] & 0x1F);
	_tRtc->year = pcf8563_PbcdToByte(ucaBuf[6]) + 2000;
	
	/* ����������,�������� */
	_tRtc->week = GetDOW(_tRtc->year,_tRtc->month, _tRtc->day);
	
	return true;
}  

/****************************************************************************
* ������: InitRtcStruct
* ��  ��: ��ʼ��ʱ�ӽṹ��
* ��  ��: RTC_T �ṹָ��
* ��  ��: ��.
* ��  ��: ��.
*/
void InitRtcStruct(RTC_T *_tRtc)
{
	_tRtc->year = 2009;
	_tRtc->month = 6;
	_tRtc->day = 29;
	_tRtc->hour = 16;
	_tRtc->minute = 17;
	_tRtc->second = 0;
	_tRtc->week = 0;	/* 0 - 6 */

	_tRtc->valid = 0;	/* ʱ����Ч */
	//pcf8563_SetDateTime(_tRtc);
}
