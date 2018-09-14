/*****************************************************************************/
/* �ļ���:    gnss_handler.c                                                 */
/* ��  ��:    GNSS�������/��ѯ����                                          */
/* ��  ��:    2018-07-20 changzehai(DTT)                                     */
/* ��  ��:    ��                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include "gnss_handler.h"
#include "gnss_receiver.h"
#include "mtfs30.h"


/*-------------------------------*/
/* ��������                      */
/*-------------------------------*/
extern satellite_info_t gsatellite_info; /* �����ۺ���Ϣ */
extern recviver_info_t  grecviver_info;  /* ���ջ���Ϣ   */
extern leap_forecast_t  gleap_forecast;  /* ����Ԥ��     */
extern ttime_t           gtime;           /* ʱ����Ϣ     */ 


/*****************************************************************************
 * ��  ��:    gnss_pps_set_handler                                                           
 * ��  ��:    ����pps                                                                
 * ��  ��:    *pdata: ��������                         
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/  
void gnss_pps_set_handler(u8_t *pparam)
{




}

/*****************************************************************************
 * ��  ��:    gnss_acmode_set_handler                                                           
 * ��  ��:    ���ý��ջ�����ģʽ                                                                
 * ��  ��:    *pparam: ������Ϣ                         
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                   
 ****************************************************************************/  
void gnss_acmode_set_handler(u8_t *pparam)
{

	u8_t ac_mode = 0;  /* ����ģʽ */

#ifdef TODO
	�������ò���
		ac_mode = ?
#endif

		switch(ac_mode)
		{
			case REV_ACMODE_GPS:    /* ��GPS��λ      */
			case REV_ACMODE_BDS:    /* ��BDS��λ      */
			case REV_ACMODE_GLO:    /* ��GLO��λ      */
			case REV_ACMODE_GAL:    /* ��GAL��λ      */
			case REV_ACMODE_ALL:    /* ��ϵͳ���϶�λ */
				/* ���ϴα���Ĺ���ģʽ��ͬ */
				if (ac_mode == grecviver_info.ac_mode) 
				{
					/* �����µĹ���ģʽ */
					gnss_receiver_acmode_set(ac_mode);

					/* �������������������� */
					gnss_receiver_msg_set(ac_mode);

					/* ���汾������ */
					grecviver_info.ac_mode = ac_mode;
				}
				break;

			default:
#if MTFS30_ERROR_EN
				MTFS30_ERROR("���ջ�����ģʽ(%d)����", mode);
#endif	
				break;

		}

}

/*****************************************************************************
 * ��  ��:    gnss_receiver_reset_handler                                                           
 * ��  ��:    �������ջ�                                                                
 * ��  ��:    *pparam: ������ʽ��Ϣ                        
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                   
 ****************************************************************************/ 
void gnss_receiver_reset_handler(u8_t *pparam)
{

	u8_t reset_mode = 0;  /* ������ʽ */

#ifdef TODO
	�������ò���
		reset_mode = ?
#endif	

		/* �ж�������ʽ */
		switch(reset_mode)
		{
			case REV_RESETMODE_ALL:           /* �����������ã�����������λ����Ϣ�������� */
			case REV_RESETMODE_CONFIGSAVE:    /* �����û�����������          */
			case REV_RESETMODE_EPHEM:         /* ����������������������    */
			case REV_RESETMODE_POSITION:      /* ��������λ����Ϣ������    */
			case REV_RESETMODE_ALMANAC:       /* ��������������Ϣ������    */
			case REV_RESETMODE_IONUTC:        /* ��������� UTC ���������� */
				/* �������ջ� */
				gnss_receiver_reset(reset_mode);
				break;

			default:
#if MTFS30_ERROR_EN
				MTFS30_ERROR("������ʽ(%d)����", reset_mode);
#endif			
				break;



		}


}
