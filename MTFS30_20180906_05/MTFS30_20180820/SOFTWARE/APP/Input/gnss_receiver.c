/*****************************************************************************/
/* �ļ���:    gnss_receiver.c                                                */
/* ��  ��:    ���ǽ��ջ���ش���                                             */
/* ��  ��:    2018-07-20 changzehai(DTT)                                     */
/* ��  ��:    ��                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include "gnss_receiver.h"
#include "dev_usart.h"
#include "ut4b0_receiver.h"

/*-------------------------------*/
/* ��������                      */
/*-------------------------------*/
satellite_info_t gsatellite_info; /* �����ۺ���Ϣ */
recviver_info_t  grecviver_info;  /* ���ջ���Ϣ   */
leap_forecast_t  gleap_forecast;  /* ����Ԥ��     */
ttime_t           gtime;          /* ʱ����Ϣ     */      




/*****************************************************************************
 * ��  ��:    gnss_receiver_init                                                           
 * ��  ��:    ���ǽ��ջ���ʼ��                                                                  
 * ��  ��:    ��                          
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/ 
void gnss_receiver_init(void)
{

	memset(&grecviver_info, 0x00, sizeof(recviver_info_t));

	/* ȡ�ý��ջ����� */
#ifdef TODO	
	grecviver_info.rec_type  = fpga_read(REV_TYPE);
#endif

	switch(grecviver_info.rec_type)
	{
		case REC_TYPE_UT4B0: /* ut4b0���ͽ��ջ� */
			/* ���ջ��ͺ����� */
			strncpy((char *)grecviver_info.rec_name, "UT4B0", 5);
			/* ut4b0���ջ���ʼ�� */
			ut4b0_receiver_init();
			break;

		default:
#if MTFS30_ERROR_EN
			MTFS30_ERROR("���ջ������д�, ���ջ�����Ϊ[%d]", grecviver_info.rec_type);
#endif
			break;
	}


}

/*****************************************************************************
 * ��  ��:    gnss_receiver_msg_parser                                                           
 * ��  ��:    ������������                                                                 
 * ��  ��:     *pdata: ��������                         
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/  
void gnss_receiver_msg_parser(u8_t *pdata)
{

	/* �жϽ��ջ�����,�����͵Ľ��ջ��������ָ����ܲ�һ�� */	 
	switch(grecviver_info.rec_type)
	{
		case REC_TYPE_UT4B0:    /* UT4B0���ջ� */
			/* ����ut4b0���ջ�������������� */
			ut4b0_receiver_msg_parser(pdata);
			break;

		default:
			break;

	}

}


/*****************************************************************************
 * ��  ��:    gnss_receiver_pps_set                                                          
 * ��  ��:    pps����                                                                 
 * ��  ��:    *pps_info: pps������Ϣ                         
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/
void gnss_receiver_pps_set(pps_info_t *pps_info)
{

	/* �жϽ��ջ����� */
	switch(grecviver_info.rec_type)
	{
		case REC_TYPE_UT4B0:    /* UT4B0���ջ� */
			/* UT4B0���ջ�pps���� */
			ut4b0_receiver_pps_set(pps_info);
			break;

		default:
			break;
	}			



}

