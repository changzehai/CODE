//*****************************************************************************
//
// config.h - Configuration of the serial to Ethernet converter.
//
// Copyright (c) 2008 Luminary Micro, Inc.  All rights reserved.
// Software License Agreement
// 
// Luminary Micro, Inc. (LMI) is supplying this software for use solely and
// exclusively on LMI's microcontroller products.
// 
// The software is owned by LMI and/or its suppliers, and is protected under
// applicable copyright laws.  All rights are reserved.  You may not combine
// this software with "viral" open-source software in order to form a larger
// program.  Any use in violation of the foregoing restrictions may subject
// the user to criminal sanctions under applicable laws, as well as to civil
// liability for the breach of the terms and conditions of this license.
// 
// THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
// LMI SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
// CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 3740 of the RDK-S2E Firmware Package.
//
//*****************************************************************************

#ifndef __CONFIG_H__
#define __CONFIG_H__

//*****************************************************************************
//
//! \addtogroup config_api
//! @{
//
//*****************************************************************************
typedef unsigned char tBoolean;

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif
//*****************************************************************************
//
// Define a boolean type, and values for true and false.
//
//*****************************************************************************



//*****************************************************************************
//
//! The number of serial to Ethernet ports supported by this module.
//
//*****************************************************************************
#define MAX_S2E_PORTS           1

//*****************************************************************************
//
//! This structure defines the port parameters used to configure the UART and
//! telnet session associated with a single instance of a port on the S2E
//! module.
//
//*****************************************************************************
typedef struct
{
    //
    //! The baud rate to be used for the UART, specified in bits-per-second
    //! (bps).
    //
    unsigned long ulBaudRate;   //4 byte

    //
    //! The data size to be use for the serial port, specified in bits.  Valid
    //! values are 5, 6, 7, and 8.
    //
    unsigned char ucDataSize;  //5 byte

    //
    //! The parity to be use for the serial port, specified as an enumeration.
    //! Valid values are 1 for no parity, 2 for odd parity, 3 for even parity,
    //! 4 for mark parity, and 5 for space parity.
    //
    unsigned char ucParity;   // 6 byte

    //
    //! The number of stop bits to be use for the serial port, specified as
    //! a number.  Valid values are 1 and 2.
    //
    unsigned char ucStopBits;  //7 byte

    //
    //! The flow control to be use for the serial port, specified as an
    //! enumeration.  Valid values are 1 for no flow control and 3 for HW
    //! (RTS/CTS) flow control.
    //
    unsigned char ucFlowControl;//8 byte

    //
    //! The timeout for the TCP connection used for the telnet session,
    //! specified in seconds.  A value of 0 indicates no timeout is to
    //! be used.
    //
    unsigned long ulTelnetTimeout;//12 byte

    //
    //! The local TCP port number to be listened on when in server mode or
    //! from which the outgoing connection will be initiated in client mode.
    //
    unsigned short usTelnetLocalPort;//14 byte

    //
    //! The TCP port number to which a connection will be established when in
    //! telnet client mode.
    //
    unsigned short usTelnetRemotePort;//16 byte

    //
    //! The IP address which will be connected to when in telnet client mode.
    //
    unsigned long ulTelnetIPAddr;     //20 byte

    //
    //! Miscellaneous flags associated with this connection.
    //
    unsigned char ucFlags;            //21 byte

    //! Padding to ensure consistent parameter block alignment, and
    //! to allow for future expansion of port parameters.
    //
    unsigned char ucReserved0[19];    //40byte
}
tPortParameters;
typedef struct
{
    //
    //! The baud rate to be used for the UART, specified in bits-per-second
    //! (bps).
    //
    unsigned short  year;   //2

    //
    //! The data size to be use for the serial port, specified in bits.  Valid
    //! values are 5, 6, 7, and 8.
    //
    unsigned char month;    //3

    //
    //! The parity to be use for the serial port, specified as an enumeration.
    //! Valid values are 1 for no parity, 2 for odd parity, 3 for even parity,
    //! 4 for mark parity, and 5 for space parity.
    //
    unsigned char day;      //4 

    //
    //! The number of stop bits to be use for the serial port, specified as
    //! a number.  Valid values are 1 and 2.
    //
    unsigned char hour;    //5

    //
    //! The flow control to be use for the serial port, specified as an
    //! enumeration.  Valid values are 1 for no flow control and 3 for HW
    //! (RTS/CTS) flow control.
    //
    unsigned char minute;

    //
    //! The timeout for the TCP connection used for the telnet session,
    //! specified in seconds.  A value of 0 indicates no timeout is to
    //! be used.
    //
    unsigned char second;//7
}
tTimeParameters;
typedef struct
{
    //
    //! The baud rate to be used for the UART, specified in bits-per-second
    //! (bps).
    //
    unsigned char  Timezone; //1 byte 

    //
    //! The data size to be use for the serial port, specified in bits.  Valid
    //! values are 5, 6, 7, and 8.
    //
    unsigned char Switchmode;//2 byte

    //
    //! The parity to be use for the serial port, specified as an enumeration.
    //! Valid values are 1 for no parity, 2 for odd parity, 3 for even parity,
    //! 4 for mark parity, and 5 for space parity.
    //
    unsigned char Manulswitch;//3 byte

    //
    //! The number of stop bits to be use for the serial port, specified as
    //! a number.  Valid values are 1 and 2.
    //
    //
		
    //! The local TCP port number to be listened on when in server mode or
    //! from which the outgoing connection will be initiated in client mode.
    //
    unsigned long ntpip;//7byte

    //
    //! The TCP port number to which a connection will be established when in
    //! telnet client mode.
    //
    unsigned long dnsip;//11 byte

    //
    //! The IP address which will be connected to when in telnet client mode.
    //
    unsigned short tcpport;//13 byte

    //
    //! Miscellaneous flags associated with this connection.
    //
    //! Padding to ensure consistent parameter block alignment, and
    //! to allow for future expansion of port parameters.
    //
}
tmoduleParameters;

