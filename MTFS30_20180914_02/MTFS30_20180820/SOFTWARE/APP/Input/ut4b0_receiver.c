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
extern location_msg_t   glocation;
extern sate_status_t    gsate_status;
extern leap_forecast_msg_t gleap_forecast_msg;;

extern ringbuffer_t pgusart1_rb;                  /* 串口1消息缓冲区 */
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
    {(u8_t *)"GPGGA",     ut4b0_gga_msg_parse},
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


	/* 禁止所有信息输出 */
	usart_send("unlog", 1); /* 使用串口1发送命令 */			     


	/* 判断工作模式 */
	switch(mode)
	{
		case REV_ACMODE_GPS:    /* 单GPS定位 */
			/* GPSGGA */
			usart_send("GPSGGA 1", 1); /* 使用串口1发送命令 */		

			/* GPSUTC */
			usart_send("GPSUTC 1", 1); /* 使用串口1发送命令 */				
			break;

		case REV_ACMODE_BDS:    /* 单BDS定位 */
			/* BDSGGA */
			usart_send("BDSGGA 1", 1); /* 使用串口1发送命令 */

			/* BDSUTC */
			usart_send("BDSUTC 1", 1); /* 使用串口1发送命令 */	
			break;

		case REV_ACMODE_GLO:    /* 单GLO定位 */
			/* GLOGGA */
			usart_send("GLOGGA 1", 1); /* 使用串口1发送命令 */			    
			break;

		case REV_ACMODE_GAL:    /* 单GAL定位 */
			/* GALGGA */
			usart_send("GALGGA 1", 1); /* 使用串口1发送命令 */	

			/* GALUTC */
			usart_send("GALUTC 1", 1); /* 使用串口1发送命令 */				
			break;

		case REV_ACMODE_ALL:    /* 多系统联合定位 */
			/* GNGGA */
			usart_send("GNGGA 1", 1); /* 使用串口1发送命令 */
 
#if MTFS30_DEBUG_EN_01
            MTFS30_DEBUG("%s\n", "GNGGA 1");      
#endif              

//			/* GPSUTC(多系统联合输出时以GPS UTC时间为准) */ 
//			memset(buf, 0x00, sizeof(buf));
//			strcpy((char *)buf, "GPSUTC 1");
//			usart_send(buf, 1); /* 使用串口1发送命令 */	
            /* GPSUTC(多系统联合输出时以GPS UTC时间为准) */ 
			usart_send("BDSUTC 1", 1); /* 使用串口1发送命令 */	
#if MTFS30_DEBUG_EN_01
            MTFS30_DEBUG("%s\n", "BDSUTC 1");    
#endif                       
			break;			

		default:
#if MTFS30_ERROR_EN
			MTFS30_ERROR("接收机工作模式(%d)有误", mode);
#endif			
			break;
	}
   

	/* GPGSV */
	usart_send("GPGSV 1", 1); /* 使用串口1发送命令 */	
#if MTFS30_DEBUG_EN_01
    MTFS30_DEBUG("%s\n", "GPGSV 1");      
#endif  
     /* 1PPS */
	usart_send("BBPPSMSGA ONCHANGED", 1); /* 使用串口1发送命令 */	
    
#if MTFS30_DEBUG_EN_01
    MTFS30_DEBUG("%s\n", "BBPPSMSGA ONCHANGED");   
#endif

