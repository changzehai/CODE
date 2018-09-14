/*****************************************************************************/
/* �ļ���:    ut4b0_receiver.c                                               */
/* ��  ��:    UT4B0���ջ���ش���                                            */
/* ��  ��:    2018-07-20 changzehai(DTT)                                     */
/* ��  ��:    ��                                                             */
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
/* ��������                      */
/*-------------------------------*/


/*-------------------------------*/
/* ��������                      */
/*-------------------------------*/
extern satellite_info_t gsatellite_info; /* �����ۺ���Ϣ */
extern recviver_info_t  grecviver_info;  /* ���ջ���Ϣ   */
extern leap_forecast_t  gleap_forecast;  /* ����Ԥ��     */
extern ttime_t           gtime;           /* ʱ����Ϣ     */ 
extern location_msg_t   glocation;
extern sate_status_t    gsate_status;
extern leap_forecast_msg_t gleap_forecast_msg;;

extern ringbuffer_t pgusart1_rb;                  /* ����1��Ϣ������ */
/*-------------------------------*/
/* ��������                      */
/*-------------------------------*/
static void ut4b0_check_cmdhandler(u8_t *pdata);
static void ut4b0_gga_msg_parse(u8_t *pdata);
static void ut4b0_gsv_msg_parse(u8_t *pdata);
static void ut4b0_gpsbds_utc_msg_parse(u8_t *pdata);
static void ut4b0_gal_utc_msg_parse(u8_t *pdata);
static void get_leap_forecast(u8_t sat_type, u32_t wn, u8_t dn, u8_t now_leap, u8_t next_leap);
static u8_t ut4b0_checkcode_compare(u8_t *pmsg, u8_t *pcode, u8_t check_type);
static void ut4b0_1pps_msg_parse(u8_t *pdata);





/* ut4b0���ջ��������ָ���� */
static cmdhandle_t ut4b0_cmdhandle[] =
{
	/* GGA��Ϣ */
	{(u8_t *)"GNGGA",      ut4b0_gga_msg_parse},
	{(u8_t *)"GPSGGA",     ut4b0_gga_msg_parse},
    {(u8_t *)"GPGGA",     ut4b0_gga_msg_parse},
	{(u8_t *)"BDSGGA",     ut4b0_gga_msg_parse},
	{(u8_t *)"GLOGGA",     ut4b0_gga_msg_parse},
	{(u8_t *)"GALGGA",     ut4b0_gga_msg_parse},

	/* GSV��Ϣ */
	{(u8_t *)"GPGSV",      ut4b0_gsv_msg_parse},
	{(u8_t *)"BDGSV",      ut4b0_gsv_msg_parse},
	{(u8_t *)"GLGSV",      ut4b0_gsv_msg_parse},
	{(u8_t *)"GAGSV",      ut4b0_gsv_msg_parse},	

	/* UTC��Ϣ */
	{(u8_t *)"GPSUTC",     ut4b0_gpsbds_utc_msg_parse},
	{(u8_t *)"BDSUTC",     ut4b0_gpsbds_utc_msg_parse},
	{(u8_t *)"GALUTC",     ut4b0_gal_utc_msg_parse},
    
    /* 1PPS */
    {(u8_t *)"BBPPSMSGAA",     ut4b0_1pps_msg_parse},

};

/*****************************************************************************
 * ��  ��:    ut4b0_receiver_init                                                           
 * ��  ��:    ut4b0���ջ���ʼ��                                                                
 * ��  ��:     *pdata: ��������                         
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/  
void ut4b0_receiver_init(void)
{
	pps_info_t pps_info; /* ���pps������Ϣ */


	/* ����ut4b0���ջ�����ģʽ */
	ut4b0_receiver_acmode_set(REV_ACMODE_ALL);

	/* ����ut4b0���ջ������������� */
	ut4b0_receiver_msg_set(REV_ACMODE_ALL);



	/* pps��ʼ���� */
	memset(&pps_info, 0x00, sizeof(pps_info));
	strcpy((char *)pps_info.en_switch, "ENABLE");  /* ʹ�� PPS �������ֻ��ʱ����Чʱ��� PPS �źţ�������ʧ�������ջ�����λʱ���� PPS �ź������ */
	strcpy((char *)pps_info.timeref, "GPS");      
	strcpy((char *)pps_info.polatity, "POSITIVE"); /* ��������Ч */
	pps_info.width = 500000;                       /* ������Ϊ500���� */	  
	pps_info.period = 1000;                        /* pps����Ϊ1��      */ 
	pps_info.rfdelay = 0;                          /* RF�ӳ�0����       */
	pps_info.userdelay = 0;                        /* �û��ӳ�0����     */

	/* ����pps */
	ut4b0_receiver_pps_set(&pps_info);



}