//*****************************************************************************
//
//! Bit 0 of field ucFlags in tPortParameters is used to indicate whether to
//! operate as a telnet server or a telnet client.
//
//*****************************************************************************
#define PORT_FLAG_TELNET_MODE   0x01

//*****************************************************************************
//
// Helpful labels used to determine if we are operating as a telnet client or
// server (based on the state of the PORT_FLAG_TELNET_MODE bit in the ucFlags
// field of tPortParameters).
//
//*****************************************************************************
#define PORT_TELNET_SERVER      0x00
#define PORT_TELNET_CLIENT      PORT_FLAG_TELNET_MODE


//*****************************************************************************
//
//! Bit 1 of field ucFlags in tPortParameters is used to indicate whether to
//! bypass the telnet protocol (raw data mode).
//
//*****************************************************************************
#define PORT_FLAG_PROTOCOL      0x02

//*****************************************************************************
//
// Helpful labels used to determine if we are operating in raw data mode, or
// in telnet protocol mode.
//
//*****************************************************************************
#define PORT_PROTOCOL_TELNET    0x00
#define PORT_PROTOCOL_RAW       PORT_FLAG_PROTOCOL

#define PORT_RAW_CLIENT        PORT_TELNET_CLIENT|PORT_PROTOCOL_RAW
//*****************************************************************************
//
// The length of the ucModName array in the tConfigParameters structure.  NOTE:
// Be extremely careful if changing this since the structure packing relies
// upon this values!
//
//*****************************************************************************
#define DEFAULT_STR_LEN         10
#define MOD_NAME_LEN            30
#define URL_NAME_LEN            64
#define BREAU_PD_LEN             6
#define DEVICE_INFO_LEN         30
#define ALL_CHANNEL_ON          3
#define ALL_CHANNEL_OFF         2
typedef struct
{
  unsigned char flag0;
  unsigned char flag1;
  unsigned char leakoffset;
  unsigned char curroffset;
  unsigned long Vk;
  unsigned long Lk;
  unsigned long Ik;
  unsigned char  reserved[16];
}
tSysCALParameters;
//*****************************************************************************
//
//! This structure contains the S2E module parameters that are saved to flash.
//! A copy exists in RAM for use during the execution of the application, which
//! is loaded from flash at startup.  The modified parameter block can also be
//! written back to flash for use on the next power cycle.
//
//*****************************************************************************
typedef struct
{
    //
    //! The sequence number of this parameter block.  When in RAM, this value
    //! is not used.  When in flash, this value is used to determine the
    //! parameter block with the most recent information.
    //
    unsigned char ucSequenceNum;  

    //
    //! The CRC of the parameter block.  When in RAM, this value is not used.
    //! When in flash, this value is used to validate the contents of the
    //! parameter block (to avoid using a partially written parameter block).
    //
    unsigned char ucCRC;         

    //
    //! The version of this parameter block.  This can be used to distinguish
    //! saved parameters that correspond to an old version of the parameter
    //! block.
    //
    unsigned char ucVersion;    

    //
    //! Character field used to store various bit flags.
    //
    unsigned char ucFlags;    //0x01 Static IP;0x02:Trap en;  0x04:Sntp en; 

    //
    //! Root log used to store various bit flags.
    //
		 unsigned char ucSyslog;
    //
    //! The TCP port number to be used for access to the UPnP Location URL that
    //! is part of the discovery response message.
    //
    unsigned short usLocationURLPort; 

    //
    //! Padding to ensure consistent parameter block alignment.
    //
    unsigned char ucReserved1[2];     

    //
    //! The configuration parameters for each port available on the S2E
    //! module.
    //
    tPortParameters sPort[MAX_S2E_PORTS];

    //
    //! An ASCII string used to identify the module to users via UPnP and
    //! web configuration.
    //
    unsigned char ucModName[MOD_NAME_LEN];
     //
    //! The MAC address .
    //
    unsigned char ulMACAddr[6]; 
    //
    //! The static IP address to use if DHCP is not in use.
    //
    unsigned long ulStaticIP;             

    //
    //! The default gateway IP address to use if DHCP is not in use.
    //
    unsigned long ulGatewayIP;           

    //
    //! The subnet mask to use if DHCP is not in use.
    //
    unsigned long ulSubnetMask;         
    //
    //! The DNS ADDRESS.
    //
    unsigned long ulDNSAddr; 
    //
    //! Padding to ensure the whole structure is 256 bytes long.
    //
    unsigned char userpasswd[16]; 
    //
    //! start time and end close time .
    //		      
    tTimeParameters    boottime[4];        
    tTimeParameters    shuttime[4]; 
   	//
    //! SNTP .
    //	
    unsigned short    SNTPInterval;
    unsigned long     SNTPService;
    unsigned char     SNTPURL[URL_NAME_LEN];
    //
    //! SNMP .
    //
    unsigned char     CommunityStr[16];		
    unsigned char     TrapService[URL_NAME_LEN];
    unsigned long     TrapPort;		
		//
    //! Web Port .
    //	     
    unsigned short     webport;   
    //
    //!Device Info
    //	
    unsigned char      Model[DEVICE_INFO_LEN];	
    unsigned char      Manufacturer[DEVICE_INFO_LEN];
    unsigned char      Serial[13];		
    unsigned char      InstallPos[DEVICE_INFO_LEN];
    unsigned char      InstallPer[DEVICE_INFO_LEN];
    unsigned char      InstallTime[DEVICE_INFO_LEN]; 
    unsigned long      RunSecond;
   //
   //!Power Parameters
   //
    unsigned short     OverVoltage;
    unsigned short     UVLO;
    unsigned short     VoltageDeadBand;
    unsigned short     OverCurrent;
    unsigned short     CurrentDeadBand;
    unsigned short     LeakCurrent;
   //
   //!CH1-CH4 Command and Alarm
   //
    unsigned char      remote_cmd;//0x01 ch1 open;0x02 ch2 open;0x04 ch3 open;0x08 ch4 open;0x10:CH1 Time en;0x20:CH2 Time en;0x40:CH3 Time en;0x80:CH4 Time en;
	  
    unsigned char      rgcnt;
    unsigned short     delay1;
    unsigned short     delay2;
    unsigned short     delay3;
    unsigned char      updata[6];
    unsigned char      shellpasswd[16]; 
    unsigned char      ucReserved2[16];      
		
}
tConfigParameters;


