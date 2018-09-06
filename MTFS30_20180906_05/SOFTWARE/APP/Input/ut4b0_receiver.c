/*****************************************************************************/
/* 文件名:    ut4b0_receiver.c                                               */
/* 描  述:    UT4B0接收机相关处理                                            */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include "ut4b0_receiver.h"
#include "gnss_receiver.h"
#include "util.h"
#include "dev_usart.h"
#include "mtfs30_debug.h"
#include "string.h"
#include "cc.h"

/*-------------------------------*/
/* 变量定义                      */
/*-------------------------------*/


/*-------------------------------*/
/* 变量声明                      */
/*-------------------------------*/
extern satellite_info_t gsatellite_info; /* 卫星综合信息 */
extern recviver_info_t  grecviver_info;  /* 接收机信息   */
extern leap_forecast_t  gleap_forecast;  /* 闰秒预告     */
extern ttime_t           gtime;           /* 时间信息     */ 

/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
static void ut4b0_check_cmdhandler(u8_t *pdata);
static void ut4b0_gga_msg_parse(u8_t *pdata);
static void ut4b0_gsv_msg_parse(u8_t *pdata);
static void ut4b0_gpsbds_utc_msg_parse(u8_t *pdata);
static void ut4b0_gal_utc_msg_parse(u8_t *pdata);
static void get_leap_forecast(u8_t sat_type, u32_t wn, u8_t dn, u8_t now_leap, u8_t next_leap);
static u8_t ut4b0_checkcode_compare(u8_t *pmsg, u8_t *pcode, u8_t check_type);
static void ut4b0_1pps_msg_parse(u8_t *pdata);





/* ut4b0接收机数据输出指令处理表 */
static cmdhandle_t ut4b0_cmdhandle[] =
{
	/* GGA信息 */
	{(u8_t *)"GNGGA",      ut4b0_gga_msg_parse},
	{(u8_t *)"GPSGGA",     ut4b0_gga_msg_parse},
	{(u8_t *)"BDSGGA",     ut4b0_gga_msg_parse},
	{(u8_t *)"GLOGGA",     ut4b0_gga_msg_parse},
	{(u8_t *)"GALGGA",     ut4b0_gga_msg_parse},

	/* GSV信息 */
	{(u8_t *)"GPGSV",      ut4b0_gsv_msg_parse},
	{(u8_t *)"BDGSV",      ut4b0_gsv_msg_parse},
	{(u8_t *)"GLGSV",      ut4b0_gsv_msg_parse},
	{(u8_t *)"GAGSV",      ut4b0_gsv_msg_parse},	

	/* UTC信息 */
	{(u8_t *)"GPSUTC",     ut4b0_gpsbds_utc_msg_parse},
	{(u8_t *)"BDSUTC",     ut4b0_gpsbds_utc_msg_parse},
	{(u8_t *)"GALUTC",     ut4b0_gal_utc_msg_parse},
    
    /* 1PPS */
    {(u8_t *)"BBPPSMSGAA",     ut4b0_1pps_msg_parse},

};

/*****************************************************************************
 * 函  数:    ut4b0_receiver_init                                                           
 * 功  能:    ut4b0接收机初始化                                                                
 * 输  入:     *pdata: 卫星数据                         
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/  
void ut4b0_receiver_init(void)
{
	pps_info_t pps_info; /* 存放pps配置信息 */


	/* 设置ut4b0接收机工作模式 */
	ut4b0_receiver_acmode_set(REV_ACMODE_ALL);

	/* 设置ut4b0接收机输出的卫星语句 */
	ut4b0_receiver_msg_set(REV_ACMODE_ALL);



	/* pps初始配置 */
	memset(&pps_info, 0x00, sizeof(pps_info));
	strcpy((char *)pps_info.en_switch, "ENABLE");  /* 使能 PPS 输出，且只有时间有效时输出 PPS 信号；当卫星失锁，接收机不定位时，无 PPS 信号输出。 */
	strcpy((char *)pps_info.timeref, "GPS");      
	strcpy((char *)pps_info.polatity, "POSITIVE"); /* 上升沿有效 */
	pps_info.width = 500000;                       /* 脉冲宽度为500毫秒 */	  
	pps_info.period = 1000;                        /* pps周期为1秒      */ 
	pps_info.rfdelay = 0;                          /* RF延迟0纳秒       */
	pps_info.userdelay = 0;                        /* 用户延迟0纳秒     */

	/* 设置pps */
	ut4b0_receiver_pps_set(&pps_info);



}

