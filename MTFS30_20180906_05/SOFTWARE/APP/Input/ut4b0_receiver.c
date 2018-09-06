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
	u8_t buf[20] = {0};


	/* ��ֹ������Ϣ��� */
	memset(buf, 0x00, sizeof(buf));
	strcpy((char *)buf, "unlog");
	usart_send(buf, 1); /* ʹ�ô���1�������� */			     


	/* �жϹ���ģʽ */
	switch(mode)
	{
		case REV_ACMODE_GPS:    /* ��GPS��λ */
			/* GPSGGA */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "GPSGGA 1");
			usart_send(buf, 1); /* ʹ�ô���1�������� */		

			/* GPSUTC */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "GPSUTC 1");
			usart_send(buf, 1); /* ʹ�ô���1�������� */				
			break;

		case REV_ACMODE_BDS:    /* ��BDS��λ */
			/* BDSGGA */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "BDSGGA 1");
			usart_send(buf, 1); /* ʹ�ô���1�������� */

			/* BDSUTC */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "BDSUTC 1");
			usart_send(buf, 1); /* ʹ�ô���1�������� */	
			break;

		case REV_ACMODE_GLO:    /* ��GLO��λ */
			/* GLOGGA */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "GLOGGA 1");
			usart_send(buf, 1); /* ʹ�ô���1�������� */			    
			break;

		case REV_ACMODE_GAL:    /* ��GAL��λ */
			/* GALGGA */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "GALGGA 1");
			usart_send(buf, 1); /* ʹ�ô���1�������� */	

			/* GALUTC */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "GALUTC 1");
			usart_send(buf, 1); /* ʹ�ô���1�������� */				
			break;

		case REV_ACMODE_ALL:    /* ��ϵͳ���϶�λ */
			/* GNGGA */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "GNGGA 1");
			usart_send(buf, 1); /* ʹ�ô���1�������� */
 
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("[%s:%d]:    %s\n", __FUNCTION__, __LINE__, buf);      
#endif              

//			/* GPSUTC(��ϵͳ�������ʱ��GPS UTCʱ��Ϊ׼) */ 
//			memset(buf, 0x00, sizeof(buf));
//			strcpy((char *)buf, "GPSUTC 1");
//			usart_send(buf, 1); /* ʹ�ô���1�������� */	
            /* GPSUTC(��ϵͳ�������ʱ��GPS UTCʱ��Ϊ׼) */ 
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "BDSUTC 1");
			usart_send(buf, 1); /* ʹ�ô���1�������� */	
            
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("%s\n", buf);    
#endif                       
			break;			

		default:
#if MTFS30_ERROR_EN
			MTFS30_ERROR("���ջ�����ģʽ(%d)����", mode);
#endif			
			break;
	}
   

	/* GPGSV */
	memset(buf, 0x00, sizeof(buf));
	strcpy((char *)buf, "GPGSV 1");
	usart_send(buf, 1); /* ʹ�ô���1�������� */	
#if MTFS30_DEBUG_EN
    MTFS30_DEBUG("%s\n", buf);      
#endif  
     /* 1PPS */
	memset(buf, 0x00, sizeof(buf));
	strcpy((char *)buf, "BBPPSMSGA ONCHANGED");
	usart_send(buf, 1); /* ʹ�ô���1�������� */	
    
#if MTFS30_DEBUG_EN
    MTFS30_DEBUG("%s\n", buf);   
#endif

