/*****************************************************************************/
/* 文件名:    gnss_handler.c                                                 */
/* 描  述:    GNSS相关设置/查询处理                                          */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include "gnss_handler.h"
#include "gnss_receiver.h"
#include "mtfs30.h"


/*-------------------------------*/
/* 变量声明                      */
/*-------------------------------*/
extern satellite_info_t gsatellite_info; /* 卫星综合信息 */
extern recviver_info_t  grecviver_info;  /* 接收机信息   */
extern leap_forecast_t  gleap_forecast;  /* 闰秒预告     */
extern ttime_t           gtime;           /* 时间信息     */ 


/*****************************************************************************
 * 函  数:    gnss_pps_set_handler                                                           
 * 功  能:    配置pps                                                                
 * 输  入:    *pdata: 卫星数据                         
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/  
void gnss_pps_set_handler(u8_t *pparam)
{




}

/*****************************************************************************
 * 函  数:    gnss_acmode_set_handler                                                           
 * 功  能:    配置接收机工作模式                                                                
 * 输  入:    *pparam: 配置信息                         
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                   
 ****************************************************************************/  
void gnss_acmode_set_handler(u8_t *pparam)
{

	u8_t ac_mode = 0;  /* 工作模式 */

#ifdef TODO
	解析配置参数
		ac_mode = ?
#endif

		switch(ac_mode)
		{
			case REV_ACMODE_GPS:    /* 单GPS定位      */
			case REV_ACMODE_BDS:    /* 单BDS定位      */
			case REV_ACMODE_GLO:    /* 单GLO定位      */
			case REV_ACMODE_GAL:    /* 单GAL定位      */
			case REV_ACMODE_ALL:    /* 多系统联合定位 */
				/* 与上次保存的工作模式不同 */
				if (ac_mode == grecviver_info.ac_mode) 
				{
					/* 配置新的工作模式 */
					gnss_receiver_acmode_set(ac_mode);

					/* 重新配置输出的卫星语句 */
					gnss_receiver_msg_set(ac_mode);

					/* 保存本次配置 */
					grecviver_info.ac_mode = ac_mode;
				}
				break;

			default:
#if MTFS30_ERROR_EN
				MTFS30_ERROR("接收机工作模式(%d)有误", mode);
#endif	
				break;

		}

}

/*****************************************************************************
 * 函  数:    gnss_receiver_reset_handler                                                           
 * 功  能:    重启接收机                                                                
 * 输  入:    *pparam: 重启方式信息                        
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                   
 ****************************************************************************/ 
void gnss_receiver_reset_handler(u8_t *pparam)
{

	u8_t reset_mode = 0;  /* 重启方式 */

#ifdef TODO
	解析配置参数
		reset_mode = ?
#endif	

		/* 判断重启方式 */
		switch(reset_mode)
		{
			case REV_RESETMODE_ALL:           /* 清除保存的设置，卫星星历、位置信息等再重启 */
			case REV_RESETMODE_CONFIGSAVE:    /* 保存用户配置再重启          */
			case REV_RESETMODE_EPHEM:         /* 清除保存的卫星星历再重启    */
			case REV_RESETMODE_POSITION:      /* 清除保存的位置信息再重启    */
			case REV_RESETMODE_ALMANAC:       /* 清除保存的历书信息再重启    */
			case REV_RESETMODE_IONUTC:        /* 清除电离层和 UTC 参数再重启 */
				/* 重启接收机 */
				gnss_receiver_reset(reset_mode);
				break;

			default:
#if MTFS30_ERROR_EN
				MTFS30_ERROR("重启方式(%d)有误", reset_mode);
#endif			
				break;



		}


}
