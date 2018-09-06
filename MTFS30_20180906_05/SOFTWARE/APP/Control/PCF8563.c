#include <includes.h>
#include "arch/cc.h"
#include "config.h"
#include "I2C_DRV.h"
#include "PCF8563.h"
#define I2C_Speed              		400000
/* 8563 I2C 总线从地址：读，0A3H；写，0A2H。 */
#define PCF8563_SLAVE_ADDRESS    0xA2




/****************************************************************************
* 函数名: pcf8563_InitI2c
* 功  能: 初始化I2C硬件设备.
* 输  入: 2进制数.
* 输  出: 无.
* 返  回: BCD码.
*/
void pcf8563_InitI2c(void)
{
	//RTC_T *_SRtc;
	I2C_GPIO_Config();
	//InitRtcStruct(_SRtc);
}

/****************************************************************************
* 函数名: ByteToPbcd
* 功  能: 将二进制数转换成BCD码.
* 输  入: 2进制数.
* 输  出: 无.
* 返  回: BCD码.
*/
unsigned char pcf8563_ByteToPbcd
(
	unsigned char _ucHex						/* 二进制数 */
)
{
	return ((_ucHex / 10) << 4) + (_ucHex % 10);
}

/****************************************************************************
* 函数名: PbcdToByte
* 功  能: 将PBCD数据转换成HEX型数据.
* 输  入: bPbcd 待转换的PBCD数据.
* 输  出: 无.
* 返  回: uint8 转换后的HEX数据.
*/
unsigned char pcf8563_PbcdToByte
(
	unsigned char _ucPbcd						/* 待转换的PBCD数据 */
)
{
   	return ((10 * (_ucPbcd >> 4)) + (_ucPbcd & 0x0f));
}

/****************************************************************************
* 函数名: IsLeapYear
* 功  能: 判断时期时间是否有效
* 输  入: year ：年.
* 输  出: 无.
* 返  回: TRUE表示闰年.
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
* 函数名: IsValidDateTime
* 功  能: 判断时期时间是否有效
* 输  入: RTC_T 日期时间结构体
* 输  出: 无.
* 返  回: TRUE表示闰年.
*/
static unsigned char RTC_MonthVal[]={31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
unsigned char IsValidDateTime(RTC_T *_tRtc)
{
	/* 年值不能超过LPC2220 硬件RTC能够处理范围: 1901-2099 */
	if(_tRtc->year < RTC_YEAR_MIN || _tRtc->year > RTC_YEAR_MAX)
	{
		return false;
	}

	/* 日期不能大于指定的软件编译日期 */
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

	/* 检查月份 */
	if((_tRtc->month < 1) || (_tRtc->month > 12))
	{
		return false;
	}

	/* 检查day */
	if((_tRtc->day < 1) || (_tRtc->day > 31))
	{
		return false;
	}

	/* 判断大小月 */
	if( _tRtc->day > RTC_MonthVal[_tRtc->month - 1])
	{
		/* 闰年2月份为29天 */
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

	/* 检查时分秒 */
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
* 函数名: pcf8563_SetDateTime
* 功  能: 设定时间函数程序
* 输  入: RTC_T 日期时间结构体， _tRtc->week 无需带入，由本函数自动计算
* 输  出: 无.
* 返  回: TRUE表示设置成功.
*/
unsigned char pcf8563_SetDateTime(RTC_T *_tRtc)
{   
	unsigned char ucBuf[16];
	
	/* 如果时钟无效，则不予设置 */
	if (IsValidDateTime(_tRtc) == false)
	{
		return false;
	}
	
	/* 根据年月日,计算星期 */
	_tRtc->week = GetDOW(_tRtc->year,_tRtc->month, _tRtc->day);
	
	ucBuf[0] = 0;	/* 控制寄存器1, TEST1=0;普通模式,STOP=0;芯片时钟运行,TESTC=0;电源复位功能失效*/
	ucBuf[1] = 0;	/* 定时和报警中断无效 */
	
	ucBuf[2] = pcf8563_ByteToPbcd(_tRtc->second);
	ucBuf[3] = pcf8563_ByteToPbcd(_tRtc->minute);
	ucBuf[4] = pcf8563_ByteToPbcd(_tRtc->hour);
	ucBuf[5] = pcf8563_ByteToPbcd(_tRtc->day);
	ucBuf[6] = pcf8563_ByteToPbcd(_tRtc->week);	/* 0 -6 */

	/*
	世纪位；C=0 指定世纪数为20××，C=1	指定世纪数为19××
	*/
	ucBuf[7] = pcf8563_ByteToPbcd(_tRtc->month);
	
	ucBuf[8] = pcf8563_ByteToPbcd((_tRtc->year - 2000) & 0xFF);	/* 年低位 */
	
	ucBuf[9] = 0x00;	/* 分钟报警关闭 */
	ucBuf[10] = 0x00;	/* 小时报警关闭 */
	ucBuf[11] = 0x01;	/* 日报警关闭 */
	ucBuf[12] = 0x00;	/* 星期报警关闭 */
	
	ucBuf[13] = 0x00;	/* CLKOUT 频率寄存器无效 */
	ucBuf[14] = 0x00;	/* 倒计数定时器寄存器无效 */
	ucBuf[15] = 0x00;	/* 定时器倒计数数值寄存器 */
	
	I2C_BufferWrite(ucBuf,16,0, PCF8563_SLAVE_ADDRESS );
	return true;
}       

/****************************************************************************
* 函数名: pcf8563_GetDateTime
* 功  能: 读取PCF8563的时间
* 输  入: 
* 输  出: RTC_T 日期时间结构体， _tRtc->week 无需带入，由本函数自动计算.
* 返  回: false表示读取失败(硬件故障)，true表示读取成功
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
	
	/* 根据年月日,计算星期 */
	_tRtc->week = GetDOW(_tRtc->year,_tRtc->month, _tRtc->day);
	
	return true;
}  

/****************************************************************************
* 函数名: InitRtcStruct
* 功  能: 初始化时钟结构体
* 输  入: RTC_T 结构指针
* 输  出: 无.
* 返  回: 无.
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

	_tRtc->valid = 0;	/* 时钟无效 */
	//pcf8563_SetDateTime(_tRtc);
}