/*****************************************************************************
 * 函  数:    ut4b0_receiver_msg_set                                                           
 * 功  能:    设置ut4b0接收机输出的卫星语句                                                               
 * 输  入:    mode: 
 *                 REV_ACMODE_GPS: 单GPS定位
 *                 REV_ACMODE_BDS: 单BDS定位
 *                 REV_ACMODE_GLO: 单GLO定位
 *                 REV_ACMODE_GAL: 单GAL定位
 *                 REV_ACMODE_ALL: 多系统联合定位                        
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/ 
void ut4b0_receiver_msg_set(u8_t mode)
{
	u8_t buf[20] = {0};


	/* 禁止所有信息输出 */
	memset(buf, 0x00, sizeof(buf));
	strcpy((char *)buf, "unlog");
	usart_send(buf, 1); /* 使用串口1发送命令 */			     


	/* 判断工作模式 */
	switch(mode)
	{
		case REV_ACMODE_GPS:    /* 单GPS定位 */
			/* GPSGGA */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "GPSGGA 1");
			usart_send(buf, 1); /* 使用串口1发送命令 */		

			/* GPSUTC */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "GPSUTC 1");
			usart_send(buf, 1); /* 使用串口1发送命令 */				
			break;

		case REV_ACMODE_BDS:    /* 单BDS定位 */
			/* BDSGGA */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "BDSGGA 1");
			usart_send(buf, 1); /* 使用串口1发送命令 */

			/* BDSUTC */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "BDSUTC 1");
			usart_send(buf, 1); /* 使用串口1发送命令 */	
			break;

		case REV_ACMODE_GLO:    /* 单GLO定位 */
			/* GLOGGA */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "GLOGGA 1");
			usart_send(buf, 1); /* 使用串口1发送命令 */			    
			break;

		case REV_ACMODE_GAL:    /* 单GAL定位 */
			/* GALGGA */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "GALGGA 1");
			usart_send(buf, 1); /* 使用串口1发送命令 */	

			/* GALUTC */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "GALUTC 1");
			usart_send(buf, 1); /* 使用串口1发送命令 */				
			break;

		case REV_ACMODE_ALL:    /* 多系统联合定位 */
			/* GNGGA */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "GNGGA 1");
			usart_send(buf, 1); /* 使用串口1发送命令 */
 
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("[%s:%d]:    %s\n", __FUNCTION__, __LINE__, buf);      
#endif              

//			/* GPSUTC(多系统联合输出时以GPS UTC时间为准) */ 
//			memset(buf, 0x00, sizeof(buf));
//			strcpy((char *)buf, "GPSUTC 1");
//			usart_send(buf, 1); /* 使用串口1发送命令 */	
            /* GPSUTC(多系统联合输出时以GPS UTC时间为准) */ 
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "BDSUTC 1");
			usart_send(buf, 1); /* 使用串口1发送命令 */	
            
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("%s\n", buf);    
#endif                       
			break;			

		default:
#if MTFS30_ERROR_EN
			MTFS30_ERROR("接收机工作模式(%d)有误", mode);
#endif			
			break;
	}
   

	/* GPGSV */
	memset(buf, 0x00, sizeof(buf));
	strcpy((char *)buf, "GPGSV 1");
	usart_send(buf, 1); /* 使用串口1发送命令 */	
#if MTFS30_DEBUG_EN
    MTFS30_DEBUG("%s\n", buf);      
#endif  
     /* 1PPS */
	memset(buf, 0x00, sizeof(buf));
	strcpy((char *)buf, "BBPPSMSGA ONCHANGED");
	usart_send(buf, 1); /* 使用串口1发送命令 */	
    
#if MTFS30_DEBUG_EN
    MTFS30_DEBUG("%s\n", buf);   
#endif

#ifdef TODO	
	/* ANTENNA */
	memset(buf, 0x00, sizeof(buf));
	strcpy((char *)buf, "ANTENNA 1");
	usart_send(buf, 1); /* 使用串口1发送命令 */	
#endif	
    


}

/*****************************************************************************
 * 函  数:    ut4b0_receiver_acmode_set                                                           
 * 功  能:    设置ut4b0接收机工作模式                                                              
 * 输  入:    mode: 
 *                 REV_ACMODE_GPS: 单GPS定位
 *                 REV_ACMODE_BDS: 单BDS定位
 *                 REV_ACMODE_GLO: 单GLO定位
 *                 REV_ACMODE_GAL: 单GAL定位
 *                 REV_ACMODE_ALL: 多系统联合定位                        
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/ 
void ut4b0_receiver_acmode_set(u8_t mode)
{
	u8_t buf[20] = {0};


	/* 判断接收机工作模式 */
	switch(mode)
	{
		case REV_ACMODE_GPS:    /* 单GPS定位 */	

			/* 禁用BDS */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "MASK BDS");         
			usart_send(buf, 1); /* 使用串口1发送命令 */

			/* 禁用GLO */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "MASK GLO");         
			usart_send(buf, 1); /* 使用串口1发送命令 */		

			/* 禁用GAL */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "MASK GAL");         
			usart_send(buf, 1); /* 使用串口1发送命令 */	

			/* 启用GPS */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "UNMASK GPS");         
			usart_send(buf, 1); /* 使用串口1发送命令 */				
			break;

		case REV_ACMODE_BDS:    /* 单BDS定位 */
			/* 禁用GPS */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "MASK GPS");         
			usart_send(buf, 1); /* 使用串口1发送命令 */

			/* 禁用GLO */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "MASK GLO");         
			usart_send(buf, 1); /* 使用串口1发送命令 */		

			/* 禁用GAL */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "MASK GAL");         
			usart_send(buf, 1); /* 使用串口1发送命令 */	

			/* 启用GPS */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "UNMASK BDS");         
			usart_send(buf, 1); /* 使用串口1发送命令 */	
			break;

		case REV_ACMODE_GLO:    /* 单GLO定位 */
			/* 禁用GPS */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "MASK GPS");         
			usart_send(buf, 1); /* 使用串口1发送命令 */

			/* 禁用BDS */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "MASK BDS");         
			usart_send(buf, 1); /* 使用串口1发送命令 */		

			/* 禁用GAL */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "MASK GAL");         
			usart_send(buf, 1); /* 使用串口1发送命令 */	

			/* 启用GLO */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "UNMASK GLO");         
			usart_send(buf, 1); /* 使用串口1发送命令 */	
			break;   

		case REV_ACMODE_GAL:    /* 单GAL定位 */
			/* 禁用GPS */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "MASK GPS");         
			usart_send(buf, 1); /* 使用串口1发送命令 */

			/* 禁用BDS */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "MASK BDS");         
			usart_send(buf, 1); /* 使用串口1发送命令 */		

			/* 禁用GLO */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "MASK GLO");         
			usart_send(buf, 1); /* 使用串口1发送命令 */	

			/* 启用GAL */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "UNMASK GAL");         
			usart_send(buf, 1); /* 使用串口1发送命令 */	
			break;

		case REV_ACMODE_ALL:   /* 多系统联合定位  */
			/* 启用GPS */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "UNMASK GPS");    
             
			usart_send(buf, 1); /* 使用串口1发送命令 */
            
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("%s\n", buf);           
#endif               

			/* 启用BDS */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "UNMASK BDS");         
			usart_send(buf, 1); /* 使用串口1发送命令 */
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("%s\n", buf);          
#endif                

			/* 启用GLO */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "UNMASK GLO");         
			usart_send(buf, 1); /* 使用串口1发送命令 */
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("%s\n", buf);          
#endif                

			/* 启用GAL */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "UNMASK GAL");         
			usart_send(buf, 1); /* 使用串口1发送命令 */
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("%s\n", buf);          
#endif                
			break;

		default:
#if MTFS30_ERROR_EN
			MTFS30_ERROR("接收机工作模式(%d)有误", mode);
#endif	
			break;
	}


}


/*****************************************************************************
 * 函  数:    ut4b0_receiver_msg_parser                                                           
 * 功  能:    解析ut4b0接收机输出的卫星数据                                                                 
 * 输  入:     *pdata: 卫星数据                         
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/  
void ut4b0_receiver_msg_parser(u8_t *pdata)
{
	u8_t *p = pdata;    /* 指向每个字符           */
	u8_t *pmsg = NULL;  /* 指向每条数据输出指令   */
    u8_t *pcode = NULL; /* 执行数据指令中的校验码 */
    u8_t msg_buf[256];
    u8_t msg_len = 0;
    u8_t msg_chck_type; /* 校验类型 */

    
	while(*p != '\0')
	{
		/* $表示一条数据输出指令开始 */
		if (*p == '$' || *p == '#')
		{
              
			/* 截取出该条数据指令 */
            p++; /* 从$后第一个字节开始截取 */ 
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("p: %s\n", p);          
#endif                 
            memset(msg_buf, 0x00, sizeof(msg_buf));
            msg_len = my_strtok('$', (const char *)p, (char *)msg_buf, 256);
            
            /* 检查是否存在"#" */
            if (strchr((char *)msg_buf, '#' ) != NULL)
            {
                memset(msg_buf, 0x00, sizeof(msg_buf));
                msg_len = my_strtok('#', (const char *)p, (char *)msg_buf, 256);
            }
            
 
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("msg_buf: %s\n", msg_buf);          
#endif            
            /* 检查该条数据指令是否完整
             * 主要检查是否存在"*","\r","\n"
             */
            if ((strchr((char *)msg_buf, '*' ) == NULL) ||
                (strchr((char *)msg_buf, '\r') == NULL) ||
                (strchr((char *)msg_buf, '\n') == NULL))
            {
                /* 跳过该条指令 */
                p = p + msg_len - 1;
                continue;
            }
            
            /* 截取出真正的数据部分 */
            pmsg = (u8_t*)strtok((char *)msg_buf, "*");
#if MTFS30_DEBUG_EN_1
            MTFS30_DEBUG("pmsg: %s\n", pmsg);          
#endif               
            
            /* 截取出校验码 */
            pcode =(u8_t*)strtok(NULL, "\r");
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("pcode: %s\n", pcode);   
            
#endif      
            /* 区分校验类型 */
            if ((strncmp("GPSUTC", (char *)pmsg, 6) == 0) || 
                (strncmp("BDSUTC", (char *)pmsg, 6) == 0) ||
                (strncmp("GALUTC", (char *)pmsg, 6) == 0) ||
                (strncmp("BBPPSMSGAA", (char *)pmsg, 10) == 0))
            {
                msg_chck_type = CHECK_TYPE_CRC32; /* CRC32校验 */
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("msg_chck_type: CHECK_TYPE_CRC32\n");   
            
#endif                  
            } 
            else
            {
                msg_chck_type = CHECK_TYPE_XOR; /* 异或校验 */
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("msg_chck_type: CHECK_TYPE_XOR\n");   
            
#endif                  
            }
                    
            
            /* 比较校验码是否一致 */
            if (CHECKCODE_EQ == ut4b0_checkcode_compare(pmsg, pcode, msg_chck_type))
            {
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("CHECKCODE_EQ\n");          
#endif                 
                /* 查找并执行该条数据指令的解析函数 */
			    ut4b0_check_cmdhandler(pmsg);            
            
            }

			/* 跳过该条指令 */
            p = p + msg_len - 1;

		}

		p++; 
	}

}