//*****************************************************************************
//
//! If this flag is set in the ucFlags field of tConfigParameters, the module
//! uses a static IP.  If not, DHCP and AutoIP are used to obtain an IP
//! address.
//
//*****************************************************************************
#define CONFIG_FLAG_STATICIP    0x01
//*****************************************************************************
//
//! If this flag is set in the ucFlags field of tConfigParameters, the module
//! Enable sntp.  If not, time don't updata from sntp.
//! address.
//
//*****************************************************************************
#define CONFIG_EN_SNTP    		 0x04
//*****************************************************************************
//
//! If this flag is set in the ucFlags field of tConfigParameters, the module
//! Enable trap.  If not, snmp trap disable.
//! address.
//
//*****************************************************************************
#define CONFIG_EN_TRAP    		 0x02

//
//! If this flag is set in the ucFlags field of tConfigParameters, the module
//! channel on-off.  
//! 
//
//*****************************************************************************
#define CMD_CH1_OPEN    		 0x01
#define CMD_CH2_OPEN    		 0x02
#define CMD_CH3_OPEN    		 0x04
#define CMD_CH4_OPEN    		 0x08
#define TIME_CH1_EN   		 	 0x10
#define TIME_CH2_EN    		   0x20
#define TIME_CH3_EN    		   0x40
#define TIME_CH4_EN   		   0x80
//
// the thresh limit of the alarm parameters.
//
#define VOL_UPPER_LIMIT     30000
#define VOL_LOWER_LIMIT     15000
#define CUR_UPPER_LIMIT     1000
#define LEAK_UPPER_LIMIT    5000
//
// the sntp intervate seconds limit.
//
#define SNTP_EN_FLAG         0x04
#define SNTP_EN_UPPER_OPTION 1
#define SNTP_UPPER_INTER    60000
#define SNTP_LOWER_INTER    1
//
// the snmp intervate seconds limit.
//
#define TRAP_EN_UPPER_OPTION   1
//
// the redey intervate seconds limit.
//
#define RECNT_UPPER_LIMIT   3
#define REDEY_UPPER_LIMIT   10000
#define REDEY_LOWER_LIMIT   1
//
// the control stype defined.
//
#define SAVED_CTR_STYPE       1
#define RESET_CTR_STYPE       2
#define RESTORE_CTR_STYPE     3
#define REDEFAULT_CTR_STYPE   4
//
// the max length of the mac address.
//
#define MAX_MACSTRING_LEN    19
//*****************************************************************************
//
//! The address of the first block of flash to be used for storing parameters.
//
//*****************************************************************************
#define FLASH_PB_START          0x0003F000//0x0003F800