/*****************************************************************************
 * ��  ��:    ut4b0_receiver_msg_set                                                           
 * ��  ��:    ����ut4b0���ջ�������������                                                               
 * ��  ��:    mode: 
 *                 REV_ACMODE_GPS: ��GPS��λ
 *                 REV_ACMODE_BDS: ��BDS��λ
 *                 REV_ACMODE_GLO: ��GLO��λ
 *                 REV_ACMODE_GAL: ��GAL��λ
 *                 REV_ACMODE_ALL: ��ϵͳ���϶�λ                        
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/ 
void ut4b0_receiver_msg_set(u8_t mode)
{


	/* ��ֹ������Ϣ��� */
	usart_send("unlog", 1); /* ʹ�ô���1�������� */			     


	/* �жϹ���ģʽ */
	switch(mode)
	{
		case REV_ACMODE_GPS:    /* ��GPS��λ */
			/* GPSGGA */
			usart_send("GPSGGA 1", 1); /* ʹ�ô���1�������� */		

			/* GPSUTC */
			usart_send("GPSUTC 1", 1); /* ʹ�ô���1�������� */				
			break;

		case REV_ACMODE_BDS:    /* ��BDS��λ */
			/* BDSGGA */
			usart_send("BDSGGA 1", 1); /* ʹ�ô���1�������� */

			/* BDSUTC */
			usart_send("BDSUTC 1", 1); /* ʹ�ô���1�������� */	
			break;

		case REV_ACMODE_GLO:    /* ��GLO��λ */
			/* GLOGGA */
			usart_send("GLOGGA 1", 1); /* ʹ�ô���1�������� */			    
			break;

		case REV_ACMODE_GAL:    /* ��GAL��λ */
			/* GALGGA */
			usart_send("GALGGA 1", 1); /* ʹ�ô���1�������� */	

			/* GALUTC */
			usart_send("GALUTC 1", 1); /* ʹ�ô���1�������� */				
			break;

		case REV_ACMODE_ALL:    /* ��ϵͳ���϶�λ */
			/* GNGGA */
			usart_send("GNGGA 1", 1); /* ʹ�ô���1�������� */
 
#if MTFS30_DEBUG_EN_01
            MTFS30_DEBUG("%s\n", "GNGGA 1");      
#endif              

//			/* GPSUTC(��ϵͳ�������ʱ��GPS UTCʱ��Ϊ׼) */ 
//			memset(buf, 0x00, sizeof(buf));
//			strcpy((char *)buf, "GPSUTC 1");
//			usart_send(buf, 1); /* ʹ�ô���1�������� */	
            /* GPSUTC(��ϵͳ�������ʱ��GPS UTCʱ��Ϊ׼) */ 
			usart_send("BDSUTC 1", 1); /* ʹ�ô���1�������� */	
#if MTFS30_DEBUG_EN_01
            MTFS30_DEBUG("%s\n", "BDSUTC 1");    
#endif                       
			break;			

		default:
#if MTFS30_ERROR_EN
			MTFS30_ERROR("���ջ�����ģʽ(%d)����", mode);
#endif			
			break;
	}
   

	/* GPGSV */
	usart_send("GPGSV 1", 1); /* ʹ�ô���1�������� */	
#if MTFS30_DEBUG_EN_01
    MTFS30_DEBUG("%s\n", "GPGSV 1");      
#endif  
     /* 1PPS */
	usart_send("BBPPSMSGA ONCHANGED", 1); /* ʹ�ô���1�������� */	
    
#if MTFS30_DEBUG_EN_01
    MTFS30_DEBUG("%s\n", "BBPPSMSGA ONCHANGED");   
#endif

#ifdef TODO	
	/* ANTENNA */
	usart_send("ANTENNA 1", 1); /* ʹ�ô���1�������� */	
#endif	
    


}

