/*****************************************************************************/
/* �ļ���:    mtfs30.h                                                       */
/* ��  ��:    MTFS30����ͷ�ļ�                                               */
/* ��  ��:    2018-07-19 changzehai(DTT)                                     */
/* ��  ��:    ��                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#ifndef __MTFS30_H_
#define __MTFS30_H_
#include "stdio.h"
#include "string.h"
#include "cc.h"


/*-------------------------------*/
/* �궨��                        */
/*-------------------------------*/

#define    OK          0      /* ���� */
#define    NG          1      /* �쳣 */ 

/* �ڶ��峤��ʱ��Ҫ���һ��λ������ '\0' */

#define    IP_LEN      5     /* IP��ַ����  */
#define    MAC_LEN     7     /* MAC��ַ���� */
#define    MASK_LEN    5     /* ���볤��    */
#define    GATEWAY_LEN 5     /* ���س���    */

/* λ����Ϣ��� */
#define    LAT_LEN     14    /* γ�ȳ��� */
#define    LON_LEN     15    /* ���ȳ��� */
#define    ALT_LEN     9     /* ���γ��� */



/* ʱ����� */
#define    HMS_LEN     10    /* ʱ���볤�� */
#define    DAY_LEN     3     /* �ճ���     */
#define    MON_LEN     3     /* �³���     */
#define    YEAR_LEN    5     /* �곤��     */
#define    ZONE_LEN    3     /* ʱ������   */
#define    LEAP_LEN    3     /* ���볤��   */

/* ���ջ���� */
#define    REV_NAME_LEN 12        /* ���ջ��ͺ����Ƴ��� */


/* pps��� */
#define    PPS_SWITCH_LEN    8 
#define    PPS_TIMEREF_LEN   5
#define    PPS_POLARITY_LEN  9

/* ��ʱָ����� */
#define    TIMING_TYPE_REALTIME  0    /* ʵʱ��λ��ʱ     */
#define    TIMING_TYPE_FIX       1    /* ����̶�������ʱ */
#define    TIMING_TYPE_SURVEYIN  2    /* �����Ż���ʱ     */

/* �澯��� */
#define    ALARM_NAME_LEN        15   /* �澯���Ƴ��� */
#define    ALARM_TIME_LEN        15   /* �澯ʱ�䳤�� */


///* PTP��� */
//#define    PTP_IP_LEN            5     /* ����Ŀ��IP��ַ����  */
//#define    PTP_MAC_LEN           7     /* ����Ŀ��MAC��ַ���� */

/* NTP��� */
#define    NTP_MD5KEY_LEN        33    /* MD5������Կ���� */




/*-------------------------------*/
/* �ṹ����                      */
/*-------------------------------*/

/* ����״̬ */
//typedef struct _SATELLITE_STATUS_T_ 
//{
//	u8_t prn[4];  /* ����PRN��� */
//	u8_t snr[3];  /* �����      */
//	u8_t elev[2]; /* ����        */
//} satellite_sta_t;
typedef struct _SATELLITE_STATUS_T_ 
{
	u8_t prn;  /* ����PRN��� */
    u8_t elev; /* ����        */
	u8_t snr;  /* �����      */
} satellite_sta_t;

typedef struct _LOCATION_MSG_T_
{

  u8_t location[45];

} location_msg_t;


typedef struct _SATE_STATUS_T_ 
{
    u8_t  status_msg[400];
    u16_t msg_len;


} sate_status_t;

/* ʱ����Ϣ */
typedef struct _TIME_T_
{
#if 0	
	u8_t year[YEAR_LEN];    /* ��     */
	u8_t mon[MON_LEN];	     /* ��     */
	u8_t day[DAY_LEN];	     /* ��     */
#endif
	u8_t hms[HMS_LEN];      /* ʱ���� */ 
#if 0
	u8_t zone[ZONE_LEN];    /* ʱ��   */
#endif
} ttime_t;