//*****************************************************************************
//
//! The address of the last block of flash to be used for storing parameters.
//! Since the end of flash is used for parameters, this is actually the first
//! address past the end of flash.
//
//*****************************************************************************
#define FLASH_PB_END            0x0003F800//0x00040000

//*****************************************************************************
//
//! The size of the parameter block to save.  This must be a power of 2,
//! and should be large enough to contain the tConfigParameters structure.
//
//*****************************************************************************
#define FLASH_PB_SIZE           512

//*****************************************************************************
//
//! The size of the ring buffers used for interface between the UART and
//! telnet session (RX).
//
//*****************************************************************************
#define RX_RING_BUF_SIZE        (256 * 2)

//*****************************************************************************
//
//! The size of the ring buffers used for interface between the UART and
//! telnet session (TX).
//
//*****************************************************************************
#define TX_RING_BUF_SIZE        (256 * 6)

//*****************************************************************************
//
//! Enable RFC-2217 (COM-PORT-OPTION) in the telnet server code.
//
//*****************************************************************************
#define CONFIG_RFC2217_ENABLED  1

//*****************************************************************************
//
//! The GPIO port on which the XVR_ON pin resides.
//
//*****************************************************************************
#define PIN_XVR_ON_PORT         GPIO_PORTB_BASE

//*****************************************************************************
//
//! The GPIO pin on which the XVR_ON pin resides.
//
//*****************************************************************************
#define PIN_XVR_ON_PIN          GPIO_PIN_4

//*****************************************************************************
//
//! The GPIO port on which the XVR_OFF_N pin resides.
//
//*****************************************************************************
#define PIN_XVR_OFF_N_PORT      GPIO_PORTB_BASE

//*****************************************************************************
//
//! The GPIO pin on which the XVR_OFF_N pin resides.
//
//*****************************************************************************
#define PIN_XVR_OFF_N_PIN       GPIO_PIN_5

//*****************************************************************************
//
//! The GPIO port on which the XVR_INV_N pin resides.
//
//*****************************************************************************
#define PIN_XVR_INV_N_PORT      GPIO_PORTB_BASE

//*****************************************************************************
//
//! The GPIO pin on which the XVR_INV_N pin resides.
//
//*****************************************************************************
#define PIN_XVR_INV_N_PIN       GPIO_PIN_2

//*****************************************************************************
//
//! The GPIO port on which the XVR_RDY pin resides.
//
//*****************************************************************************
#define PIN_XVR_RDY_PORT        GPIO_PORTB_BASE

//*****************************************************************************
//
//! The GPIO pin on which the XVR_RDY pin resides.
//
//*****************************************************************************
#define PIN_XVR_RDY_PIN         GPIO_PIN_3

//*****************************************************************************
//
//! The GPIO port on which the U0RX pin resides.
//
//*****************************************************************************
#define PIN_U0RX_PORT           GPIO_PORTA_BASE

//*****************************************************************************
//
//! The GPIO pin on which the U0RX pin resides.
//
//*****************************************************************************
#define PIN_U0RX_PIN            GPIO_PIN_0

//*****************************************************************************
//
//! The GPIO port on which the U0TX pin resides.
//
//*****************************************************************************
#define PIN_U0TX_PORT           GPIO_PORTA_BASE

//*****************************************************************************
//
//! The GPIO pin on which the U0TX pin resides.
//
//*****************************************************************************
#define PIN_U0TX_PIN            GPIO_PIN_1

