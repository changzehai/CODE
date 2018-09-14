/*****************************************************************************/
/* 文件名:    mtfs30.h                                                       */
/* 描  述:    MTFS30公用头文件                                               */
/* 创  建:    2018-07-19 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#ifndef __MTFS30_H_
#define __MTFS30_H_
#include "stdio.h"
#include "string.h"
#include "cc.h"


/*-------------------------------*/
/* 宏定义                        */
/*-------------------------------*/

#define    OK          0      /* 正常 */
#define    NG          1      /* 异常 */ 

/* 在定义长度时，要多加一个位结束符 '\0' */

#define    IP_LEN      5     /* IP地址长度  */
#define    MAC_LEN     7     /* MAC地址长度 */
#define    MASK_LEN    5     /* 掩码长度    */
#define    GATEWAY_LEN 5     /* 网关长度    */

/* 位置信息相关 */
#define    LAT_LEN     14    /* 纬度长度 */
#define    LON_LEN     15    /* 经度长度 */
#define    ALT_LEN     9     /* 海拔长度 */



/* 时间相关 */
#define    HMS_LEN     10    /* 时分秒长度 */
#define    DAY_LEN     3     /* 日长度     */
#define    MON_LEN     3     /* 月长度     */
#define    YEAR_LEN    5     /* 年长度     */
#define    ZONE_LEN    3     /* 时区长度   */
#define    LEAP_LEN    3     /* 闰秒长度   */

/* 接收机相关 */
#define    REV_NAME_LEN 12        /* 接收机型号名称长度 */


/* pps相关 */
#define    PPS_SWITCH_LEN    8 
#define    PPS_TIMEREF_LEN   5
#define    PPS_POLARITY_LEN  9

/* 授时指令相关 */
#define    TIMING_TYPE_REALTIME  0    /* 实时定位授时     */
#define    TIMING_TYPE_FIX       1    /* 输入固定坐标授时 */
#define    TIMING_TYPE_SURVEYIN  2    /* 自主优化授时     */

/* 告警相关 */
#define    ALARM_NAME_LEN        15   /* 告警名称长度 */
#define    ALARM_TIME_LEN        15   /* 告警时间长度 */


///* PTP相关 */
//#define    PTP_IP_LEN            5     /* 单播目的IP地址长度  */
//#define    PTP_MAC_LEN           7     /* 单播目的MAC地址长度 */

/* NTP相关 */
#define    NTP_MD5KEY_LEN        33    /* MD5加密密钥长度 */




/*-------------------------------*/
/* 结构定义                      */
/*-------------------------------*/