/*****************************************************************************
 * 函  数:    ut4b0_checkcode_compare                                                           
 * 功  能:    计算数据指令数据部分的校验码并与该数据指令中的校验码进行比较                                                              
 * 输  入:    *pmsg      : 数据指令数据部分   
 *            *pcode     : 数据指令中的校验码
 *            check_type : 校验方式
 *                         CHECK_TYPE_XOR  : 异或校验
 *                         CHECK_TYPE_CRC32: crc32校验
 * 输  出:    无                                                    
 * 返回值:    CHECKCODE_EQ: 校验码相同; CHECKCODE_NOEQ: 校验码不同                                                   
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static u8_t ut4b0_checkcode_compare(u8_t *pmsg, u8_t *pcode, u8_t check_type)
{
    
    u8_t ret = CHECKCODE_NOEQ;
   
    /* 判断校验方式 */
    switch(check_type)
    {
    case CHECK_TYPE_XOR:    /* 异或校验 */
        ret = util_xor_check(pmsg, strlen((char *)pmsg), pcode);
        break;
        
    case CHECK_TYPE_CRC32: /* crc32校验 */
        ret = util_check_crc32(pmsg, strlen((char *)pmsg), pcode);
        break;
    
    default:
#if MTFS30_ERROR_EN
			MTFS30_ERROR("校验方式(%d)有误\n", check_type);
#endif	        
        break;
    }


    return ret;
}

/*****************************************************************************
 * 函  数:    ut4b0_check_cmdhandler                                                           
 * 功  能:    查找UT4B0接收机数据输出指令的解析函数并调用                                                              
 * 输  入:     *pdata: 卫星数据                         
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/  
static void ut4b0_check_cmdhandler(u8_t *pdata)
{
	u8_t i = 0;
	u8_t cnt = 0;        /* 命令表中命令条数 */


	/* 计算命令条数 */
	cnt = sizeof(ut4b0_cmdhandle) / sizeof(ut4b0_cmdhandle[0]);

	/* 查找指定命令 */
	for (i = 0; i < cnt; i++)
	{
		if(strncmp((char *)ut4b0_cmdhandle[i].cmd, (char*)pdata, strlen((char *)ut4b0_cmdhandle[i].cmd)) == 0)
		{
			/* 调用解析函数 */
			ut4b0_cmdhandle[i].cmd_fun(pdata);
			return;
		}
	} 

#if MTFS30_ERROR_EN
	u8_t *pcmd = pdata; 

	/* 取出输出指令 */
	pcmd = strtok(pdata, ",");
	MTFS30_ERROR("输出指令(%s)有误", pcmd);
#endif	

}

/*****************************************************************************
 * 函  数:    ut4b0_gga_msg_parse                                                           
 * 功  能:    解析UT4B0接收机输出GGA信息                                                                
 * 输  入:    *pdata: XXXGGA信息                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/  
static void ut4b0_gga_msg_parse(u8_t *pdata)
{
	u8_t *p = NULL;  


	/* GGA头(不做处理) */
	p = (u8_t *)strtok((char *)pdata, ",");

	/* UTC时间 */
	p = (u8_t *)strtok(NULL, ",");
	memset(gtime.hms, 0x00, sizeof(gtime.hms));
	strcpy((char *)gtime.hms, (const char *)p);
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("UTC时间： %s\n", gtime.hms);     
#endif 

	/* 纬度 */
	p = (u8_t *)strtok(NULL, ",");		
	memset(gsatellite_info.location.lat, 0x00, sizeof(gsatellite_info.location.lat));
	strcpy((char *)gsatellite_info.location.lat, (const char *)p);
#if MTFS30_DEBUG_EN
    MTFS30_DEBUG("[%s:%d] 纬度: %s\n", __FUNCTION__, __LINE__, gsatellite_info.location.lat);   
#endif 
    
	/* 纬度方向 */
	p = (u8_t *)strtok(NULL, ",");
	gsatellite_info.location.latdir = *p;
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("[%s:%d] 纬度方向: %c\n", __FUNCTION__, __LINE__, gsatellite_info.location.latdir);
#endif 
            
	/* 经度 */
	p = (u8_t *)strtok(NULL, ",");
	memset(gsatellite_info.location.lon, 0x00, sizeof(gsatellite_info.location.lon));
	strcpy((char *)gsatellite_info.location.lon, (const char *)p);   
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("经度： %s\n", gsatellite_info.location.lon);          
#endif 
            
	/* 经度方向 */
	p = (u8_t *)strtok(NULL, ",");
	gsatellite_info.location.londir = *p;
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("经度方向： %c\n", gsatellite_info.location.londir);          
#endif
    
	/* GPS质量指示符(不做处理) */
	p = (u8_t *)strtok(NULL, ",");

	/* 使用中的卫星数 */
	p = (u8_t *)strtok(NULL, ",");
	gsatellite_info.use_sats = *p;
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("使用中的卫星数： %c\n", gsatellite_info.use_sats);          
#endif

	/* 水平精度因子(不做处理) */
	p = (u8_t *)strtok(NULL, ",");

	/* 海拔 */
	p = (u8_t *)strtok(NULL, ",");
	memset(gsatellite_info.location.alt, 0x00, sizeof(gsatellite_info.location.alt));
	strcpy((char *)gsatellite_info.location.alt, (const char *)p);	
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("海拔： %s\n", gsatellite_info.location.alt);          
#endif	

	/* 海拔之后的其他参数不做处理 */	 

 
}