#ifdef TODO	
	/* ANTENNA */
	memset(buf, 0x00, sizeof(buf));
	strcpy((char *)buf, "ANTENNA 1");
	usart_send(buf, 1); /* ʹ�ô���1�������� */	
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
	u8_t buf[20] = {0};


	/* �жϽ��ջ�����ģʽ */
	switch(mode)
	{
		case REV_ACMODE_GPS:    /* ��GPS��λ */	

			/* ����BDS */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "MASK BDS");         
			usart_send(buf, 1); /* ʹ�ô���1�������� */

			/* ����GLO */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "MASK GLO");         
			usart_send(buf, 1); /* ʹ�ô���1�������� */		

			/* ����GAL */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "MASK GAL");         
			usart_send(buf, 1); /* ʹ�ô���1�������� */	

			/* ����GPS */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "UNMASK GPS");         
			usart_send(buf, 1); /* ʹ�ô���1�������� */				
			break;

		case REV_ACMODE_BDS:    /* ��BDS��λ */
			/* ����GPS */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "MASK GPS");         
			usart_send(buf, 1); /* ʹ�ô���1�������� */

			/* ����GLO */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "MASK GLO");         
			usart_send(buf, 1); /* ʹ�ô���1�������� */		

			/* ����GAL */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "MASK GAL");         
			usart_send(buf, 1); /* ʹ�ô���1�������� */	

			/* ����GPS */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "UNMASK BDS");         
			usart_send(buf, 1); /* ʹ�ô���1�������� */	
			break;

		case REV_ACMODE_GLO:    /* ��GLO��λ */
			/* ����GPS */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "MASK GPS");         
			usart_send(buf, 1); /* ʹ�ô���1�������� */

			/* ����BDS */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "MASK BDS");         
			usart_send(buf, 1); /* ʹ�ô���1�������� */		

			/* ����GAL */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "MASK GAL");         
			usart_send(buf, 1); /* ʹ�ô���1�������� */	

			/* ����GLO */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "UNMASK GLO");         
			usart_send(buf, 1); /* ʹ�ô���1�������� */	
			break;   

		case REV_ACMODE_GAL:    /* ��GAL��λ */
			/* ����GPS */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "MASK GPS");         
			usart_send(buf, 1); /* ʹ�ô���1�������� */

			/* ����BDS */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "MASK BDS");         
			usart_send(buf, 1); /* ʹ�ô���1�������� */		

			/* ����GLO */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "MASK GLO");         
			usart_send(buf, 1); /* ʹ�ô���1�������� */	

			/* ����GAL */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "UNMASK GAL");         
			usart_send(buf, 1); /* ʹ�ô���1�������� */	
			break;

		case REV_ACMODE_ALL:   /* ��ϵͳ���϶�λ  */
			/* ����GPS */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "UNMASK GPS");    
             
			usart_send(buf, 1); /* ʹ�ô���1�������� */
            
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("%s\n", buf);           
#endif               

			/* ����BDS */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "UNMASK BDS");         
			usart_send(buf, 1); /* ʹ�ô���1�������� */
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("%s\n", buf);          
#endif                

			/* ����GLO */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "UNMASK GLO");         
			usart_send(buf, 1); /* ʹ�ô���1�������� */
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("%s\n", buf);          
#endif                

			/* ����GAL */
			memset(buf, 0x00, sizeof(buf));
			strcpy((char *)buf, "UNMASK GAL");         
			usart_send(buf, 1); /* ʹ�ô���1�������� */
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("%s\n", buf);          
#endif                
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
    u8_t msg_buf[256];
    u8_t msg_len = 0;
    u8_t msg_chck_type; /* У������ */

    
	while(*p != '\0')
	{
		/* $��ʾһ���������ָ�ʼ */
		if (*p == '$' || *p == '#')
		{
              
			/* ��ȡ����������ָ�� */
            p++; /* ��$���һ���ֽڿ�ʼ��ȡ */ 
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("p: %s\n", p);          
#endif                 
            memset(msg_buf, 0x00, sizeof(msg_buf));
            msg_len = my_strtok('$', (const char *)p, (char *)msg_buf, 256);
            
            /* ����Ƿ����"#" */
            if (strchr((char *)msg_buf, '#' ) != NULL)
            {
                memset(msg_buf, 0x00, sizeof(msg_buf));
                msg_len = my_strtok('#', (const char *)p, (char *)msg_buf, 256);
            }
            
 
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("msg_buf: %s\n", msg_buf);          
#endif            
            /* ����������ָ���Ƿ�����
             * ��Ҫ����Ƿ����"*","\r","\n"
             */
            if ((strchr((char *)msg_buf, '*' ) == NULL) ||
                (strchr((char *)msg_buf, '\r') == NULL) ||
                (strchr((char *)msg_buf, '\n') == NULL))
            {
                /* ��������ָ�� */
                p = p + msg_len - 1;
                continue;
            }
            
            /* ��ȡ�����������ݲ��� */
            pmsg = (u8_t*)strtok((char *)msg_buf, "*");
#if MTFS30_DEBUG_EN_1
            MTFS30_DEBUG("pmsg: %s\n", pmsg);          
#endif               
            
            /* ��ȡ��У���� */
            pcode =(u8_t*)strtok(NULL, "\r");
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("pcode: %s\n", pcode);   
            
#endif      
            /* ����У������ */
            if ((strncmp("GPSUTC", (char *)pmsg, 6) == 0) || 
                (strncmp("BDSUTC", (char *)pmsg, 6) == 0) ||
                (strncmp("GALUTC", (char *)pmsg, 6) == 0) ||
                (strncmp("BBPPSMSGAA", (char *)pmsg, 10) == 0))
            {
                msg_chck_type = CHECK_TYPE_CRC32; /* CRC32У�� */
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("msg_chck_type: CHECK_TYPE_CRC32\n");   
            
#endif                  
            } 
            else
            {
                msg_chck_type = CHECK_TYPE_XOR; /* ���У�� */
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("msg_chck_type: CHECK_TYPE_XOR\n");   
            
#endif                  
            }
                    
            
            /* �Ƚ�У�����Ƿ�һ�� */
            if (CHECKCODE_EQ == ut4b0_checkcode_compare(pmsg, pcode, msg_chck_type))
            {
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("CHECKCODE_EQ\n");          
#endif                 
                /* ���Ҳ�ִ�и�������ָ��Ľ������� */
			    ut4b0_check_cmdhandler(pmsg);            
            
            }

			/* ��������ָ�� */
            p = p + msg_len - 1;

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
	u8_t *p = NULL;  


	/* GGAͷ(��������) */
	p = (u8_t *)strtok((char *)pdata, ",");

	/* UTCʱ�� */
	p = (u8_t *)strtok(NULL, ",");
	memset(gtime.hms, 0x00, sizeof(gtime.hms));
	strcpy((char *)gtime.hms, (const char *)p);
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("UTCʱ�䣺 %s\n", gtime.hms);     
#endif 

	/* γ�� */
	p = (u8_t *)strtok(NULL, ",");		
	memset(gsatellite_info.location.lat, 0x00, sizeof(gsatellite_info.location.lat));
	strcpy((char *)gsatellite_info.location.lat, (const char *)p);
#if MTFS30_DEBUG_EN
    MTFS30_DEBUG("[%s:%d] γ��: %s\n", __FUNCTION__, __LINE__, gsatellite_info.location.lat);   
#endif 
    
	/* γ�ȷ��� */
	p = (u8_t *)strtok(NULL, ",");
	gsatellite_info.location.latdir = *p;
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("[%s:%d] γ�ȷ���: %c\n", __FUNCTION__, __LINE__, gsatellite_info.location.latdir);
#endif 
            
	/* ���� */
	p = (u8_t *)strtok(NULL, ",");
	memset(gsatellite_info.location.lon, 0x00, sizeof(gsatellite_info.location.lon));
	strcpy((char *)gsatellite_info.location.lon, (const char *)p);   
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("���ȣ� %s\n", gsatellite_info.location.lon);          
#endif 
            
	/* ���ȷ��� */
	p = (u8_t *)strtok(NULL, ",");
	gsatellite_info.location.londir = *p;
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("���ȷ��� %c\n", gsatellite_info.location.londir);          
#endif
    
	/* GPS����ָʾ��(��������) */
	p = (u8_t *)strtok(NULL, ",");

	/* ʹ���е������� */
	p = (u8_t *)strtok(NULL, ",");
	gsatellite_info.use_sats = *p;
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("ʹ���е��������� %c\n", gsatellite_info.use_sats);          
#endif

	/* ˮƽ��������(��������) */
	p = (u8_t *)strtok(NULL, ",");

	/* ���� */
	p = (u8_t *)strtok(NULL, ",");
	memset(gsatellite_info.location.alt, 0x00, sizeof(gsatellite_info.location.alt));
	strcpy((char *)gsatellite_info.location.alt, (const char *)p);	
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("���Σ� %s\n", gsatellite_info.location.alt);          
#endif	

	/* ����֮������������������� */	 

 
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
	u8_t  i;
	u8_t  *p = NULL;
	u8_t  gsv_msg_total = 0; /* ��Ϣ����            */
	u8_t  gsv_msg = 0;       /* ��Ϣ��              */
	u8_t  gsv_amount_stas;   /* �ɼ�����������      */
	static u8_t gps_cnt = 0; /* �ѽ�����GPS���ǿ��� */
	static u8_t bds_cnt = 0; /* �ѽ�����BDS���ǿ��� */
	static u8_t glo_cnt = 0; /* �ѽ�����GLO���ǿ��� */
	static u8_t gal_cnt = 0; /* �ѽ�����GAL���ǿ��� */
    u8_t  sat_cnt = 0;       /* ÿ������к��е����ǿ��� */
	satellite_sta_t satellite_sta[4]; /* ���ڱ���ÿ��GSV��Ϣ�а�����4������״̬���� */	
#if MTFS30_DEBUG_EN
    u8_t k = 0;
#endif 


	/* GSVͷ(��������) */
	p = (u8_t *)strtok((char *)pdata, ",");

	/* ��Ϣ���� */
	p = (u8_t *)strtok(NULL, ",");
	gsv_msg_total = *(u8_t*)p - '0';
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("��Ϣ������ %d\n", gsv_msg_total);
#endif 
            


	/* ��Ϣ�� */
	p = (u8_t *)strtok(NULL, ",");
	gsv_msg = *(u8_t*)p - '0';
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("��Ϣ���� %d\n", gsv_msg);

#endif     

	/* �ɼ����������� */
	p = (u8_t *)strtok(NULL, ",");
    gsv_amount_stas = (u8_t)str_to_int32u(p);
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("�ɼ������������� %d\n", gsv_amount_stas);

#endif       


	/* ����һ��GSV��Ϣ�а���������״̬��Ϣ */
	for(i = 0; i < 4; i++)
	{
		/* ����PRN��� */
		p = (u8_t *)strtok(NULL, ",");
        if (p == NULL) /* ������� */
        {
            break;
        }
       
       satellite_sta[i].prn = (u8_t)str_to_int32u(p);
        
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("���Ǳ��[%d]�� %d\n", i, satellite_sta[i].prn);

#endif         

		/* ���� */
		p = (u8_t *)strtok(NULL, ",");
        satellite_sta[i].elev = (u8_t)str_to_int32u(p);
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("����[%d]�� %d\n", i, satellite_sta[i].elev); 

#endif   
		/* �汱��λ��(��������) */
		p = (u8_t *)strtok(NULL, ",");

		/* ����� */
		p = (u8_t *)strtok(NULL, ",");
        satellite_sta[i].snr = (u8_t)str_to_int32u(p);
         
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("�����[%d]�� %d\n", i, satellite_sta[i].snr);

#endif   
        /* ÿ����Ϣ���������ǿ�����1 */    
        sat_cnt++; 
	}


	/* GPS */
	if (strncmp((char *)pdata, "GPGSV", 5) == 0)
	{
		/* �ɼ���GPS�������� */
		gsatellite_info.gps_amount_sats = gsv_amount_stas;

		/* ��������GSV��Ϣ�е�����״̬���� */
		memcpy(&gsatellite_info.gps_status[gps_cnt], satellite_sta, sizeof(satellite_sta_t) * sat_cnt);


		/* �ѽ���������GPS����״̬��Ϣ */			
		if (gsv_msg == gsv_msg_total)
		{
#if MTFS30_DEBUG_EN
  
         
            for(k = 0; k < gsatellite_info.gps_amount_sats; k++)
            {
                MTFS30_DEBUG("GPS����[%d]������PRN��ţ� %d\n", k, gsatellite_info.gps_status[k].prn);

                MTFS30_DEBUG("GPS����[%d]���������ǣ� %d\n", k, gsatellite_info.gps_status[k].elev);

                MTFS30_DEBUG("GPS����[%d]����������ȣ� %d\n", k, gsatellite_info.gps_status[k].snr);
            }
#endif  
			gps_cnt = 0;

		} 
		else
		{
			gps_cnt += sat_cnt; /* �ѽ�����GPS���ǿ������£�ÿ��GSV��Ϣ������4������ */
		}		
	}
	/* BDS */
	else if(strncmp((char *)pdata, "BDGSV", 5) == 0)
	{
		/* �ɼ���BDS�������� */
		gsatellite_info.bds_amount_sats = gsv_amount_stas;

		/* ��������GSV��Ϣ�е�����״̬���� */
		memcpy(&gsatellite_info.bds_status[bds_cnt], satellite_sta, sizeof(satellite_sta_t) * sat_cnt);

		/* �ѽ���������BDS����״̬��Ϣ */
		if (gsv_msg == gsv_msg_total)
		{
#if MTFS30_DEBUG_EN
  
            for(k = 0; k < gsatellite_info.bds_amount_sats; k++)
            {
                MTFS30_DEBUG("BDS����[%d]������PRN��ţ� %d\n", k, gsatellite_info.bds_status[k].prn); 
                MTFS30_DEBUG("BDS����[%d]���������ǣ� %d\n", k, gsatellite_info.bds_status[k].elev);
                MTFS30_DEBUG("BDSS����[%d]����������ȣ� %d\n", k, gsatellite_info.bds_status[k].snr);
            }
#endif              
			bds_cnt = 0;	
		} 
		else
		{
			bds_cnt += sat_cnt; /* �ѽ�����BDS���ǿ������£�ÿ��GSV��Ϣ������4������ */
		}		
	}
	/* GLO */
	else if(strncmp((char *)pdata, "GLGSV", 5) == 0)
	{
		/* �ɼ���GLO�������� */
		gsatellite_info.glo_amount_sats = gsv_amount_stas;

		/* ��������GSV��Ϣ�е�����״̬���� */
		memcpy(&gsatellite_info.glo_status[glo_cnt], satellite_sta, sizeof(satellite_sta_t) * sat_cnt);

		/* �ѽ���������GLO����״̬��Ϣ */
		if (gsv_msg == gsv_msg_total)
		{
#if MTFS30_DEBUG_EN
  
            
            for(k = 0; k < gsatellite_info.glo_amount_sats; k++)
            {
                MTFS30_DEBUG("GLO����[%d]������PRN��ţ� %d\n", k, gsatellite_info.glo_status[k].prn);

                MTFS30_DEBUG("GLO����[%d]���������ǣ� %d\n", k, gsatellite_info.glo_status[k].elev);

                MTFS30_DEBUG("GLO����[%d]����������ȣ� %d\n", k, gsatellite_info.glo_status[k].snr);
            }
#endif              
			glo_cnt = 0;	
		} 
		else
		{
			glo_cnt += sat_cnt; /* �ѽ�����GLO���ǿ������£�ÿ��GSV��Ϣ������4������ */
		}		
	}
	/* GAL */
	else if(strncmp((char *)pdata, "GAGSV", 5) == 0)
	{
		/* �ɼ���GAL�������� */
		gsatellite_info.gal_amount_sats = gsv_amount_stas;
        
		/* ��������GSV��Ϣ�е�����״̬���� */
		memcpy(&gsatellite_info.gal_status[gal_cnt], satellite_sta, sizeof(satellite_sta_t) * sat_cnt);

		/* �ѽ���������GAL����״̬��Ϣ */
		if (gsv_msg == gsv_msg_total)
		{
#if MTFS30_DEBUG_EN
  
            
            for(k = 0; k < gsatellite_info.gal_amount_sats; k++)
            {
                MTFS30_DEBUG("GAL����[%d]������PRN��ţ� %d\n", k, gsatellite_info.gal_status[k].prn);

                MTFS30_DEBUG("GAL����[%d]���������ǣ� %d\n", k, gsatellite_info.gal_status[k].elev);

                MTFS30_DEBUG("GAL����[%d]����������ȣ� %d\n", k, gsatellite_info.gal_status[k].snr);
            }
#endif              
			gal_cnt = 0;	
		} 
		else
		{
			gal_cnt += sat_cnt; /* �ѽ�����GAL���ǿ������£�ÿ��GSV��Ϣ������4������ */
		}		
	}
	else
	{

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
	u8_t i;
	u8_t cnt;
	u8_t *p;
	u32_t wn;         /* �µ�������Ч���ܼ���         */
	u8_t  dn;         /* �µ�������Ч�������ռ���     */
	u8_t now_leap;    /* ��ǰ����                     */
	u8_t next_leap;   /* �µ�����                     */
	u8_t i_wn;        /* �µ�������Ч���ܼ���λ��     */



	/* ��ʼ�� */
	i = 0;
	cnt = 0;	
	i_wn = 0;


	/* �Թ�Logͷ */
	while(pdata[i++] != ';');


	while(pdata[i] != '\0')
	{
		if(pdata[i] == ',')
		{
			cnt++;
            if (cnt == 4)
            {
                i_wn = i + 1; /* �µ�������Ч���ܼ���λ��     */
                break;
            }
		}
        
        i++;
	}

    /* �µ�������Ч���ܼ��� */
    p = (u8_t *)strtok((char *)&pdata[i_wn], ",");
    wn = str_to_int32u(p);
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("�µ�������Ч���ܼ����� %d\n", wn);

#endif
            
    /* �µ�������Ч�������ռ��� */
    p = (u8_t *)strtok(NULL, ",");
    dn = str_to_int32u(p);
    
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("�µ�������Ч�������ռ����� %d\n", dn);

#endif 
            
    /* ��ǰ���� */
    p = (u8_t *)strtok(NULL, ",");
    now_leap = (u8_t)str_to_int32u(p);
    
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("��ǰ���룺 %d\n", now_leap);

#endif
            
    /* �µ����� */	        
    p = (u8_t *)strtok(NULL, ",");
    next_leap = (u8_t)str_to_int32u(p);
    
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("�µ����룺 %d\n", next_leap);

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
 * ��  ��:    ut4b0_gpsbds_utc_msg_parse                                                           
 * ��  ��:    ����UT4B0���ջ������UTC��Ϣ����������Ԥ��(GAL����)                                                                 
 * ��  ��:    *pdata: UTC��Ϣ;                       
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/
static void ut4b0_gal_utc_msg_parse(u8_t *pdata)
{
	u8_t i;
	u8_t cnt;
	u8_t *p;
	u32_t wn;         /* �µ�������Ч���ܼ���         */
	u8_t  dn;         /* �µ�������Ч�������ռ���     */
	u8_t now_leap;    /* ��ǰ����                     */
	u8_t next_leap;   /* �µ�����                     */
	u8_t i_now_leap;  /* ��ǰ����λ��                 */



	/* ��ʼ�� */
	i = 0;
	cnt = 0;	
	i_now_leap = 0;


	/* �Թ�Logͷ */
	while(pdata[i++] != ';');


	while(pdata[i++] != '\0')
	{
		if(pdata[i] == ',')
		{
			cnt++;
            if (cnt == 2)
            {
                i_now_leap = i + 1; /* ��ǰ����λ�� */
                break;
            }
		}
	}

    /* ��ǰ���� */
    p = (u8_t *)strtok((char *)&pdata[i_now_leap], ",");
	now_leap = (u8_t)str_to_int32u(p);
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("��ǰ���룺 %d\n", now_leap);

#endif
    
    
    /* UTC �����Ĳο�ʱ��(��������) */
    p = (u8_t *)strtok(NULL, ",");
    
    /* UTC �ο�����(��������) */
    p = (u8_t *)strtok(NULL, ",");
 
    
    /* �µ�������Ч���ܼ��� */
    p = (u8_t *)strtok(NULL, ",");
    wn = str_to_int32u(p);
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("�µ�������Ч���ܼ����� %d\n", wn);

#endif
            
    /* �µ�������Ч�������ռ��� */
    p = (u8_t *)strtok(NULL, ",");
    dn = str_to_int32u(p);    
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("�µ�������Ч�������ռ����� %d\n", dn);

#endif  
    
    /* �µ����� */
    p = (u8_t *)strtok(NULL, ",");
    next_leap = (u8_t)str_to_int32u(p); 
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("�µ����룺 %d\n", next_leap);

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
		sprintf((char *)gleap_forecast.hms, "%s", "075960"); /* ����ʱ�������뷢��ʱ��Ϊ: 07:59:60 */
	}
	else
	{
		sprintf((char *)gleap_forecast.hms, "%s", "075958"); /* ����ʱ�为���뷢��ʱ��Ϊ: 23:59:58 */
	}
    
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("�µ����뷢���꣺ %s\n", gleap_forecast.year);
            MTFS30_DEBUG("�µ����뷢���£� %s\n", gleap_forecast.mon);
            MTFS30_DEBUG("�µ����뷢���գ� %s\n", gleap_forecast.day);
            MTFS30_DEBUG("�µ�����ֵ�� %d\n", gleap_forecast.next_leap);
            MTFS30_DEBUG("��/�����룺 %d\n", gleap_forecast.sign);
            MTFS30_DEBUG("���뷢��ʱ���룺 %s\n", gleap_forecast.hms);
                        
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

    u8_t pps_id;
    u8_t *p = NULL;
    
    
    
	/* �Թ�Logͷ */
	p = (u8_t *)strtok((char *)pdata, ";");

	/* 1PPS ID */
	p = (u8_t *)strtok(NULL, ",");
	pps_id = *(u8_t*)p - '0';
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("1PPS ID�� %d\n", pps_id);          
#endif 
            
    /* 1PPS״̬��ID 0Ϊ׼ */        
    if (pps_id != 0)
    {
        return ;
    }        
    
    /* 1PPS״̬ */
    p = (u8_t *)strtok(NULL, ",");  
    grecviver_info.pps_status = *(u8_t*)p - '0';
    
#if MTFS30_DEBUG_EN
            MTFS30_DEBUG("1PPS״̬�� %d\n", grecviver_info.pps_status);          
#endif     
    
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
	sprintf((char *)buf, "CONFIG PPS %s %s %s %d %d %d %d*00\r\n",
			pps_info->en_switch, pps_info->timeref, pps_info->polatity,
			pps_info->width, pps_info->period, pps_info->rfdelay, pps_info->userdelay);


	/* ʹ�ô���1�������� */
#if TODO 
	usart_send(buf, USART_COM_1);
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