/* 卫星状态 */
//typedef struct _SATELLITE_STATUS_T_ 
//{
//	u8_t prn[4];  /* 卫星PRN编号 */
//	u8_t snr[3];  /* 信噪比      */
//	u8_t elev[2]; /* 仰角        */
//} satellite_sta_t;
typedef struct _SATELLITE_STATUS_T_ 
{
	u8_t prn;  /* 卫星PRN编号 */
    u8_t elev; /* 仰角        */
	u8_t snr;  /* 信噪比      */
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

/* 时间信息 */
typedef struct _TIME_T_
{
#if 0	
	u8_t year[YEAR_LEN];    /* 年     */
	u8_t mon[MON_LEN];	     /* 月     */
	u8_t day[DAY_LEN];	     /* 日     */
#endif
	u8_t hms[HMS_LEN];      /* 时分秒 */ 
#if 0
	u8_t zone[ZONE_LEN];    /* 时区   */
#endif
} ttime_t;

/* 位置信息 */
typedef struct _LOCATION_T_
{
	u8_t lat[LAT_LEN];    /* 纬度            */ 
	u8_t latdir;          /* 纬度方向        */
	u8_t lon[LON_LEN];    /* 经度            */
	u8_t londir;          /* 经度方向        */
	u8_t alt[ALT_LEN];    /* 海拔            */
} location_t;

/* 卫星综合信息 */
//typedef struct _SATELLITE_INFO_T_
//{   
//	location_t      location;          /* 位置信息           */
//	u8_t           use_sats;          /* 使用中的卫星数     */
//	u8_t           gps_amount_sats[3];   /* 可见的GPS卫星总数  */   
//	satellite_sta_t gps_status[12];    /* GPS卫星状态        */
//	u8_t           bds_amount_sats[3];   /* 可见的北斗卫星总数 */
//	satellite_sta_t bds_status[12];    /* 北斗卫星状态       */
//	u8_t           glo_amount_sats[3];   /* 可见的GLO卫星总数  */
//	satellite_sta_t glo_status[12];    /* GLONASS卫星状态    */
//	u8_t           gal_amount_sats[3];   /* 可见的GAL卫星总数  */
//	satellite_sta_t gal_status[12];    /* GAL卫星状态        */
//
//} satellite_info_t;
typedef struct _SATELLITE_INFO_T_
{   
	location_t      location;          /* 位置信息           */
	u8_t           use_sats;          /* 使用中的卫星数     */
	u8_t           gps_amount_sats;   /* 可见的GPS卫星总数  */   
	satellite_sta_t gps_status[36];    /* GPS卫星状态        */
	u8_t           bds_amount_sats;   /* 可见的北斗卫星总数 */
	satellite_sta_t bds_status[36];    /* 北斗卫星状态       */
	u8_t           glo_amount_sats;   /* 可见的GLO卫星总数  */
	satellite_sta_t glo_status[36];    /* GLONASS卫星状态    */
	u8_t           gal_amount_sats;   /* 可见的GAL卫星总数  */
	satellite_sta_t gal_status[36];    /* GAL卫星状态        */

} satellite_info_t;

/* 接收机信息 */
typedef struct _RECEIVER_INFO_T_
{
	u8_t sat_en;					   /* 卫星定位使能    */
	u8_t ant_sta;					   /* 天线状态        */
	u8_t rec_type;		               /* 接收的类型      */
	u8_t ac_mode;	                   /* 接收机工作模式  */
	u8_t rec_name[REV_NAME_LEN];      /*接收的型号名称   */
    u8_t pps_status;                  /* 1PPS状态       */

#if 0

	u8_t pos_mode[POS_MODE_LEN];	   /* 定位状态        */
	u8_t rec_ver[REV_VER_LEN];		   /* 接收机的版本号  */
	u8_t pps_available;               /* 1PPS是否可用    */	
#endif

} recviver_info_t;

/* 闰秒预告 */
typedef struct _LEAP_FORECAST_T_
{
	u8_t year[YEAR_LEN];    /* 新的闰秒发生年     */
	u8_t mon[MON_LEN];      /* 新的闰秒发生月     */
	u8_t day[DAY_LEN];      /* 新的闰秒发生日     */	
	u8_t hms[HMS_LEN];      /* 新的闰秒发生时分秒 */
	u8_t next_leap;         /* 新的闰秒值         */
	s8_t sign;              /* 正/负闰秒          */
} leap_forecast_t;

/* 闰秒预告信息 */
typedef struct _LEAP_FORECAST_MSG_T_
{
    u8_t leap_msg[20];
} leap_forecast_msg_t;


/* PPS信息 */
typedef struct _PPS_INFO_T_
{
	u8_t  en_switch[PPS_SWITCH_LEN];  /* PPS输出使能/关闭      */
	u8_t  timeref[PPS_TIMEREF_LEN];   /* 当前只支持GPS和BDS    */
	u8_t  polatity[PPS_POLARITY_LEN]; /* PPS上升沿/下降沿有效  */
	u32_t width;                      /* pps脉冲宽度(微秒)     */
	u32_t period;                     /* pps脉冲输出周期(毫秒) */
	s32_t rfdelay;                    /* RF延迟(纳秒)          */
	s32_t userdelay;                  /* 用户设置延迟(纳秒)    */
} pps_info_t;

/* 告警定义 */
typedef struct _ALARM_T_
{
	u8_t alarm_name;    /* 告警名称 */
	u8_t alarm_source;  /* 告警源   */
	u8_t alarm_id;      /* 告警ID   */
	u8_t alarm_level;   /* 告警级别 */
	u8_t alarm_status;  /* 告警状态 */
	u8_t alarm_time;    /* 告警时间 */
} alarm_t;

/* 告警信息定义 */
typedef struct _ALARM_INFO_T_
{
	alarm_t sate_lose;         /* 卫星信号丢失告警   */
	alarm_t sate_degrad;       /* 卫星信号劣化告警   */   
	alarm_t net_linkfail;      /* 网口linkfail告警   */
	alarm_t laser_offline;     /* 激光器离线告警     */
	alarm_t optical_lose;      /* 光口LOS告警        */
	alarm_t pll_hold;          /* 锁相环保持告警     */
	alarm_t pll_free;          /* 锁相环自由振荡告警 */   
	alarm_t pll_fast;          /* 锁相环快捕告警     */
	alarm_t osci_invalid;      /* 振荡器失效告警     */
	alarm_t clock_lose;        /* 外时钟lose告警     */
} alarm_info_t;

/* 告警屏蔽信息定义 */
typedef struct _ALARM_MASK_T_
{
	u16_t   sate_lose_mask;    /* 卫星信号丢失告警屏蔽   */
	u16_t   sate_degrad_mask;  /* 卫星信号劣化告警屏蔽   */
	u16_t   net_linkfail_mask; /* 网口linkfail告警屏蔽   */
	u16_t   laser_offline_mask;/* 激光器离线告警屏蔽     */
	u16_t   optical_lose_mask; /* 光口LOS告警屏蔽        */
	u16_t   pll_hold_mask;     /* 锁相环保持告警屏蔽     */
	u16_t   pll_free_mask;     /* 锁相环自由振荡告警屏蔽 */
	u16_t   pll_fast_mask;     /* 锁相环快捕告警屏蔽     */
	u16_t   osci_invalid_mask; /* 振荡器失效告警屏蔽     */
	u16_t   clock_lose_mask;   /* 外时钟lose告警屏蔽     */
} alarm_mask_t;

/* PTP定义 */
typedef struct _PTP_INFO_T_
{
	u8_t   domain;           /* PTP域                   */
	u8_t   ip[IP_LEN];       /* 单播目的IP地址          */
	u8_t   mac[MAC_LEN];     /* 单播目的MAC地址         */
	u8_t   compensation;     /* 非对称延时补偿          */
	u8_t   prio1;            /* 优先级1                 */
	u8_t   prio2;            /* 优先级2                 */
	u8_t   enp;              /* 报文封装格式(ETH/UDP)   */
	u8_t   delay;            /* 延时机制                */
	u8_t   step;             /* 时戳发送模式            */
	u8_t   enable;           /* 端口PTP使能/不使能      */
	u8_t   esmc;             /* 端口ESMC报文使能/不使能 */
	u8_t   announce;         /* Announce发包频率(1/16Hz到256Hz，默认8Hz) */
	u8_t   sync;             /* Sync发包频率(1/16Hz到256Hz，默认16Hz)    */
} ptp_info_t;

/* NTP定义 */
typedef struct _NTP_INFO_T_
{
	u8_t   mul_enable;              /* NTP广播使能/不使能 */
	u8_t   md5_enable;              /* MD5加密使能/不使能 */
	u8_t   md5_key[NTP_MD5KEY_LEN]; /* MD5加密密钥        */
} ntp_info_t;

/* 总体信息 */
typedef struct _OVERALL_INFO_T_
{
	u8_t    protocol_type;        /* PTP协议使能/NTP协议使能 */
	u8_t    ptp_version;          /* PTP协议版本             */
	u8_t    ip[IP_LEN];           /* 管理IP                  */
	u8_t    netmask[MASK_LEN];    /* 掩码                    */
	u8_t    gateway[GATEWAY_LEN]; /* 网关地址                */
	u8_t    gnss_enable;          /* GNSS信号使能            */
	u8_t    time_zone;            /* IRIG-B输出时区          */
	u8_t    tod_type;             /* TOD输出格式             */
	u8_t    ssm_enable;           /* SSM报文发送使能         */
	u8_t    pll_in_compen;        /* 锁相环输入延时补偿      */
	u8_t    pll_out_compen;       /* 锁相环输出延时补偿      */

} overall_info_t;

/* 网络信息 */
typedef struct _NET_INFO_T_
{
	u8_t    mac[MAC_LEN];         /* MAC地址                 */
	u8_t    source_ip[IP_LEN];    /* 源IP地址                */
	u8_t    netmask[MASK_LEN];    /* 掩码                    */
	u8_t    gateway[GATEWAY_LEN]; /* 网关地址                */
	u8_t    vlan_enable;          /* VLAN使能                */
	u8_t    vlan_id;              /* VLAN ID                 */
	u8_t    vlan_pri;             /* VLAN PRI                */

} net_info_t;

/* 光模块定义 */
typedef struct _SFP_INFO_T_
{
	u16_t current_opower;    /* 当前发送光功率 */
	u16_t max_opower;        /* 最大发送光功率 */
	u16_t min_opower;        /* 最小发送光功率 */
	u16_t current_ipower;    /* 当前接收光功率 */
	u16_t max_ipower;        /* 最大接收光功率 */
	u16_t min_ipower;        /* 最小接收光功率 */
	u16_t current_bias;      /* 当前偏置电流   */
	u16_t max_bias;          /* 最大偏置电流   */
	u16_t min_bias;          /* 最小偏置电流   */
	s16_t current_temp;      /* 光模块当前温度 */
	s16_t max_temp;          /* 光模块最高温度 */
	s16_t min_temp;          /* 光模块最低温度 */

} sfp_info_t;

/* 历史数据定义 */
typedef struct _HISTORY_T_
{
	u16_t max15;    /* 前1~15分钟内最大值  */
	u16_t min15;    /* 前1~15分钟内最小值  */
	u16_t avg15;    /* 前1~15分钟内平均值  */
	u16_t max30;    /* 前16~30分钟内最大值 */
	u16_t min30;    /* 前16~30分钟内最小值 */
	u16_t avg30;    /* 前16~30分钟内平均值 */
	u16_t max45;    /* 前31~45分钟内最大值 */
	u16_t min45;    /* 前31~45分钟内最小值 */
	u16_t avg45;    /* 前31~45分钟内平均值 */
	u16_t max60;    /* 前46~60分钟内最大值 */
	u16_t min60;    /* 前46~60分钟内最小值 */
	u16_t avg60;    /* 前46~60分钟内平均值 */
} history_t;

/* 光模块历史温度定义(温度可能为负值) */
typedef struct _SFP_TEMP_HISTORY_T_
{
	s16_t max15;    /* 前1~15分钟内最大值  */
	s16_t min15;    /* 前1~15分钟内最小值  */
	s16_t avg15;    /* 前1~15分钟内平均值  */
	s16_t max30;    /* 前16~30分钟内最大值 */
	s16_t min30;    /* 前16~30分钟内最小值 */
	s16_t avg30;    /* 前16~30分钟内平均值 */
	s16_t max45;    /* 前31~45分钟内最大值 */
	s16_t min45;    /* 前31~45分钟内最小值 */
	s16_t avg45;    /* 前31~45分钟内平均值 */
	s16_t max60;    /* 前46~60分钟内最大值 */
	s16_t min60;    /* 前46~60分钟内最小值 */
	s16_t avg60;    /* 前46~60分钟内平均值 */
} sfp_temp_history_t;

/* 光模块历史数据 */
typedef struct _SFP_HISTORY_T_
{
	history_t              opower_history;    /* 发送光功率历史数据 */
	history_t              ipower_history;    /* 接收光功率历史数据 */
	history_t              bias_history;      /* 偏置电流历史数据   */
	sfp_temp_history_t     temp_history;      /* 光模块温度历史数据 */        
} sfp_history_t;

/* 报文历史 */
typedef struct _MSG_HISTORY_T_
{
	u16_t msg_15min;     /* 前1~15分钟的数据  */
	u16_t msg_30min;     /* 前16~30分钟的数据 */
	u16_t msg_45min;     /* 前31~45分钟的数据 */
	u16_t msg_60min;     /* 前46~60分钟的数据 */
} msg_history_t;

/* 报文统计 */
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









/* 命令处理器结构定义 */
typedef struct  __CMDHANDLE_T_
{
	u8_t* cmd;
	void (*cmd_fun)(u8_t *pparam);
} cmdhandle_t;
#endif
