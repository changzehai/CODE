/*****************************************************************************/
/* �ļ���:    gnss_init.c                                                    */
/* ��  ��:    GNSSģ���ʼ��                                                 */
/* ��  ��:    2018-05-29 changzehai(DTT)                                     */
/* ��  ��:    ��                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/

/*****************************************************************************
 * ��  ��:    gnss_init                                                           
 * ��  ��:    GNSSģ���ʼ��                                                                  
 * ��  ��:    ��                          
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/  
void gnss_init(void)
{
	char cmd[50];	


	/* ���GNGGA��� */
	memset(cmd, 0x00, sizeof(cmd));
	strcpy(cmd, "GNGGA COM1 1");
	USAET_Send_data(1, cmd);

	/* ���GPGSV��� */
	memset(cmd, 0x00, sizeof(cmd));
	strcpy(cmd, "GPGSV COM1 1");
	USAET_Send_data(1, cmd);

	/* ���GNRMC��� */
	memset(cmd, 0x00, sizeof(cmd));
	strcpy(cmd, "GNRMC COM1 1");
	USAET_Send_data(1, cmd);

	/* �������������Ż���ʱ */
#if 1
	memset(cmd, 0x00, sizeof(cmd));
	strcpy(cmd, "MODE TIMING");
	USAET_Send_data(1, cmd);
#else	
	memset(cmd, 0x00, sizeof(cmd));
	strcpy(cmd, "MODE TIMING Surveyin 60 1.5 2.5"); /* �����Ż�ʱ��Ϊ60�룬ˮƽ����1.5�ף��߳̾���2.5�� */
	USAET_Send_data(1, cmd);
#endif	
	/* PPS�������� */
	memset(cmd, 0x00, sizeof(cmd));
	strcpy(cmd, "CONFIG PPS ENABLE GPS POSITIVE 500000 1000 0 0");
	USAET_Send_data(1, cmd);	


}