/*****************************************************************************
 * 函  数:    ut4b0_gsv_msg_parse                                                           
 * 功  能:    解析UT4B0接收机输出GSV信息                                                                 
 * 输  入:    *pdata: GSV信息;                       
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/  
static void ut4b0_gsv_msg_parse(u8_t *pdata)
{
	u8_t  i;
	u8_t  *p = NULL;
	u8_t  gsv_msg_total = 0; /* 信息总数            */
	u8_t  gsv_msg = 0;       /* 信息数              */
	u8_t  gsv_amount_stas;   /* 可见的卫星总数      */
	static u8_t gps_cnt = 0; /* 已解析的GPS卫星颗数 */
	static u8_t bds_cnt = 0; /* 已解析的BDS卫星颗数 */
	static u8_t glo_cnt = 0; /* 已解析的GLO卫星颗数 */
	static u8_t gal_cnt = 0; /* 已解析的GAL卫星颗数 */
    u8_t  sat_cnt = 0;       /* 每条语句中含有的卫星颗数 */
	satellite_sta_t satellite_sta[4]; /* 用于保存每条GSV信息中包含的4颗卫星状态数据 */	
#if MTFS30_DEBUG_EN
    u8_t k = 0;
#endif 


	/* GSV头(不做处理) */
	p = (u8_t *)strtok((char *)pdata, ",");

	/* 信息总数 */
	p = (u8_t *)strtok(NULL, ",");
	gsv_msg_total = *(u8_t*)p - '0';
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("信息总数： %d\n", gsv_msg_total);
#endif 
            


	/* 信息数 */
	p = (u8_t *)strtok(NULL, ",");
	gsv_msg = *(u8_t*)p - '0';
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("信息数： %d\n", gsv_msg);

#endif     

	/* 可见的卫星总数 */
	p = (u8_t *)strtok(NULL, ",");
    gsv_amount_stas = (u8_t)str_to_int32u(p);
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("可见的卫星总数： %d\n", gsv_amount_stas);

#endif       


	/* 解析一条GSV信息中包含的卫星状态信息 */
	for(i = 0; i < 4; i++)
	{
		/* 卫星PRN编号 */
		p = (u8_t *)strtok(NULL, ",");
        if (p == NULL) /* 解析完毕 */
        {
            break;
        }
       
       satellite_sta[i].prn = (u8_t)str_to_int32u(p);
        
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("卫星编号[%d]： %d\n", i, satellite_sta[i].prn);

#endif         

		/* 仰角 */
		p = (u8_t *)strtok(NULL, ",");
        satellite_sta[i].elev = (u8_t)str_to_int32u(p);
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("仰角[%d]： %d\n", i, satellite_sta[i].elev); 

#endif   
		/* 真北方位角(不做处理) */
		p = (u8_t *)strtok(NULL, ",");

		/* 信噪比 */
		p = (u8_t *)strtok(NULL, ",");
        satellite_sta[i].snr = (u8_t)str_to_int32u(p);
         
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("信噪比[%d]： %d\n", i, satellite_sta[i].snr);

#endif   
        /* 每条信息包含的卫星可数加1 */    
        sat_cnt++; 
	}


	/* GPS */
	if (strncmp((char *)pdata, "GPGSV", 5) == 0)
	{
		/* 可见的GPS卫星总数 */
		gsatellite_info.gps_amount_sats = gsv_amount_stas;

		/* 拷贝本条GSV信息中的卫星状态数据 */
		memcpy(&gsatellite_info.gps_status[gps_cnt], satellite_sta, sizeof(satellite_sta_t) * sat_cnt);


		/* 已解析完所有GPS卫星状态信息 */			
		if (gsv_msg == gsv_msg_total)
		{
#if MTFS30_DEBUG_EN
  
         
            for(k = 0; k < gsatellite_info.gps_amount_sats; k++)
            {
                MTFS30_DEBUG("GPS卫星[%d]的卫星PRN编号： %d\n", k, gsatellite_info.gps_status[k].prn);

                MTFS30_DEBUG("GPS卫星[%d]的卫星仰角： %d\n", k, gsatellite_info.gps_status[k].elev);

                MTFS30_DEBUG("GPS卫星[%d]的卫星信噪比： %d\n", k, gsatellite_info.gps_status[k].snr);
            }
#endif  
			gps_cnt = 0;

		} 
		else
		{
			gps_cnt += sat_cnt; /* 已解析的GPS卫星颗数更新，每条GSV信息最多包含4颗卫星 */
		}		
	}
	/* BDS */
	else if(strncmp((char *)pdata, "BDGSV", 5) == 0)
	{
		/* 可见的BDS卫星总数 */
		gsatellite_info.bds_amount_sats = gsv_amount_stas;

		/* 拷贝本条GSV信息中的卫星状态数据 */
		memcpy(&gsatellite_info.bds_status[bds_cnt], satellite_sta, sizeof(satellite_sta_t) * sat_cnt);

		/* 已解析完所有BDS卫星状态信息 */
		if (gsv_msg == gsv_msg_total)
		{
#if MTFS30_DEBUG_EN
  
            for(k = 0; k < gsatellite_info.bds_amount_sats; k++)
            {
                MTFS30_DEBUG("BDS卫星[%d]的卫星PRN编号： %d\n", k, gsatellite_info.bds_status[k].prn); 
                MTFS30_DEBUG("BDS卫星[%d]的卫星仰角： %d\n", k, gsatellite_info.bds_status[k].elev);
                MTFS30_DEBUG("BDSS卫星[%d]的卫星信噪比： %d\n", k, gsatellite_info.bds_status[k].snr);
            }
#endif              
			bds_cnt = 0;	
		} 
		else
		{
			bds_cnt += sat_cnt; /* 已解析的BDS卫星颗数更新，每条GSV信息最多包含4颗卫星 */
		}		
	}
	/* GLO */
	else if(strncmp((char *)pdata, "GLGSV", 5) == 0)
	{
		/* 可见的GLO卫星总数 */
		gsatellite_info.glo_amount_sats = gsv_amount_stas;

		/* 拷贝本条GSV信息中的卫星状态数据 */
		memcpy(&gsatellite_info.glo_status[glo_cnt], satellite_sta, sizeof(satellite_sta_t) * sat_cnt);

		/* 已解析完所有GLO卫星状态信息 */
		if (gsv_msg == gsv_msg_total)
		{
#if MTFS30_DEBUG_EN
  
            
            for(k = 0; k < gsatellite_info.glo_amount_sats; k++)
            {
                MTFS30_DEBUG("GLO卫星[%d]的卫星PRN编号： %d\n", k, gsatellite_info.glo_status[k].prn);

                MTFS30_DEBUG("GLO卫星[%d]的卫星仰角： %d\n", k, gsatellite_info.glo_status[k].elev);

                MTFS30_DEBUG("GLO卫星[%d]的卫星信噪比： %d\n", k, gsatellite_info.glo_status[k].snr);
            }
#endif              
			glo_cnt = 0;	
		} 
		else
		{
			glo_cnt += sat_cnt; /* 已解析的GLO卫星颗数更新，每条GSV信息最多包含4颗卫星 */
		}		
	}
	/* GAL */
	else if(strncmp((char *)pdata, "GAGSV", 5) == 0)
	{
		/* 可见的GAL卫星总数 */
		gsatellite_info.gal_amount_sats = gsv_amount_stas;
        
		/* 拷贝本条GSV信息中的卫星状态数据 */
		memcpy(&gsatellite_info.gal_status[gal_cnt], satellite_sta, sizeof(satellite_sta_t) * sat_cnt);

		/* 已解析完所有GAL卫星状态信息 */
		if (gsv_msg == gsv_msg_total)
		{
#if MTFS30_DEBUG_EN
  
            
            for(k = 0; k < gsatellite_info.gal_amount_sats; k++)
            {
                MTFS30_DEBUG("GAL卫星[%d]的卫星PRN编号： %d\n", k, gsatellite_info.gal_status[k].prn);

                MTFS30_DEBUG("GAL卫星[%d]的卫星仰角： %d\n", k, gsatellite_info.gal_status[k].elev);

                MTFS30_DEBUG("GAL卫星[%d]的卫星信噪比： %d\n", k, gsatellite_info.gal_status[k].snr);
            }
#endif              
			gal_cnt = 0;	
		} 
		else
		{
			gal_cnt += sat_cnt; /* 已解析的GAL卫星颗数更新，每条GSV信息最多包含4颗卫星 */
		}		
	}
	else
	{

	}

}