/*****************************************************************************
 * ��  ��:    ut4b0_receiver_acmode_set                                                           
 * ��  ��:    ����ut4b0���ջ�����ģʽ                                                              
 * ��  ��:    mode: 
 *                 REV_ACMODE_GPS: ��GPS��λ
 *                 REV_ACMODE_BDS: ��BDS��λ
 *                 REV_ACMODE_GLO: ��GLO��λ
 *                 REV_ACMODE_GAL: ��GAL��λ
 *                 REV_ACMODE_ALL: ��ϵͳ���϶�λ                        
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/ 
void ut4b0_receiver_acmode_set(u8_t mode)
{

	/* �жϽ��ջ�����ģʽ */
	switch(mode)
	{
		case REV_ACMODE_GPS:    /* ��GPS��λ */	

			/* ����BDS */      
			usart_send("MASK BDS", 1); /* ʹ�ô���1�������� */

			/* ����GLO */      
			usart_send("MASK GLO", 1); /* ʹ�ô���1�������� */		

			/* ����GAL */       
			usart_send("MASK GAL", 1); /* ʹ�ô���1�������� */	

			/* ����GPS */      
			usart_send("UNMASK GPS", 1); /* ʹ�ô���1�������� */				
			break;

		case REV_ACMODE_BDS:    /* ��BDS��λ */
			/* ����GPS */      
			usart_send("MASK GPS", 1); /* ʹ�ô���1�������� */

			/* ����GLO */        
			usart_send("MASK GLO", 1); /* ʹ�ô���1�������� */		

			/* ����GAL */      
			usart_send("MASK GAL", 1); /* ʹ�ô���1�������� */	

			/* ����GPS */        
			usart_send("UNMASK BDS", 1); /* ʹ�ô���1�������� */	
			break;

		case REV_ACMODE_GLO:    /* ��GLO��λ */
			/* ����GPS */        
			usart_send("MASK GPS", 1); /* ʹ�ô���1�������� */

			/* ����BDS */        
			usart_send("MASK BDS", 1); /* ʹ�ô���1�������� */		

			/* ����GAL */        
			usart_send("MASK GAL", 1); /* ʹ�ô���1�������� */	

			/* ����GLO */        
			usart_send("UNMASK GLO", 1); /* ʹ�ô���1�������� */	
			break;   

		case REV_ACMODE_GAL:    /* ��GAL��λ */
			/* ����GPS */       
			usart_send("MASK GPS", 1); /* ʹ�ô���1�������� */

			/* ����BDS */        
			usart_send("MASK BDS", 1); /* ʹ�ô���1�������� */		

			/* ����GLO */        
			usart_send("MASK GLO", 1); /* ʹ�ô���1�������� */	

			/* ����GAL */        
			usart_send("UNMASK GAL", 1); /* ʹ�ô���1�������� */	
			break;

		case REV_ACMODE_ALL:   /* ��ϵͳ���϶�λ  */
			/* ����GPS */
			usart_send("UNMASK GPS", 1); /* ʹ�ô���1�������� */           
//#if MTFS30_DEBUG_EN
//            MTFS30_DEBUG("%s\n", "UNMASK GPS");           
//#endif               

			/* ����BDS */        
			usart_send("UNMASK BDS", 1); /* ʹ�ô���1�������� */
//#if MTFS30_DEBUG_EN
//            MTFS30_DEBUG("%s\n", "UNMASK BDS");          
//#endif                

			/* ����GLO */
			usart_send("UNMASK GLO", 1); /* ʹ�ô���1�������� */
//#if MTFS30_DEBUG_EN
//            MTFS30_DEBUG("%s\n", "UNMASK GLO");          
//#endif                

			/* ����GAL */        
			usart_send("UNMASK GAL", 1); /* ʹ�ô���1�������� */
//#if MTFS30_DEBUG_EN
//            MTFS30_DEBUG("%s\n", "UNMASK GAL");          
//#endif                
			break;

		default:
#if MTFS30_ERROR_EN
			MTFS30_ERROR("���ջ�����ģʽ(%d)����", mode);
#endif	
			break;
	}


}