/* λ����Ϣ */
typedef struct _LOCATION_T_
{
	u8_t lat[LAT_LEN];    /* γ��            */ 
	u8_t latdir;          /* γ�ȷ���        */
	u8_t lon[LON_LEN];    /* ����            */
	u8_t londir;          /* ���ȷ���        */
	u8_t alt[ALT_LEN];    /* ����            */
} location_t;

/* �����ۺ���Ϣ */
//typedef struct _SATELLITE_INFO_T_
//{   
//	location_t      location;          /* λ����Ϣ           */
//	u8_t           use_sats;          /* ʹ���е�������     */
//	u8_t           gps_amount_sats[3];   /* �ɼ���GPS��������  */   
//	satellite_sta_t gps_status[12];    /* GPS����״̬        */
//	u8_t           bds_amount_sats[3];   /* �ɼ��ı����������� */
//	satellite_sta_t bds_status[12];    /* ��������״̬       */
//	u8_t           glo_amount_sats[3];   /* �ɼ���GLO��������  */
//	satellite_sta_t glo_status[12];    /* GLONASS����״̬    */
//	u8_t           gal_amount_sats[3];   /* �ɼ���GAL��������  */
//	satellite_sta_t gal_status[12];    /* GAL����״̬        */
//
//} satellite_info_t;
typedef struct _SATELLITE_INFO_T_
{   
	location_t      location;          /* λ����Ϣ           */
	u8_t           use_sats;          /* ʹ���е�������     */
	u8_t           gps_amount_sats;   /* �ɼ���GPS��������  */   
	satellite_sta_t gps_status[36];    /* GPS����״̬        */
	u8_t           bds_amount_sats;   /* �ɼ��ı����������� */
	satellite_sta_t bds_status[36];    /* ��������״̬       */
	u8_t           glo_amount_sats;   /* �ɼ���GLO��������  */
	satellite_sta_t glo_status[36];    /* GLONASS����״̬    */
	u8_t           gal_amount_sats;   /* �ɼ���GAL��������  */
	satellite_sta_t gal_status[36];    /* GAL����״̬        */

} satellite_info_t;

/* ���ջ���Ϣ */
typedef struct _RECEIVER_INFO_T_
{
	u8_t sat_en;					   /* ���Ƕ�λʹ��    */
	u8_t ant_sta;					   /* ����״̬        */
	u8_t rec_type;		               /* ���յ�����      */
	u8_t ac_mode;	                   /* ���ջ�����ģʽ  */
	u8_t rec_name[REV_NAME_LEN];      /*���յ��ͺ�����   */
    u8_t pps_status;                  /* 1PPS״̬       */

#if 0

	u8_t pos_mode[POS_MODE_LEN];	   /* ��λ״̬        */
	u8_t rec_ver[REV_VER_LEN];		   /* ���ջ��İ汾��  */
	u8_t pps_available;               /* 1PPS�Ƿ����    */	
#endif

} recviver_info_t;

/* ����Ԥ�� */
typedef struct _LEAP_FORECAST_T_
{
	u8_t year[YEAR_LEN];    /* �µ����뷢����     */
	u8_t mon[MON_LEN];      /* �µ����뷢����     */
	u8_t day[DAY_LEN];      /* �µ����뷢����     */	
	u8_t hms[HMS_LEN];      /* �µ����뷢��ʱ���� */
	u8_t next_leap;         /* �µ�����ֵ         */
	s8_t sign;              /* ��/������          */
} leap_forecast_t;

/* ����Ԥ����Ϣ */
typedef struct _LEAP_FORECAST_MSG_T_
{
    u8_t leap_msg[20];
} leap_forecast_msg_t;