/*****************************************************************************
 * 函  数:    ut4b0_gpsbds_utc_msg_parse                                                           
 * 功  能:    解析UT4B0接收机输出的UTC信息并生成闰秒预告(GPS/BDS卫星)                                                                 
 * 输  入:    *pdata: UTC信息;                       
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static void ut4b0_gpsbds_utc_msg_parse(u8_t *pdata)
{
	u8_t i;
	u8_t cnt;
	u8_t *p;
	u32_t wn;         /* 新的闰秒生效的周计数         */
	u8_t  dn;         /* 新的闰秒生效的周内日计数     */
	u8_t now_leap;    /* 当前闰秒                     */
	u8_t next_leap;   /* 新的闰秒                     */
	u8_t i_wn;        /* 新的闰秒生效的周计数位置     */



	/* 初始化 */
	i = 0;
	cnt = 0;	
	i_wn = 0;


	/* 略过Log头 */
	while(pdata[i++] != ';');


	while(pdata[i] != '\0')
	{
		if(pdata[i] == ',')
		{
			cnt++;
            if (cnt == 4)
            {
                i_wn = i + 1; /* 新的闰秒生效的周计数位置     */
                break;
            }
		}
        
        i++;
	}

    /* 新的闰秒生效的周计数 */
    p = (u8_t *)strtok((char *)&pdata[i_wn], ",");
    wn = str_to_int32u(p);
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("新的闰秒生效的周计数： %d\n", wn);

#endif
            
    /* 新的闰秒生效的周内日计数 */
    p = (u8_t *)strtok(NULL, ",");
    dn = str_to_int32u(p);
    
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("新的闰秒生效的周内日计数： %d\n", dn);

#endif 
            
    /* 当前闰秒 */
    p = (u8_t *)strtok(NULL, ",");
    now_leap = (u8_t)str_to_int32u(p);
    
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("当前闰秒： %d\n", now_leap);

#endif
            
    /* 新的闰秒 */	        
    p = (u8_t *)strtok(NULL, ",");
    next_leap = (u8_t)str_to_int32u(p);
    
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("新的闰秒： %d\n", next_leap);

#endif            
    

	if (strncmp((char *)pdata, "GPSUTC", 6) == 0)   /* GPS卫星 */
	{
        /* GPS特殊处理 */
        dn = dn - 1; /* 周内日计数 */
        
		/* 生成闰秒预告 */
		get_leap_forecast(SAT_TYPE_GPS, wn, dn, now_leap, next_leap);
	}
	else if (strncmp((char *)pdata, "BDSUTC", 6) == 0)    /* BDS卫星 */
	{
        
        /* BDS特殊处理 */
        now_leap = now_leap + 14;  /* 当前闰秒，以GPS时间为准  */
        next_leap = next_leap + 14;/*  新的闰秒，以GPS时间为准 */
        
		/* 生成闰秒预告 */
		get_leap_forecast(SAT_TYPE_BDS, wn, dn+1, now_leap, next_leap);		
	}



}