//*****************************************************************************
//
//! The GPIO port on which the U0RTS pin resides.
//
//*****************************************************************************
#define PIN_U2RTS_PORT          GPIO_PORTC_BASE

//*****************************************************************************
//
//! The GPIO pin on which the U0RTS pin resides.
//
//*****************************************************************************
#define PIN_U2RTS_PIN           GPIO_PIN_5

//*****************************************************************************
//
//! The interrupt on which the U0RTS pin resides.
//
//*****************************************************************************
#define PIN_U2RTS_INT           INT_GPIOC

//*****************************************************************************
//
//! The GPIO port on which the U0CTS pin resides.
//
//*****************************************************************************
#define PIN_U2CTS_PORT          GPIO_PORTC_BASE

//*****************************************************************************
//
//! The GPIO pin on which the U0CTS pin resides.
//
//*****************************************************************************
#define PIN_U2CTS_PIN           GPIO_PIN_4

//*****************************************************************************
//
//! The GPIO port on which the U1RX pin resides.
//
//*****************************************************************************
#define PIN_U2RX_PORT           GPIO_PORTG_BASE

//*****************************************************************************
//
//! The GPIO pin on which the U1RX pin resides.
//
//*****************************************************************************
#define PIN_U2RX_PIN            GPIO_PIN_0

//*****************************************************************************
//
//! The GPIO port on which the U1TX pin resides.
//
//*****************************************************************************
#define PIN_U2TX_PORT           GPIO_PORTG_BASE

//*****************************************************************************
//
//! The GPIO pin on which the U1TX pin resides.
//
//*****************************************************************************
#define PIN_U2TX_PIN            GPIO_PIN_1

//*****************************************************************************
//
//! The GPIO port on which the U1RTS pin resides.
//
//*****************************************************************************
#define PIN_U1RTS_PORT          GPIO_PORTA_BASE
#define PIN_U1RD_PORT          	GPIO_PORTC_BASE

//*****************************************************************************
//
//! The GPIO pin on which the U1RTS pin resides.
//
//*****************************************************************************
#define PIN_U1RTS_PIN           GPIO_PIN_2

//*****************************************************************************
//
//! The interrupt on which the U1RTS pin resides.
//
//*****************************************************************************
#define PIN_U1RTS_INT           INT_GPIOA

//*****************************************************************************
//
//! The GPIO port on which the U1CTS pin resides.
//
//*****************************************************************************
#define PIN_U1CTS_PORT          GPIO_PORTA_BASE

//*****************************************************************************
//
//! The GPIO pin on which the U1CTS pin resides.
//
//*****************************************************************************
#define PIN_U1CTS_PIN           GPIO_PIN_3
#define PIN_U1RD_PIN      		  GPIO_PIN_4
//*****************************************************************************
//
// IP Address Acquisition Modes
//
//*****************************************************************************
#define IPADDR_USE_STATIC       0
#define IPADDR_USE_DHCP         1
#define IPADDR_USE_AUTOIP       2
//*****************************************************************************
//
//! This structure defines the time parameters used to configure the RTC and
//! telnet session associated with a single instance of a port on the S2E
//! module.
//
//*****************************************************************************
typedef struct
{
	unsigned char valid;		/*  */
	unsigned short year;		/*  */
	unsigned char month;		/*  */
	unsigned char day;			/* */
	unsigned char hour;			/*  */
	unsigned char minute;		/* */
	unsigned char second;		/*  */
	unsigned char week;			/*  */
}
RTC_T;
//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

//*****************************************************************************
//
// A flag to indicate that a firmware update has been requested.
//
//*****************************************************************************
extern tBoolean g_bStartBootloader;

//*****************************************************************************
//
// A flag to the main loop indicating that it should update the IP address
// after a short delay (to allow us to send a suitable page back to the web
// browser telling it the address has changed).
//
//*****************************************************************************
extern tBoolean g_bChangeIPAddress;

//*****************************************************************************
//
// Prototypes for the globals exported from the configuration module, along
// with public API function prototypes.
//
//*****************************************************************************
extern tConfigParameters g_sParameters;
extern const tConfigParameters *g_psDefaultParameters;
extern const tConfigParameters *const g_psFactoryParameters;
extern void ConfigInit(void);
extern void ConfigLoadFactory(void);
extern void ConfigLoad(void);
extern void ConfigSave(void);
extern void ConfigWebInit(void);
extern void ConfigUpdateIPAddress(void);
extern void wdogFeed(void);
extern void ConfigPreUpdateIPAddress(void);
extern void SystemSave(void);

#endif // __CONFIG_H__