/* PPS��Ϣ */
typedef struct _PPS_INFO_T_
{
	u8_t  en_switch[PPS_SWITCH_LEN];  /* PPS���ʹ��/�ر�      */
	u8_t  timeref[PPS_TIMEREF_LEN];   /* ��ǰֻ֧��GPS��BDS    */
	u8_t  polatity[PPS_POLARITY_LEN]; /* PPS������/�½�����Ч  */
	u32_t width;                      /* pps������(΢��)     */
	u32_t period;                     /* pps�����������(����) */
	s32_t rfdelay;                    /* RF�ӳ�(����)          */
	s32_t userdelay;                  /* �û������ӳ�(����)    */
} pps_info_t;

/* �澯���� */
typedef struct _ALARM_T_
{
	u8_t alarm_name;    /* �澯���� */
	u8_t alarm_source;  /* �澯Դ   */
	u8_t alarm_id;      /* �澯ID   */
	u8_t alarm_level;   /* �澯���� */
	u8_t alarm_status;  /* �澯״̬ */
	u8_t alarm_time;    /* �澯ʱ�� */
} alarm_t;

/* �澯��Ϣ���� */
typedef struct _ALARM_INFO_T_
{
	alarm_t sate_lose;         /* �����źŶ�ʧ�澯   */
	alarm_t sate_degrad;       /* �����ź��ӻ��澯   */   
	alarm_t net_linkfail;      /* ����linkfail�澯   */
	alarm_t laser_offline;     /* ���������߸澯     */
	alarm_t optical_lose;      /* ���LOS�澯        */
	alarm_t pll_hold;          /* ���໷���ָ澯     */
	alarm_t pll_free;          /* ���໷�����񵴸澯 */   
	alarm_t pll_fast;          /* ���໷�첶�澯     */
	alarm_t osci_invalid;      /* ����ʧЧ�澯     */
	alarm_t clock_lose;        /* ��ʱ��lose�澯     */
} alarm_info_t;

/* �澯������Ϣ���� */
typedef struct _ALARM_MASK_T_
{
	u16_t   sate_lose_mask;    /* �����źŶ�ʧ�澯����   */
	u16_t   sate_degrad_mask;  /* �����ź��ӻ��澯����   */
	u16_t   net_linkfail_mask; /* ����linkfail�澯����   */
	u16_t   laser_offline_mask;/* ���������߸澯����     */
	u16_t   optical_lose_mask; /* ���LOS�澯����        */
	u16_t   pll_hold_mask;     /* ���໷���ָ澯����     */
	u16_t   pll_free_mask;     /* ���໷�����񵴸澯���� */
	u16_t   pll_fast_mask;     /* ���໷�첶�澯����     */
	u16_t   osci_invalid_mask; /* ����ʧЧ�澯����     */
	u16_t   clock_lose_mask;   /* ��ʱ��lose�澯����     */
} alarm_mask_t;

/* PTP���� */
typedef struct _PTP_INFO_T_
{
	u8_t   domain;           /* PTP��                   */
	u8_t   ip[IP_LEN];       /* ����Ŀ��IP��ַ          */
	u8_t   mac[MAC_LEN];     /* ����Ŀ��MAC��ַ         */
	u8_t   compensation;     /* �ǶԳ���ʱ����          */
	u8_t   prio1;            /* ���ȼ�1                 */
	u8_t   prio2;            /* ���ȼ�2                 */
	u8_t   enp;              /* ���ķ�װ��ʽ(ETH/UDP)   */
	u8_t   delay;            /* ��ʱ����                */
	u8_t   step;             /* ʱ������ģʽ            */
	u8_t   enable;           /* �˿�PTPʹ��/��ʹ��      */
	u8_t   esmc;             /* �˿�ESMC����ʹ��/��ʹ�� */
	u8_t   announce;         /* Announce����Ƶ��(1/16Hz��256Hz��Ĭ��8Hz) */
	u8_t   sync;             /* Sync����Ƶ��(1/16Hz��256Hz��Ĭ��16Hz)    */
} ptp_info_t;