/*****************************************************************************
 * ��  ��:    ut4b0_receiver_msg_parser                                                           
 * ��  ��:    ����ut4b0���ջ��������������                                                                 
 * ��  ��:     *pdata: ��������                         
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/  
void ut4b0_receiver_msg_parser(u8_t *pdata)
{
	u8_t *p = pdata;    /* ָ��ÿ���ַ�           */
	u8_t *pmsg = NULL;  /* ָ��ÿ���������ָ��   */
    u8_t *pcode = NULL; /* ִ������ָ���е�У���� */
    u8_t msg_chck_type; /* У������ */

//#if MTFS30_DEBUG_EN_01
//            MTFS30_DEBUG("pdata: %s\n", pdata);          
//#endif    
            
	while(*p != '\0')
	{
		/* $��ʾһ���������ָ�ʼ */
		if (*p == '$' || *p == '#')
		{
              
            /* ����������ָ���Ƿ�����
             * ��Ҫ����Ƿ����"*","\r","\n"
             */
            if ((strchr((char *)p, '*' ) == NULL) ||
                (strchr((char *)p, '\r') == NULL) ||
                (strchr((char *)p, '\n') == NULL))
            {
                /* ָ����Ч���˳� */
#if MTFS30_ERROR_EN
			MTFS30_ERROR("ָ����Ч(%s)\n", p);
#endif	                
                break;
            }
            
            /* ��ȡ�����������ݲ��� */           			
            p++; /* ��$���һ���ֽڿ�ʼ��ȡ */ 
            pmsg = (u8_t*)strtok((char *)p, "*");
//#if MTFS30_DEBUG_EN_01_1
//            MTFS30_DEBUG("pmsg: %s\n", pmsg);          
//#endif               
            
            /* ��ȡ��У���� */
            pcode =(u8_t*)strtok(NULL, "\r");
//#if MTFS30_DEBUG_EN_01
//            MTFS30_DEBUG("pcode: %s\n", pcode);               
//#endif      
            /* ����У������ */
            if ((strncmp("GPSUTC", (char *)pmsg, 6) == 0) || 
                (strncmp("BDSUTC", (char *)pmsg, 6) == 0) ||
                (strncmp("GALUTC", (char *)pmsg, 6) == 0) ||
                (strncmp("BBPPSMSGAA", (char *)pmsg, 10) == 0))
            {
                msg_chck_type = CHECK_TYPE_CRC32; /* CRC32У�� */
//#if MTFS30_DEBUG_EN_01
//            MTFS30_DEBUG("msg_chck_type: CHECK_TYPE_CRC32\n");   
//            
//#endif                  
            } 
            else
            {
                msg_chck_type = CHECK_TYPE_XOR; /* ���У�� */
//#if MTFS30_DEBUG_EN_01
//            MTFS30_DEBUG("msg_chck_type: CHECK_TYPE_XOR\n");   
//            
//#endif                  
            }
                    
            
            /* �Ƚ�У�����Ƿ�һ�� */
            if (CHECKCODE_EQ == ut4b0_checkcode_compare(pmsg, pcode, msg_chck_type))
            {
//#if MTFS30_DEBUG_EN_01
//            MTFS30_DEBUG("CHECKCODE_EQ\n");          
//#endif      
                
                /* ���⴦�� */
                if ((strncmp("GPGSV", (char *)pmsg, 5) != 0) &&
                    (strncmp("BDGSV", (char *)pmsg, 5) != 0) &&
                    (strncmp("GLGSV", (char *)pmsg, 5) != 0) &&
                    (strncmp("GAGSV", (char *)pmsg, 5) != 0))
                {
                    
                    gsate_status.status_msg[gsate_status.msg_len - 1] = ';';
                    gsate_status.status_msg[gsate_status.msg_len] = '\0';
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("״̬��Ϣ�� %s\t״̬��Ϣ���� = %d\n", gsate_status.status_msg, gsate_status.msg_len);     
#endif                       
                    gsate_status.msg_len = 0;
                
                }
                /* ���Ҳ�ִ�и�������ָ��Ľ������� */
			    ut4b0_check_cmdhandler(pmsg);         
            }
            
            break;           
		}

		p++; 
	}

}