#ifdef TODO	
	/* ANTENNA */
	usart_send("ANTENNA 1", 1); /* 使用串口1发送命令 */	
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

	/* 判断接收机工作模式 */
	switch(mode)
	{
		case REV_ACMODE_GPS:    /* 单GPS定位 */	

			/* 禁用BDS */      
			usart_send("MASK BDS", 1); /* 使用串口1发送命令 */

			/* 禁用GLO */      
			usart_send("MASK GLO", 1); /* 使用串口1发送命令 */		

			/* 禁用GAL */       
			usart_send("MASK GAL", 1); /* 使用串口1发送命令 */	

			/* 启用GPS */      
			usart_send("UNMASK GPS", 1); /* 使用串口1发送命令 */				
			break;

		case REV_ACMODE_BDS:    /* 单BDS定位 */
			/* 禁用GPS */      
			usart_send("MASK GPS", 1); /* 使用串口1发送命令 */

			/* 禁用GLO */        
			usart_send("MASK GLO", 1); /* 使用串口1发送命令 */		

			/* 禁用GAL */      
			usart_send("MASK GAL", 1); /* 使用串口1发送命令 */	

			/* 启用GPS */        
			usart_send("UNMASK BDS", 1); /* 使用串口1发送命令 */	
			break;

		case REV_ACMODE_GLO:    /* 单GLO定位 */
			/* 禁用GPS */        
			usart_send("MASK GPS", 1); /* 使用串口1发送命令 */

			/* 禁用BDS */        
			usart_send("MASK BDS", 1); /* 使用串口1发送命令 */		

			/* 禁用GAL */        
			usart_send("MASK GAL", 1); /* 使用串口1发送命令 */	

			/* 启用GLO */        
			usart_send("UNMASK GLO", 1); /* 使用串口1发送命令 */	
			break;   

		case REV_ACMODE_GAL:    /* 单GAL定位 */
			/* 禁用GPS */       
			usart_send("MASK GPS", 1); /* 使用串口1发送命令 */

			/* 禁用BDS */        
			usart_send("MASK BDS", 1); /* 使用串口1发送命令 */		

			/* 禁用GLO */        
			usart_send("MASK GLO", 1); /* 使用串口1发送命令 */	

			/* 启用GAL */        
			usart_send("UNMASK GAL", 1); /* 使用串口1发送命令 */	
			break;

		case REV_ACMODE_ALL:   /* 多系统联合定位  */
			/* 启用GPS */
			usart_send("UNMASK GPS", 1); /* 使用串口1发送命令 */           
//#if MTFS30_DEBUG_EN
//            MTFS30_DEBUG("%s\n", "UNMASK GPS");           
//#endif               

			/* 启用BDS */        
			usart_send("UNMASK BDS", 1); /* 使用串口1发送命令 */
//#if MTFS30_DEBUG_EN
//            MTFS30_DEBUG("%s\n", "UNMASK BDS");          
//#endif                

			/* 启用GLO */
			usart_send("UNMASK GLO", 1); /* 使用串口1发送命令 */
//#if MTFS30_DEBUG_EN
//            MTFS30_DEBUG("%s\n", "UNMASK GLO");          
//#endif                

			/* 启用GAL */        
			usart_send("UNMASK GAL", 1); /* 使用串口1发送命令 */
//#if MTFS30_DEBUG_EN
//            MTFS30_DEBUG("%s\n", "UNMASK GAL");          
//#endif                
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
    u8_t msg_chck_type; /* 校验类型 */

//#if MTFS30_DEBUG_EN_01
//            MTFS30_DEBUG("pdata: %s\n", pdata);          
//#endif    
            
	while(*p != '\0')
	{
		/* $表示一条数据输出指令开始 */
		if (*p == '$' || *p == '#')
		{
              
            /* 检查该条数据指令是否完整
             * 主要检查是否存在"*","\r","\n"
             */
            if ((strchr((char *)p, '*' ) == NULL) ||
                (strchr((char *)p, '\r') == NULL) ||
                (strchr((char *)p, '\n') == NULL))
            {
                /* 指令无效，退出 */
#if MTFS30_ERROR_EN
			MTFS30_ERROR("指令无效(%s)\n", p);
#endif	                
                break;
            }
            
            /* 截取出真正的数据部分 */           			
            p++; /* 从$后第一个字节开始截取 */ 
            pmsg = (u8_t*)strtok((char *)p, "*");
//#if MTFS30_DEBUG_EN_01_1
//            MTFS30_DEBUG("pmsg: %s\n", pmsg);          
//#endif               
            
            /* 截取出校验码 */
            pcode =(u8_t*)strtok(NULL, "\r");
//#if MTFS30_DEBUG_EN_01
//            MTFS30_DEBUG("pcode: %s\n", pcode);               
//#endif      
            /* 区分校验类型 */
            if ((strncmp("GPSUTC", (char *)pmsg, 6) == 0) || 
                (strncmp("BDSUTC", (char *)pmsg, 6) == 0) ||
                (strncmp("GALUTC", (char *)pmsg, 6) == 0) ||
                (strncmp("BBPPSMSGAA", (char *)pmsg, 10) == 0))
            {
                msg_chck_type = CHECK_TYPE_CRC32; /* CRC32校验 */
//#if MTFS30_DEBUG_EN_01
//            MTFS30_DEBUG("msg_chck_type: CHECK_TYPE_CRC32\n");   
//            
//#endif                  
            } 
            else
            {
                msg_chck_type = CHECK_TYPE_XOR; /* 异或校验 */
//#if MTFS30_DEBUG_EN_01
//            MTFS30_DEBUG("msg_chck_type: CHECK_TYPE_XOR\n");   
//            
//#endif                  
            }
                    
            
            /* 比较校验码是否一致 */
            if (CHECKCODE_EQ == ut4b0_checkcode_compare(pmsg, pcode, msg_chck_type))
            {
//#if MTFS30_DEBUG_EN_01
//            MTFS30_DEBUG("CHECKCODE_EQ\n");          
//#endif      
                
                /* 特殊处理 */
                if ((strncmp("GPGSV", (char *)pmsg, 5) != 0) &&
                    (strncmp("BDGSV", (char *)pmsg, 5) != 0) &&
                    (strncmp("GLGSV", (char *)pmsg, 5) != 0) &&
                    (strncmp("GAGSV", (char *)pmsg, 5) != 0))
                {
                    
                    gsate_status.status_msg[gsate_status.msg_len - 1] = ';';
                    gsate_status.status_msg[gsate_status.msg_len] = '\0';
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("状态信息： %s\t状态信息长度 = %d\n", gsate_status.status_msg, gsate_status.msg_len);     
#endif                       
                    gsate_status.msg_len = 0;
                
                }
                /* 查找并执行该条数据指令的解析函数 */
			    ut4b0_check_cmdhandler(pmsg);         
            }
            
            break;           
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
    u8_t i = 0; 
    u8_t comma_cnt = 0; /* 逗号个数             */
    u8_t len = 0;       /* 保存的消息长度       */
    u8_t param_len = 0;  /* 保存每个参数的长度  */
    u8_t pre_comma_index; /* 上一次逗号的位置   */
    
    
    /* 初始化 */
    glocation.location[0] = '\0';
      
    /* 解析每个参数 */
    while(pdata[i])
    {
        if (pdata[i] == ',')
        {
            comma_cnt++;
            
            
            switch(comma_cnt)
            {
            case 2:    /* UTC时间        */
            case 3:
            case 4:
            case 5:
            case 6:
            case 8:
                memcpy(&glocation.location[len], &pdata[pre_comma_index+1], param_len + 1);
                len += param_len + 1;

                break;
            case 10:
                memcpy(&glocation.location[len], &pdata[pre_comma_index+1], param_len);
                len += param_len;
                glocation.location[len++] = ';';
                glocation.location[len] = '\0';
                
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("定位信息： %s\t定位信息[0] = %d\n", glocation.location, glocation.location[0]);     
#endif                  
                return;      
                break;
            }    
            
            pre_comma_index = i;
            param_len = 0;
        }
        else 
        {
            ///* 临时保存每个参数 */
            //buf[len++] = pdata[i];
            param_len++;
        }
        
        i++;
    }    

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



    
    u8_t i = 0; 
    u8_t comma_cnt = 0; /* 逗号个数             */
    u8_t param_len = 0;  /* 保存每个参数的长度  */
    u8_t pre_comma_index; /* 上一次逗号的位置   */
    u8_t cmd_buf[6] = {0};
    u8_t  gsv_msg = 0;           /* 信息数               */    

    
    /* 初始化 */   
    if (gsate_status.msg_len == 0)
    {
        gsate_status.status_msg[gsate_status.msg_len - 1] = '\0';        
    }



    
    pdata[strlen((char *)pdata)] = ',';
    my_strtok(',', (char *)pdata, (char *)cmd_buf, sizeof(cmd_buf));
 
    
    /* 解析每个参数 */
    while(pdata[i])
    {
        if (pdata[i] == ',')
        {
            comma_cnt++;
            
            
            switch(comma_cnt)
            {
            case 3:
                memcpy(&gsv_msg, &pdata[pre_comma_index+1], param_len);
                if (gsv_msg == '1')
                {
 
                    strncpy((char *)&gsate_status.status_msg[gsate_status.msg_len], (char *)cmd_buf, strlen((char *)cmd_buf));
                    gsate_status.msg_len += strlen((char *)cmd_buf);
                    gsate_status.status_msg[gsate_status.msg_len] = ',';
                    gsate_status.msg_len++;
                }
                break;
            case 4:
                if (gsv_msg == '1')
                {
                    memcpy(&gsate_status.status_msg[gsate_status.msg_len], &pdata[pre_comma_index+1], param_len + 1);
                    gsate_status.msg_len += param_len + 1;
                    
                }
                break;
            case 5:
            case 6:
            case 8:
            case 9:
            case 10:
            case 12:
            case 13:         
            case 14:
            case 16:
            case 17:
            case 18:
            case 20:                   
                memcpy(&gsate_status.status_msg[gsate_status.msg_len], &pdata[pre_comma_index+1], param_len + 1);
                gsate_status.msg_len += param_len + 1;
                
                break;
            }    
            
            pre_comma_index = i;
            param_len = 0;
        }
        else 
        {
            param_len++;
        }
        
        i++;
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


    u8_t i = 0; 
    u8_t comma_cnt = 0; /* 逗号个数             */
    u8_t param_len = 0;  /* 保存每个参数的长度  */
    u8_t param_buf[20]= {0};
	u32_t wn = 0;         /* 新的闰秒生效的周计数         */
	u8_t  dn = 0;         /* 新的闰秒生效的周内日计数     */
	u8_t now_leap = 0;    /* 当前闰秒                     */
	u8_t next_leap = 0;   /* 新的闰秒                     */

    
  
    /* 略过Log头 */
	while(pdata[i++] != ';');
      
    /* 解析每个参数 */
    while(pdata[i])
    {
        if (pdata[i] == ',')
        {
            comma_cnt++;
            
            param_buf[param_len]='\0';
            switch(comma_cnt)
            {
            case 5:    /* UTC时间        */
                wn = str_to_int32u(param_buf);
                break;
            case 6:
                dn = (u8_t)str_to_int32u(param_buf);
                break;
            case 7:
                now_leap = (u8_t)str_to_int32u(param_buf);
                break;
            case 8:
                next_leap = (u8_t)str_to_int32u(param_buf);
               
                goto LEAP;
                break;
            }    
            
            param_len = 0;
        }
        else 
        {
            param_buf[param_len++] = pdata[i];
        }
        
        i++;
    } 
    

    
LEAP:
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("新的闰秒生效的周计数： %d\n新的闰秒生效的周内日计数： %d\n当前闰秒： %d\n新的闰秒： %d\n", 
                         wn, dn, now_leap,next_leap);

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
 * 函  数:    ut4b0_gal_utc_msg_parse                                                           
 * 功  能:    解析UT4B0接收机输出的UTC信息并生成闰秒预告(GAL卫星)                                                                 
 * 输  入:    *pdata: UTC信息;                       
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static void ut4b0_gal_utc_msg_parse(u8_t *pdata)
{


    u8_t i = 0; 
    u8_t comma_cnt = 0; /* 逗号个数             */
    u8_t param_len = 0;  /* 保存每个参数的长度  */
    u8_t param_buf[20]= {0};
	u32_t wn = 0;         /* 新的闰秒生效的周计数         */
	u8_t  dn = 0;         /* 新的闰秒生效的周内日计数     */
	u8_t now_leap = 0;    /* 当前闰秒                     */
	u8_t next_leap = 0;   /* 新的闰秒                     */

    
    
    /* 略过Log头 */
	while(pdata[i++] != ';');
      
    /* 解析每个参数 */
    while(pdata[i])
    {
        if (pdata[i] == ',')
        {
            comma_cnt++;
            
            param_buf[param_len]='\0';
            switch(comma_cnt)
            {
            case 3:    /* UTC时间        */
                now_leap = (u8_t)str_to_int32u(param_buf);
                break;
            case 6:
                wn = str_to_int32u(param_buf);
                break;
            case 7:
                dn = (u8_t)str_to_int32u(param_buf);
                break;
            case 8:
                next_leap = (u8_t)str_to_int32u(param_buf);
               
                goto LEAP;
                break;
            }    
            
            param_len = 0;
        }
        else 
        {
            param_buf[param_len++] = pdata[i];
        }
        
        i++;
    } 
    
    
LEAP:
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("GAL 新的闰秒生效的周计数： %d\n新的闰秒生效的周内日计数： %d\n当前闰秒： %d\n新的闰秒： %d\n", 
                         wn, dn, now_leap,next_leap);

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

    
    gleap_forecast_msg.leap_msg[0] = '\0';
    
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
        sprintf((char *)gleap_forecast_msg.leap_msg, "%d,%d,%d,%s,%d,%d;", year, mon, day, "075960", next_leap, next_leap - now_leap);
	}
	else
	{
        sprintf((char *)gleap_forecast_msg.leap_msg, "%d,%d,%d,%s,%d,%d;", year, mon, day, "075958", next_leap, next_leap - now_leap);
	}
    
#if MTFS30_DEBUG_EN
    MTFS30_DEBUG("闰秒预告:%s\n", gleap_forecast_msg.leap_msg);
                        
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
    u8_t i = 0; 
    u8_t comma_cnt = 0; /* 逗号个数             */
    u8_t param_len = 0;  /* 保存每个参数的长度  */
    u8_t param_buf[20]= {0};
    u8_t pps_id;
    u8_t pps_status;
    

    
    /* 略过Log头 */
	while(pdata[i++] != ';');
      
    /* 解析每个参数 */
    while(pdata[i])
    {
        if (pdata[i] == ',')
        {
            comma_cnt++;
            
            param_buf[param_len]='\0';
            switch(comma_cnt)
            {
            case 1:    /* UTC时间        */
                pps_id = (u8_t)str_to_int32u(param_buf);
                break;
            case 2:
                if (pps_id != 0)
                {
                    return ;
                }
                pps_status = str_to_int32u(param_buf);
#if MTFS30_DEBUG_EN
    MTFS30_DEBUG("1pps ID[%d]状态为:%d;\n", pps_id, pps_status);                       
#endif                     
                return;
                break;
            }    
            
            param_len = 0;
        }
        else 
        {
            param_buf[param_len++] = pdata[i];
        }
        
        i++;
    } 


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
	sprintf((char *)buf, "CONFIG PPS %s %s %s %d %d %d %d",
			pps_info->en_switch, pps_info->timeref, pps_info->polatity,
			pps_info->width, pps_info->period, pps_info->rfdelay, pps_info->userdelay);


	/* 使用串口1发送命令 */
	usart_send(buf, USART_COM_1);
#if MTFS30_DEBUG_EN
    MTFS30_DEBUG("1pps配置:%s\n", buf);                       
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


