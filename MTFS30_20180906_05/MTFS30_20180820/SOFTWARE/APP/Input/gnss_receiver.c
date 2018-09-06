/*****************************************************************************/
/* 文件名:    gnss_receiver.c                                                */
/* 描  述:    卫星接收机相关处理                                             */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include "gnss_receiver.h"
#include "dev_usart.h"
#include "ut4b0_receiver.h"

/*-------------------------------*/
/* 变量定义                      */
/*-------------------------------*/
satellite_info_t gsatellite_info; /* 卫星综合信息 */
recviver_info_t  grecviver_info;  /* 接收机信息   */
leap_forecast_t  gleap_forecast;  /* 闰秒预告     */
ttime_t           gtime;          /* 时间信息     */      




/*****************************************************************************
 * 函  数:    gnss_receiver_init                                                           
 * 功  能:    卫星接收机初始化                                                                  
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/ 
void gnss_receiver_init(void)
{

	memset(&grecviver_info, 0x00, sizeof(recviver_info_t));

	/* 取得接收机类型 */
#ifdef TODO	
	grecviver_info.rec_type  = fpga_read(REV_TYPE);
#endif

	switch(grecviver_info.rec_type)
	{
		case REC_TYPE_UT4B0: /* ut4b0类型接收机 */
			/* 接收机型号名称 */
			strncpy((char *)grecviver_info.rec_name, "UT4B0", 5);
			/* ut4b0接收机初始化 */
			ut4b0_receiver_init();
			break;

		default:
#if MTFS30_ERROR_EN
			MTFS30_ERROR("接收机类型有错, 接收机类型为[%d]", grecviver_info.rec_type);
#endif
			break;
	}


}

/*****************************************************************************
 * 函  数:    gnss_receiver_msg_parser                                                           
 * 功  能:    解析卫星数据                                                                 
 * 输  入:     *pdata: 卫星数据                         
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/  
void gnss_receiver_msg_parser(u8_t *pdata)
{

	/* 判断接收机类型,各类型的接收机输出数据指令可能不一样 */	 
	switch(grecviver_info.rec_type)
	{
		case REC_TYPE_UT4B0:    /* UT4B0接收机 */
			/* 解析ut4b0接收机输出的卫星数据 */
			ut4b0_receiver_msg_parser(pdata);
			break;

		default:
			break;

	}

}


/*****************************************************************************
 * 函  数:    gnss_receiver_pps_set                                                          
 * 功  能:    pps配置                                                                 
 * 输  入:    *pps_info: pps配置信息                         
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void gnss_receiver_pps_set(pps_info_t *pps_info)
{

	/* 判断接收机类型 */
	switch(grecviver_info.rec_type)
	{
		case REC_TYPE_UT4B0:    /* UT4B0接收机 */
			/* UT4B0接收机pps配置 */
			ut4b0_receiver_pps_set(pps_info);
			break;

		default:
			break;
	}			



}

///*****************************************************************************
// * 函  数:    gnss_receiver_timing_set                                                          
// * 功  能:    授时指令配置                                                                 
// * 输  入:    type  : 授时类型 
// TIMING_TYPE_REALTIME：实时定位授时;
//TIMING_TYPE_FIX     : 输入固定坐标授时
//TIMING_TYPE_SURVEYIN: 自主优化授时
//param1: 指令参数1
//param2: 指令参数2
//param3：指令参数3			  
// * 输  出:    无                                                    
// * 返回值:    无                                                    
// * 创  建:    2018-07-18 changzehai(DTT)                            
// * 更  新:    无                                       
// ****************************************************************************/
//void gnss_receiver_timing_set(u8_t type, u8_t *param1, u8_t *param2, u8_t *param3)
//{
//	u8_t buf[128] = {0};
//
//	switch(type)
//	{
//		case TIMING_TYPE_REALTIME:    /* 实时定位授时     */
//			sprintf((char *)buf, "%s*00\r\n", "ODE TIMING");
//			break;
//
//		case TIMING_TYPE_FIX:         /* 输入固定坐标授时 */
//			sprintf((char *)buf, "MODE TIMING FIX %s %s %s*00\r\n", param1, param2, param3);
//			break;
//
//		case TIMING_TYPE_SURVEYIN:    /* 自主优化授时     */
//			sprintf((char *)buf, "MODE TIMING FIX %s %s %s*00\r\n", param1, param2, param3);
//			break;
//
//		default:
//			break;
//	}
//
//
//
//	/* 使用串口1发送命令 */
//#ifdef TODO 
//	usart_send(buf, strlen((char*)buf));
//#endif		
//
//}

/*****************************************************************************
 * 函  数:    gnss_receiver_acmode_set                                                          
 * 功  能:    配置接收机工作模式                                                                 
 * 输  入:    ac_mode: 
 *                    REV_ACMODE_GPS: 单GPS定位
 *                    REV_ACMODE_BDS: 单BDS定位
 *                    REV_ACMODE_GLO: 单GLO定位
 *                    REV_ACMODE_GAL: 单GAL定位
 *                    REV_ACMODE_ALL: 多系统联合定位 
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void gnss_receiver_acmode_set(u8_t ac_mode)
{

	switch(grecviver_info.rec_type)
	{
		case REC_TYPE_UT4B0:    /* UT4B0接收机 */
			/* 配置UT4B0接收机工作模式 */
			ut4b0_receiver_acmode_set(ac_mode);
			break;

		default:
			break;
	}	


}

/*****************************************************************************
 * 函  数:    gnss_receiver_msg_set                                                           
 * 功  能:    设置接收机输出的卫星语句                                                               
 * 输  入:    ac_mode: 
 *                    REV_ACMODE_GPS: 单GPS定位
 *                    REV_ACMODE_BDS: 单BDS定位
 *                    REV_ACMODE_GLO: 单GLO定位
 *                    REV_ACMODE_GAL: 单GAL定位
 *                    REV_ACMODE_ALL: 多系统联合定位                        
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void gnss_receiver_msg_set(u8_t ac_mode)
{

	switch(grecviver_info.rec_type)
	{
		case REC_TYPE_UT4B0:    /* UT4B0接收机 */
			/* 设置ut4b0接收机输出的卫星语句  */
			ut4b0_receiver_msg_set(ac_mode);
			break;

		default:
			break;
	}	

} 

/*****************************************************************************
 * 函  数:    gnss_receiver_reset                                                           
 * 功  能:    按指定方式重启接收机                                                               
 * 输  入:    reset_mode: 
 *                       REV_RESETMODE_ALL        : 清除保存的设置，卫星星历、位置信息等再重启
 *                       REV_RESETMODE_CONFIGSAVE : 保存用户配置再重启          
 *                       REV_RESETMODE_EPHEM      : 清除保存的卫星星历再重启   
 *                       REV_RESETMODE_POSITION   : 清除保存的位置信息再重启
 *                       REV_RESETMODE_ALMANAC    : 清除保存的历书信息再重启 
 *                       REV_RESETMODE_IONUTC     : 清除电离层和 UTC 参数再重启
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void gnss_receiver_reset(u8_t reset_mode)
{

	switch(grecviver_info.rec_type)
	{
		case REC_TYPE_UT4B0:    /* UT4B0接收机 */
			/* 重启ut4b0接收机  */
			ut4b0_receiver_msg_set(reset_mode);
			break;

		default:
			break;
	}	


}