/*****************************************************************************
 * ��  ��:    ut4b0_checkcode_compare                                                           
 * ��  ��:    ��������ָ�����ݲ��ֵ�У���벢�������ָ���е�У������бȽ�                                                              
 * ��  ��:    *pmsg      : ����ָ�����ݲ���   
 *            *pcode     : ����ָ���е�У����
 *            check_type : У�鷽ʽ
 *                         CHECK_TYPE_XOR  : ���У��
 *                         CHECK_TYPE_CRC32: crc32У��
 * ��  ��:    ��                                                    
 * ����ֵ:    CHECKCODE_EQ: У������ͬ; CHECKCODE_NOEQ: У���벻ͬ                                                   
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/
static u8_t ut4b0_checkcode_compare(u8_t *pmsg, u8_t *pcode, u8_t check_type)
{
    
    u8_t ret = CHECKCODE_NOEQ;
   
    /* �ж�У�鷽ʽ */
    switch(check_type)
    {
    case CHECK_TYPE_XOR:    /* ���У�� */
        ret = util_xor_check(pmsg, strlen((char *)pmsg), pcode);
        break;
        
    case CHECK_TYPE_CRC32: /* crc32У�� */
        ret = util_check_crc32(pmsg, strlen((char *)pmsg), pcode);
        break;
    
    default:
#if MTFS30_ERROR_EN
			MTFS30_ERROR("У�鷽ʽ(%d)����\n", check_type);
#endif	        
        break;
    }


    return ret;
}

/*****************************************************************************
 * ��  ��:    ut4b0_check_cmdhandler                                                           
 * ��  ��:    ����UT4B0���ջ��������ָ��Ľ�������������                                                              
 * ��  ��:     *pdata: ��������                         
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/  
static void ut4b0_check_cmdhandler(u8_t *pdata)
{
	u8_t i = 0;
	u8_t cnt = 0;        /* ��������������� */


	/* ������������ */
	cnt = sizeof(ut4b0_cmdhandle) / sizeof(ut4b0_cmdhandle[0]);

	/* ����ָ������ */
	for (i = 0; i < cnt; i++)
	{
		if(strncmp((char *)ut4b0_cmdhandle[i].cmd, (char*)pdata, strlen((char *)ut4b0_cmdhandle[i].cmd)) == 0)
		{
			/* ���ý������� */
			ut4b0_cmdhandle[i].cmd_fun(pdata);
			return;
		}
	} 

#if MTFS30_ERROR_EN
	u8_t *pcmd = pdata; 

	/* ȡ�����ָ�� */
	pcmd = strtok(pdata, ",");
	MTFS30_ERROR("���ָ��(%s)����", pcmd);
#endif	

}

/*****************************************************************************
 * ��  ��:    ut4b0_gga_msg_parse                                                           
 * ��  ��:    ����UT4B0���ջ����GGA��Ϣ                                                                
 * ��  ��:    *pdata: XXXGGA��Ϣ                          
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/  
static void ut4b0_gga_msg_parse(u8_t *pdata)
{
    u8_t i = 0; 
    u8_t comma_cnt = 0; /* ���Ÿ���             */
    u8_t len = 0;       /* �������Ϣ����       */
    u8_t param_len = 0;  /* ����ÿ�������ĳ���  */
    u8_t pre_comma_index; /* ��һ�ζ��ŵ�λ��   */
    
    
    /* ��ʼ�� */
    glocation.location[0] = '\0';
      
    /* ����ÿ������ */
    while(pdata[i])
    {
        if (pdata[i] == ',')
        {
            comma_cnt++;
            
            
            switch(comma_cnt)
            {
            case 2:    /* UTCʱ��        */
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
            MTFS30_DEBUG("��λ��Ϣ�� %s\t��λ��Ϣ[0] = %d\n", glocation.location, glocation.location[0]);     
#endif                  
                return;      
                break;
            }    
            
            pre_comma_index = i;
            param_len = 0;
        }
        else 
        {
            ///* ��ʱ����ÿ������ */
            //buf[len++] = pdata[i];
            param_len++;
        }
        
        i++;
    }    

}

