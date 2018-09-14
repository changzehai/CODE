/*****************************************************************************/
/* 文件名:    gnss_init.c                                                    */
/* 描  述:    GNSS模块初始化                                                 */
/* 创  建:    2018-05-29 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/

/*****************************************************************************
 * 函  数:    gnss_init                                                           
 * 功  能:    GNSS模块初始化                                                                  
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/  
void gnss_init(void)
{
	char cmd[50];	


	/* 输出GNGGA语句 */
	memset(cmd, 0x00, sizeof(cmd));
	strcpy(cmd, "GNGGA COM1 1");
	USAET_Send_data(1, cmd);

	/* 输出GPGSV语句 */
	memset(cmd, 0x00, sizeof(cmd));
	strcpy(cmd, "GPGSV COM1 1");
	USAET_Send_data(1, cmd);

	/* 输出GNRMC语句 */
	memset(cmd, 0x00, sizeof(cmd));
	strcpy(cmd, "GNRMC COM1 1");
	USAET_Send_data(1, cmd);

	/* 设置卫星自主优化授时 */
#if 1
	memset(cmd, 0x00, sizeof(cmd));
	strcpy(cmd, "MODE TIMING");
	USAET_Send_data(1, cmd);
#else	
	memset(cmd, 0x00, sizeof(cmd));
	strcpy(cmd, "MODE TIMING Surveyin 60 1.5 2.5"); /* 自主优化时间为60秒，水平精度1.5米，高程精度2.5米 */
	USAET_Send_data(1, cmd);
#endif	
	/* PPS脉冲配置 */
	memset(cmd, 0x00, sizeof(cmd));
	strcpy(cmd, "CONFIG PPS ENABLE GPS POSITIVE 500000 1000 0 0");
	USAET_Send_data(1, cmd);	


}