/*****************************************************************************
 * 函  数:    ut4b0_gpsbds_utc_msg_parse                                                           
 * 功  能:    解析UT4B0接收机输出的UTC信息并生成闰秒预告(GAL卫星)                                                                 
 * 输  入:    *pdata: UTC信息;                       
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static void ut4b0_gal_utc_msg_parse(u8_t *pdata)
{
	u8_t i;
	u8_t cnt;
	u8_t *p;
	u32_t wn;         /* 新的闰秒生效的周计数         */
	u8_t  dn;         /* 新的闰秒生效的周内日计数     */
	u8_t now_leap;    /* 当前闰秒                     */
	u8_t next_leap;   /* 新的闰秒                     */
	u8_t i_now_leap;  /* 当前闰秒位置                 */



	/* 初始化 */
	i = 0;
	cnt = 0;	
	i_now_leap = 0;


	/* 略过Log头 */
	while(pdata[i++] != ';');


	while(pdata[i++] != '\0')
	{
		if(pdata[i] == ',')
		{
			cnt++;
            if (cnt == 2)
            {
                i_now_leap = i + 1; /* 当前闰秒位置 */
                break;
            }
		}
	}

    /* 当前闰秒 */
    p = (u8_t *)strtok((char *)&pdata[i_now_leap], ",");
	now_leap = (u8_t)str_to_int32u(p);
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("当前闰秒： %d\n", now_leap);

#endif
    
    
    /* UTC 参数的参考时间(不做处理) */
    p = (u8_t *)strtok(NULL, ",");
    
    /* UTC 参考周数(不做处理) */
    p = (u8_t *)strtok(NULL, ",");
 
    
    /* 新的闰秒生效的周计数 */
    p = (u8_t *)strtok(NULL, ",");
    wn = str_to_int32u(p);
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("新的闰秒生效的周计数： %d\n", wn);

#endif
            
    /* 新的闰秒生效的周内日计数 */
    p = (u8_t *)strtok(NULL, ",");
    dn = str_to_int32u(p);    
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("新的闰秒生效的周内日计数： %d\n", dn);

#endif  
    
    /* 新的闰秒 */
    p = (u8_t *)strtok(NULL, ",");
    next_leap = (u8_t)str_to_int32u(p); 
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("新的闰秒： %d\n", next_leap);

#endif     
    

	/* 生成闰秒预告 */
	get_leap_forecast(SAT_TYPE_GAL, wn, dn, now_leap, next_leap);


}

/*****************************************************************************
 * 函  数:    get_leap_forecast                                                           
 * 功  能:    生成闰秒预告                                                                 
 * 输  入:    sat_type  : 卫星类型
 *            wn        ：新的闰秒生效的周计数 
 *            dn        ：新的闰秒生效的周内日计数
 *            now_leap  ：当前闰秒
 *            next_leap ：下次闰秒
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static void get_leap_forecast(u8_t sat_type, u32_t wn, u8_t dn, u8_t now_leap, u8_t next_leap)
{

	u32_t year;   /* 新的闰秒发生年 */ 
	u8_t  mon;    /* 新的闰秒发生月 */
	u8_t  day;    /* 新的闰秒发生日 */
	u32_t days;   /* 新的闰秒发生日距离起始历元的总天数 */
	u8_t  mon_days[12] ={31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; /* 每月天数 */

	switch(sat_type)
	{
		case SAT_TYPE_GPS:    /* GPS卫星 */
			year = 1980; /* GPS时间起始年 */
			mon = 1;     /* GPS时间起始月 */
			day = 6;     /* GPS时间起始日 */
			break;

		case SAT_TYPE_BDS:    /* BDS卫星 */
			year = 2006; /* BDS时间起始年 */      
			mon = 1;     /* BDS时间起始月 */    
			day = 1;     /* BDS时间起始日 */
			break;
            
        case SAT_TYPE_GAL:     /* GAL卫星 */
			year = 1999; /* GAL时间起始年 */      
			mon = 8;     /* GAL时间起始月 */    
			day = 22;    /* GAL时间起始日 */ 
            break;
            
        default:            
            break;
	}


	/*-------------------------------*/
	/* 计算新的闰秒发生的年月日      */
	/*-------------------------------*/	 

	/* 新的闰秒发生日距离起始历元的总天数 */
	days = wn * 7 + dn + day;

	while(days > mon_days[mon-1])
	{

		/* 天数超过当月的总天数，则进入下一个月 */
		days -= mon_days[mon-1];
		mon++;

		/* 超过12月进入下一年 */
		if (mon == 13)
		{
			year++;

			/* 判定闰年还是平年 */
			if ((year % 400 == 0) || (year % 100 != 0 && year % 4 == 0))
			{
				mon_days[1] = 29; /* 闰年的2月为29天 */
			}
			else 
			{
				mon_days[1] = 28; /* 平年的2月为28天 */
			}

			mon = 1; /* 新的一年，月份重新从1月开始 */
		}


	}

	/* 新的闰秒发生日 */
	day = days; 


	/*-------------------------------*/
	/* 生成闰秒预告                  */
	/*-------------------------------*/	

	/* 初始化 */
	memset(&gleap_forecast, 0x00, sizeof(gleap_forecast));	 
	/* 新的闰秒发生年 */
	sprintf((char *)gleap_forecast.year, "%d", year);
	/* 新的闰秒发生月 */
	sprintf((char *)gleap_forecast.mon, "%d", mon);
	/* 新的闰秒发生日 */
	sprintf((char *)gleap_forecast.day, "%d", day);
	/* 新的闰秒值 */
	gleap_forecast.next_leap = next_leap;
	/* 正/负闰秒 */
	gleap_forecast.sign = next_leap - now_leap;

	/* 闰秒发生时分秒 */
	if (gleap_forecast.sign >= 0)/* 正闰秒 */
	{
		sprintf((char *)gleap_forecast.hms, "%s", "075960"); /* 北京时间正闰秒发生时间为: 07:59:60 */
	}
	else
	{
		sprintf((char *)gleap_forecast.hms, "%s", "075958"); /* 北京时间负闰秒发生时间为: 23:59:58 */
	}
    
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("新的闰秒发生年： %s\n", gleap_forecast.year);
            MTFS30_DEBUG("新的闰秒发生月： %s\n", gleap_forecast.mon);
            MTFS30_DEBUG("新的闰秒发生日： %s\n", gleap_forecast.day);
            MTFS30_DEBUG("新的闰秒值： %d\n", gleap_forecast.next_leap);
            MTFS30_DEBUG("正/负闰秒： %d\n", gleap_forecast.sign);
            MTFS30_DEBUG("闰秒发生时分秒： %s\n", gleap_forecast.hms);
                        
#endif         

}