/* NTP���� */
typedef struct _NTP_INFO_T_
{
	u8_t   mul_enable;              /* NTP�㲥ʹ��/��ʹ�� */
	u8_t   md5_enable;              /* MD5����ʹ��/��ʹ�� */
	u8_t   md5_key[NTP_MD5KEY_LEN]; /* MD5������Կ        */
} ntp_info_t;

/* ������Ϣ */
typedef struct _OVERALL_INFO_T_
{
	u8_t    protocol_type;        /* PTPЭ��ʹ��/NTPЭ��ʹ�� */
	u8_t    ptp_version;          /* PTPЭ��汾             */
	u8_t    ip[IP_LEN];           /* ����IP                  */
	u8_t    netmask[MASK_LEN];    /* ����                    */
	u8_t    gateway[GATEWAY_LEN]; /* ���ص�ַ                */
	u8_t    gnss_enable;          /* GNSS�ź�ʹ��            */
	u8_t    time_zone;            /* IRIG-B���ʱ��          */
	u8_t    tod_type;             /* TOD�����ʽ             */
	u8_t    ssm_enable;           /* SSM���ķ���ʹ��         */
	u8_t    pll_in_compen;        /* ���໷������ʱ����      */
	u8_t    pll_out_compen;       /* ���໷�����ʱ����      */

} overall_info_t;

/* ������Ϣ */
typedef struct _NET_INFO_T_
{
	u8_t    mac[MAC_LEN];         /* MAC��ַ                 */
	u8_t    source_ip[IP_LEN];    /* ԴIP��ַ                */
	u8_t    netmask[MASK_LEN];    /* ����                    */
	u8_t    gateway[GATEWAY_LEN]; /* ���ص�ַ                */
	u8_t    vlan_enable;          /* VLANʹ��                */
	u8_t    vlan_id;              /* VLAN ID                 */
	u8_t    vlan_pri;             /* VLAN PRI                */

} net_info_t;

/* ��ģ�鶨�� */
typedef struct _SFP_INFO_T_
{
	u16_t current_opower;    /* ��ǰ���͹⹦�� */
	u16_t max_opower;        /* ����͹⹦�� */
	u16_t min_opower;        /* ��С���͹⹦�� */
	u16_t current_ipower;    /* ��ǰ���չ⹦�� */
	u16_t max_ipower;        /* �����չ⹦�� */
	u16_t min_ipower;        /* ��С���չ⹦�� */
	u16_t current_bias;      /* ��ǰƫ�õ���   */
	u16_t max_bias;          /* ���ƫ�õ���   */
	u16_t min_bias;          /* ��Сƫ�õ���   */
	s16_t current_temp;      /* ��ģ�鵱ǰ�¶� */
	s16_t max_temp;          /* ��ģ������¶� */
	s16_t min_temp;          /* ��ģ������¶� */

} sfp_info_t;

/* ��ʷ���ݶ��� */
typedef struct _HISTORY_T_
{
	u16_t max15;    /* ǰ1~15���������ֵ  */
	u16_t min15;    /* ǰ1~15��������Сֵ  */
	u16_t avg15;    /* ǰ1~15������ƽ��ֵ  */
	u16_t max30;    /* ǰ16~30���������ֵ */
	u16_t min30;    /* ǰ16~30��������Сֵ */
	u16_t avg30;    /* ǰ16~30������ƽ��ֵ */
	u16_t max45;    /* ǰ31~45���������ֵ */
	u16_t min45;    /* ǰ31~45��������Сֵ */
	u16_t avg45;    /* ǰ31~45������ƽ��ֵ */
	u16_t max60;    /* ǰ46~60���������ֵ */
	u16_t min60;    /* ǰ46~60��������Сֵ */
	u16_t avg60;    /* ǰ46~60������ƽ��ֵ */
} history_t;