///*****************************************************************************
// * ��  ��:    gnss_receiver_timing_set                                                          
// * ��  ��:    ��ʱָ������                                                                 
// * ��  ��:    type  : ��ʱ���� 
// TIMING_TYPE_REALTIME��ʵʱ��λ��ʱ;
//TIMING_TYPE_FIX     : ����̶�������ʱ
//TIMING_TYPE_SURVEYIN: �����Ż���ʱ
//param1: ָ�����1
//param2: ָ�����2
//param3��ָ�����3			  
// * ��  ��:    ��                                                    
// * ����ֵ:    ��                                                    
// * ��  ��:    2018-07-18 changzehai(DTT)                            
// * ��  ��:    ��                                       
// ****************************************************************************/
//void gnss_receiver_timing_set(u8_t type, u8_t *param1, u8_t *param2, u8_t *param3)
//{
//	u8_t buf[128] = {0};
//
//	switch(type)
//	{
//		case TIMING_TYPE_REALTIME:    /* ʵʱ��λ��ʱ     */
//			sprintf((char *)buf, "%s*00\r\n", "ODE TIMING");
//			break;
//
//		case TIMING_TYPE_FIX:         /* ����̶�������ʱ */
//			sprintf((char *)buf, "MODE TIMING FIX %s %s %s*00\r\n", param1, param2, param3);
//			break;
//
//		case TIMING_TYPE_SURVEYIN:    /* �����Ż���ʱ     */
//			sprintf((char *)buf, "MODE TIMING FIX %s %s %s*00\r\n", param1, param2, param3);
//			break;
//
//		default:
//			break;
//	}
//
//
//
//	/* ʹ�ô���1�������� */
//#ifdef TODO 
//	usart_send(buf, strlen((char*)buf));
//#endif		
//
//}

/*****************************************************************************
 * ��  ��:    gnss_receiver_acmode_set                                                          
 * ��  ��:    ���ý��ջ�����ģʽ                                                                 
 * ��  ��:    ac_mode: 
 *                    REV_ACMODE_GPS: ��GPS��λ
 *                    REV_ACMODE_BDS: ��BDS��λ
 *                    REV_ACMODE_GLO: ��GLO��λ
 *                    REV_ACMODE_GAL: ��GAL��λ
 *                    REV_ACMODE_ALL: ��ϵͳ���϶�λ 
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/
void gnss_receiver_acmode_set(u8_t ac_mode)
{

	switch(grecviver_info.rec_type)
	{
		case REC_TYPE_UT4B0:    /* UT4B0���ջ� */
			/* ����UT4B0���ջ�����ģʽ */
			ut4b0_receiver_acmode_set(ac_mode);
			break;

		default:
			break;
	}	


}

/*****************************************************************************
 * ��  ��:    gnss_receiver_msg_set                                                           
 * ��  ��:    ���ý��ջ�������������                                                               
 * ��  ��:    ac_mode: 
 *                    REV_ACMODE_GPS: ��GPS��λ
 *                    REV_ACMODE_BDS: ��BDS��λ
 *                    REV_ACMODE_GLO: ��GLO��λ
 *                    REV_ACMODE_GAL: ��GAL��λ
 *                    REV_ACMODE_ALL: ��ϵͳ���϶�λ                        
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/
void gnss_receiver_msg_set(u8_t ac_mode)
{

	switch(grecviver_info.rec_type)
	{
		case REC_TYPE_UT4B0:    /* UT4B0���ջ� */
			/* ����ut4b0���ջ�������������  */
			ut4b0_receiver_msg_set(ac_mode);
			break;

		default:
			break;
	}	

} 

/*****************************************************************************
 * ��  ��:    gnss_receiver_reset                                                           
 * ��  ��:    ��ָ����ʽ�������ջ�                                                               
 * ��  ��:    reset_mode: 
 *                       REV_RESETMODE_ALL        : �����������ã�����������λ����Ϣ��������
 *                       REV_RESETMODE_CONFIGSAVE : �����û�����������          
 *                       REV_RESETMODE_EPHEM      : ����������������������   
 *                       REV_RESETMODE_POSITION   : ��������λ����Ϣ������
 *                       REV_RESETMODE_ALMANAC    : ��������������Ϣ������ 
 *                       REV_RESETMODE_IONUTC     : ��������� UTC ����������
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/
void gnss_receiver_reset(u8_t reset_mode)
{

	switch(grecviver_info.rec_type)
	{
		case REC_TYPE_UT4B0:    /* UT4B0���ջ� */
			/* ����ut4b0���ջ�  */
			ut4b0_receiver_msg_set(reset_mode);
			break;

		default:
			break;
	}	


}