/*****************************************************************************
 * ��  ��:    ut4b0_gsv_msg_parse                                                           
 * ��  ��:    ����UT4B0���ջ����GSV��Ϣ                                                                 
 * ��  ��:    *pdata: GSV��Ϣ;                       
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/  
static void ut4b0_gsv_msg_parse(u8_t *pdata)
{



    
    u8_t i = 0; 
    u8_t comma_cnt = 0; /* ���Ÿ���             */
    u8_t param_len = 0;  /* ����ÿ�������ĳ���  */
    u8_t pre_comma_index; /* ��һ�ζ��ŵ�λ��   */
    u8_t cmd_buf[6] = {0};
    u8_t  gsv_msg = 0;           /* ��Ϣ��               */    

    
    /* ��ʼ�� */   
    if (gsate_status.msg_len == 0)
    {
        gsate_status.status_msg[gsate_status.msg_len - 1] = '\0';        
    }



    
    pdata[strlen((char *)pdata)] = ',';
    my_strtok(',', (char *)pdata, (char *)cmd_buf, sizeof(cmd_buf));
 
    
    /* ����ÿ������ */
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
 * ��  ��:    ut4b0_gpsbds_utc_msg_parse                                                           
 * ��  ��:    ����UT4B0���ջ������UTC��Ϣ����������Ԥ��(GPS/BDS����)                                                                 
 * ��  ��:    *pdata: UTC��Ϣ;                       
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/
static void ut4b0_gpsbds_utc_msg_parse(u8_t *pdata)
{


    u8_t i = 0; 
    u8_t comma_cnt = 0; /* ���Ÿ���             */
    u8_t param_len = 0;  /* ����ÿ�������ĳ���  */
    u8_t param_buf[20]= {0};
	u32_t wn = 0;         /* �µ�������Ч���ܼ���         */
	u8_t  dn = 0;         /* �µ�������Ч�������ռ���     */
	u8_t now_leap = 0;    /* ��ǰ����                     */
	u8_t next_leap = 0;   /* �µ�����                     */

    
  
    /* �Թ�Logͷ */
	while(pdata[i++] != ';');
      
    /* ����ÿ������ */
    while(pdata[i])
    {
        if (pdata[i] == ',')
        {
            comma_cnt++;
            
            param_buf[param_len]='\0';
            switch(comma_cnt)
            {
            case 5:    /* UTCʱ��        */
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
            MTFS30_DEBUG("�µ�������Ч���ܼ����� %d\n�µ�������Ч�������ռ����� %d\n��ǰ���룺 %d\n�µ����룺 %d\n", 
                         wn, dn, now_leap,next_leap);

#endif       
	if (strncmp((char *)pdata, "GPSUTC", 6) == 0)   /* GPS���� */
	{
        /* GPS���⴦�� */
        dn = dn - 1; /* �����ռ��� */
        
		/* ��������Ԥ�� */
		get_leap_forecast(SAT_TYPE_GPS, wn, dn, now_leap, next_leap);
	}
	else if (strncmp((char *)pdata, "BDSUTC", 6) == 0)    /* BDS���� */
	{
        
        /* BDS���⴦�� */
        now_leap = now_leap + 14;  /* ��ǰ���룬��GPSʱ��Ϊ׼  */
        next_leap = next_leap + 14;/*  �µ����룬��GPSʱ��Ϊ׼ */
        
		/* ��������Ԥ�� */
		get_leap_forecast(SAT_TYPE_BDS, wn, dn+1, now_leap, next_leap);		
	}
    
}

/*****************************************************************************
 * ��  ��:    ut4b0_gal_utc_msg_parse                                                           
 * ��  ��:    ����UT4B0���ջ������UTC��Ϣ����������Ԥ��(GAL����)                                                                 
 * ��  ��:    *pdata: UTC��Ϣ;                       
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/
static void ut4b0_gal_utc_msg_parse(u8_t *pdata)
{


    u8_t i = 0; 
    u8_t comma_cnt = 0; /* ���Ÿ���             */
    u8_t param_len = 0;  /* ����ÿ�������ĳ���  */
    u8_t param_buf[20]= {0};
	u32_t wn = 0;         /* �µ�������Ч���ܼ���         */
	u8_t  dn = 0;         /* �µ�������Ч�������ռ���     */
	u8_t now_leap = 0;    /* ��ǰ����                     */
	u8_t next_leap = 0;   /* �µ�����                     */

    
    
    /* �Թ�Logͷ */
	while(pdata[i++] != ';');
      
    /* ����ÿ������ */
    while(pdata[i])
    {
        if (pdata[i] == ',')
        {
            comma_cnt++;
            
            param_buf[param_len]='\0';
            switch(comma_cnt)
            {
            case 3:    /* UTCʱ��        */
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
            MTFS30_DEBUG("GAL �µ�������Ч���ܼ����� %d\n�µ�������Ч�������ռ����� %d\n��ǰ���룺 %d\n�µ����룺 %d\n", 
                         wn, dn, now_leap,next_leap);

#endif      
	/* ��������Ԥ�� */
	get_leap_forecast(SAT_TYPE_GAL, wn, dn, now_leap, next_leap);

    
}

/*****************************************************************************
 * ��  ��:    get_leap_forecast                                                           
 * ��  ��:    ��������Ԥ��                                                                 
 * ��  ��:    sat_type  : ��������
 *            wn        ���µ�������Ч���ܼ��� 
 *            dn        ���µ�������Ч�������ռ���
 *            now_leap  ����ǰ����
 *            next_leap ���´�����
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/
static void get_leap_forecast(u8_t sat_type, u32_t wn, u8_t dn, u8_t now_leap, u8_t next_leap)
{

	u32_t year;   /* �µ����뷢���� */ 
	u8_t  mon;    /* �µ����뷢���� */
	u8_t  day;    /* �µ����뷢���� */
	u32_t days;   /* �µ����뷢���վ�����ʼ��Ԫ�������� */
	u8_t  mon_days[12] ={31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; /* ÿ������ */

    
    gleap_forecast_msg.leap_msg[0] = '\0';
    
	switch(sat_type)
	{
		case SAT_TYPE_GPS:    /* GPS���� */
			year = 1980; /* GPSʱ����ʼ�� */
			mon = 1;     /* GPSʱ����ʼ�� */
			day = 6;     /* GPSʱ����ʼ�� */
			break;

		case SAT_TYPE_BDS:    /* BDS���� */
			year = 2006; /* BDSʱ����ʼ�� */      
			mon = 1;     /* BDSʱ����ʼ�� */    
			day = 1;     /* BDSʱ����ʼ�� */
			break;
            
        case SAT_TYPE_GAL:     /* GAL���� */
			year = 1999; /* GALʱ����ʼ�� */      
			mon = 8;     /* GALʱ����ʼ�� */    
			day = 22;    /* GALʱ����ʼ�� */ 
            break;
            
        default:            
            break;
	}


	/*-------------------------------*/
	/* �����µ����뷢����������      */
	/*-------------------------------*/	 

	/* �µ����뷢���վ�����ʼ��Ԫ�������� */
	days = wn * 7 + dn + day;

	while(days > mon_days[mon-1])
	{

		/* �����������µ����������������һ���� */
		days -= mon_days[mon-1];
		mon++;

		/* ����12�½�����һ�� */
		if (mon == 13)
		{
			year++;

			/* �ж����껹��ƽ�� */
			if ((year % 400 == 0) || (year % 100 != 0 && year % 4 == 0))
			{
				mon_days[1] = 29; /* �����2��Ϊ29�� */
			}
			else 
			{
				mon_days[1] = 28; /* ƽ���2��Ϊ28�� */
			}

			mon = 1; /* �µ�һ�꣬�·����´�1�¿�ʼ */
		}


	}

	/* �µ����뷢���� */
	day = days; 


	/*-------------------------------*/
	/* ��������Ԥ��                  */
	/*-------------------------------*/	

	/* ��ʼ�� */
	memset(&gleap_forecast, 0x00, sizeof(gleap_forecast));	 
	/* �µ����뷢���� */
	sprintf((char *)gleap_forecast.year, "%d", year);
	/* �µ����뷢���� */
	sprintf((char *)gleap_forecast.mon, "%d", mon);
	/* �µ����뷢���� */
	sprintf((char *)gleap_forecast.day, "%d", day);
	/* �µ�����ֵ */
	gleap_forecast.next_leap = next_leap;
	/* ��/������ */
	gleap_forecast.sign = next_leap - now_leap;

	/* ���뷢��ʱ���� */
	if (gleap_forecast.sign >= 0)/* ������ */
	{
        sprintf((char *)gleap_forecast_msg.leap_msg, "%d,%d,%d,%s,%d,%d;", year, mon, day, "075960", next_leap, next_leap - now_leap);
	}
	else
	{
        sprintf((char *)gleap_forecast_msg.leap_msg, "%d,%d,%d,%s,%d,%d;", year, mon, day, "075958", next_leap, next_leap - now_leap);
	}
    
#if MTFS30_DEBUG_EN
    MTFS30_DEBUG("����Ԥ��:%s\n", gleap_forecast_msg.leap_msg);
                        
#endif         

}

/*****************************************************************************
 * ��  ��:    ut4b0_1pps_msg_parse                                                           
 * ��  ��:    ����UT4B0���ջ������1PPS��Ϣ                                                                
 * ��  ��:    *pdata: UTC��Ϣ;                       
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/
static void ut4b0_1pps_msg_parse(u8_t *pdata)
{
    u8_t i = 0; 
    u8_t comma_cnt = 0; /* ���Ÿ���             */
    u8_t param_len = 0;  /* ����ÿ�������ĳ���  */
    u8_t param_buf[20]= {0};
    u8_t pps_id;
    u8_t pps_status;
    

    
    /* �Թ�Logͷ */
	while(pdata[i++] != ';');
      
    /* ����ÿ������ */
    while(pdata[i])
    {
        if (pdata[i] == ',')
        {
            comma_cnt++;
            
            param_buf[param_len]='\0';
            switch(comma_cnt)
            {
            case 1:    /* UTCʱ��        */
                pps_id = (u8_t)str_to_int32u(param_buf);
                break;
            case 2:
                if (pps_id != 0)
                {
                    return ;
                }
                pps_status = str_to_int32u(param_buf);
#if MTFS30_DEBUG_EN
    MTFS30_DEBUG("1pps ID[%d]״̬Ϊ:%d;\n", pps_id, pps_status);                       
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
 * ��  ��:    ut4b0_receiver_pps_set                                                          
 * ��  ��:    pps����                                                                 
 * ��  ��:    *pps_info: pps������Ϣ                         
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/
void ut4b0_receiver_pps_set(pps_info_t *pps_info)
{
	u8_t buf[128] = {0};


	/* ����pps�������У������ʱ��00���� */
	sprintf((char *)buf, "CONFIG PPS %s %s %s %d %d %d %d",
			pps_info->en_switch, pps_info->timeref, pps_info->polatity,
			pps_info->width, pps_info->period, pps_info->rfdelay, pps_info->userdelay);


	/* ʹ�ô���1�������� */
	usart_send(buf, USART_COM_1);
#if MTFS30_DEBUG_EN
    MTFS30_DEBUG("1pps����:%s\n", buf);                       
#endif        


}

/*****************************************************************************
 * ��  ��:    ut4b0_receiver_reset                                                           
 * ��  ��:    ��ָ����ʽ����UT4B0���ջ�                                                              
 * ��  ��:    reset_mode: 
REV_RESETMODE_ALL        : �����������ã�����������λ����Ϣ��������
REV_RESETMODE_CONFIGSAVE : �����û�����������          
REV_RESETMODE_EPHEM      : ����������������������   
REV_RESETMODE_POSITION   : ��������λ����Ϣ������
REV_RESETMODE_ALMANAC    : ��������������Ϣ������ 
REV_RESETMODE_IONUTC     : ��������� UTC ����������
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/
void ut4b0_receiver_reset(u8_t reset_mode)
{
	u8_t buf[15] = {0};

	switch(reset_mode)
	{
		case REV_RESETMODE_ALL:           /* �����������ã�����������λ����Ϣ�������� */
			strcpy((char *)buf, "FRESET");
			break;

		case REV_RESETMODE_CONFIGSAVE:    /* �����û�����������          */
			strcpy((char *)buf, "RESET");
			break;

		case REV_RESETMODE_EPHEM:         /* ����������������������    */
			strcpy((char *)buf, "RESET EPHEM");
			break;

		case REV_RESETMODE_POSITION:      /* ��������λ����Ϣ������    */
			strcpy((char *)buf, "RESET POSITION");
			break;

		case REV_RESETMODE_ALMANAC:       /* ��������������Ϣ������    */
			strcpy((char *)buf, "RESET ALMANAC");
			break;

		case REV_RESETMODE_IONUTC:        /* ��������� UTC ���������� */
			strcpy((char *)buf, "RESET IONUTC");
			break;

		default:
			break;
	}

	/* ʹ�ô���1�������� */
#if TODO 
	usart_send(buf, USART_COM_1);
#endif	

}