/* ��ģ����ʷ�¶ȶ���(�¶ȿ���Ϊ��ֵ) */
typedef struct _SFP_TEMP_HISTORY_T_
{
	s16_t max15;    /* ǰ1~15���������ֵ  */
	s16_t min15;    /* ǰ1~15��������Сֵ  */
	s16_t avg15;    /* ǰ1~15������ƽ��ֵ  */
	s16_t max30;    /* ǰ16~30���������ֵ */
	s16_t min30;    /* ǰ16~30��������Сֵ */
	s16_t avg30;    /* ǰ16~30������ƽ��ֵ */
	s16_t max45;    /* ǰ31~45���������ֵ */
	s16_t min45;    /* ǰ31~45��������Сֵ */
	s16_t avg45;    /* ǰ31~45������ƽ��ֵ */
	s16_t max60;    /* ǰ46~60���������ֵ */
	s16_t min60;    /* ǰ46~60��������Сֵ */
	s16_t avg60;    /* ǰ46~60������ƽ��ֵ */
} sfp_temp_history_t;

/* ��ģ����ʷ���� */
typedef struct _SFP_HISTORY_T_
{
	history_t              opower_history;    /* ���͹⹦����ʷ���� */
	history_t              ipower_history;    /* ���չ⹦����ʷ���� */
	history_t              bias_history;      /* ƫ�õ�����ʷ����   */
	sfp_temp_history_t     temp_history;      /* ��ģ���¶���ʷ���� */        
} sfp_history_t;

/* ������ʷ */
typedef struct _MSG_HISTORY_T_
{
	u16_t msg_15min;     /* ǰ1~15���ӵ�����  */
	u16_t msg_30min;     /* ǰ16~30���ӵ����� */
	u16_t msg_45min;     /* ǰ31~45���ӵ����� */
	u16_t msg_60min;     /* ǰ46~60���ӵ����� */
} msg_history_t;

/* ����ͳ�� */
typedef struct _MSG_CNT_T_
{
	msg_history_t recv_packets;            /* Total Receive Packets           */
	msg_history_t recv_crcerror;           /* Receive CRC Error Packets       */
	msg_history_t recv_nms;                /* Receive NMS Packets             */
	msg_history_t recv_ptp;                /* Receive PTP packets, include delay_req_ipv4 and pdelay_req_ipv4 packets. */
	msg_history_t recv_arp;                /* Receive ARP Packets.            */
	msg_history_t recv_icmp;               /* Receive ICMP Packets            */
	msg_history_t recv_annouce;            /* Received Announce Message Count */
	msg_history_t recv_signaling;          /* Received Sgnling Message Count  */
	msg_history_t recv_delreq;             /* Received Delreq Message Count   */
	msg_history_t recv_pdelreq;            /* Received PDelreq Message Count  */
	msg_history_t recv_ntprequest;         /* Received NTPrequest Packets     */
	msg_history_t transmit_packets;        /* Total Transmit Packets          */
	msg_history_t transmit_annouce;        /* Sent Announce Message Count     */
	msg_history_t transmit_sync;           /* Sent Sync Message Count         */
	msg_history_t transmit_synce;          /* Transmit SyncE SSM Packets      */
	msg_history_t transmit_flwup;          /* Sent Flwup Message Count        */
	msg_history_t transmit_delresp;        /* Sent Delresp Message Count      */
	msg_history_t transmit_pdelrespflwup;  /* Sent PDrspflw Message Count     */
	msg_history_t transmit_signaling;      /* Sent Sgnling Message Count      */
	msg_history_t transmit_arpreply;       /* Transmit ARP Reply Packets      */
	msg_history_t transmit_management;     /* Transmit Management Packets     */
	msg_history_t transmit_icmpreply;      /* Transmit ICMP Reply Packets     */
	msg_history_t transmit_intpresponse;   /* TransmitINTPresponse Packets    */

} msg_cnt_t;









/* ��������ṹ���� */
typedef struct  __CMDHANDLE_T_
{
	u8_t* cmd;
	void (*cmd_fun)(u8_t *pparam);
} cmdhandle_t;
#endif