/*****************************************************************************
 * 函  数:    ut4b0_1pps_msg_parse                                                           
 * 功  能:    解析UT4B0接收机输出的1PPS信息                                                                
 * 输  入:    *pdata: UTC信息;                       
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static void ut4b0_1pps_msg_parse(u8_t *pdata)
{

    u8_t pps_id;
    u8_t *p = NULL;
    
    
    
	/* 略过Log头 */
	p = (u8_t *)strtok((char *)pdata, ";");

	/* 1PPS ID */
	p = (u8_t *)strtok(NULL, ",");
	pps_id = *(u8_t*)p - '0';
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("1PPS ID： %d\n", pps_id);          
#endif 
            
    /* 1PPS状态以ID 0为准 */        
    if (pps_id != 0)
    {
        return ;
    }        
    
    /* 1PPS状态 */
    p = (u8_t *)strtok(NULL, ",");  
    grecviver_info.pps_status = *(u8_t*)p - '0';
    
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("1PPS状态： %d\n", grecviver_info.pps_status);          
#endif     
    
}

/*****************************************************************************
 * 函  数:    ut4b0_receiver_pps_set                                                          
 * 功  能:    pps配置                                                                 
 * 输  入:    *pps_info: pps配置信息                         
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void ut4b0_receiver_pps_set(pps_info_t *pps_info)
{
	u8_t buf[128] = {0};


	/* 做成pps配置命令，校验码暂时用00代替 */
	sprintf((char *)buf, "CONFIG PPS %s %s %s %d %d %d %d*00\r\n",
			pps_info->en_switch, pps_info->timeref, pps_info->polatity,
			pps_info->width, pps_info->period, pps_info->rfdelay, pps_info->userdelay);


	/* 使用串口1发送命令 */
#if TODO 
	usart_send(buf, USART_COM_1);
#endif	

}

/*****************************************************************************
 * 函  数:    ut4b0_receiver_reset                                                           
 * 功  能:    按指定方式重启UT4B0接收机                                                              
 * 输  入:    reset_mode: 
REV_RESETMODE_ALL        : 清除保存的设置，卫星星历、位置信息等再重启
REV_RESETMODE_CONFIGSAVE : 保存用户配置再重启          
REV_RESETMODE_EPHEM      : 清除保存的卫星星历再重启   
REV_RESETMODE_POSITION   : 清除保存的位置信息再重启
REV_RESETMODE_ALMANAC    : 清除保存的历书信息再重启 
REV_RESETMODE_IONUTC     : 清除电离层和 UTC 参数再重启
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void ut4b0_receiver_reset(u8_t reset_mode)
{
	u8_t buf[15] = {0};

	switch(reset_mode)
	{
		case REV_RESETMODE_ALL:           /* 清除保存的设置，卫星星历、位置信息等再重启 */
			strcpy((char *)buf, "FRESET");
			break;

		case REV_RESETMODE_CONFIGSAVE:    /* 保存用户配置再重启          */
			strcpy((char *)buf, "RESET");
			break;

		case REV_RESETMODE_EPHEM:         /* 清除保存的卫星星历再重启    */
			strcpy((char *)buf, "RESET EPHEM");
			break;

		case REV_RESETMODE_POSITION:      /* 清除保存的位置信息再重启    */
			strcpy((char *)buf, "RESET POSITION");
			break;

		case REV_RESETMODE_ALMANAC:       /* 清除保存的历书信息再重启    */
			strcpy((char *)buf, "RESET ALMANAC");
			break;

		case REV_RESETMODE_IONUTC:        /* 清除电离层和 UTC 参数再重启 */
			strcpy((char *)buf, "RESET IONUTC");
			break;

		default:
			break;
	}

	/* 使用串口1发送命令 */
#if TODO 
	usart_send(buf, USART_COM_1);
#endif	

}


