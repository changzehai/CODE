#ifndef ATT7053_HEADER
#define ATT7053_HEADER

#ifdef   ATT7053_GLOBALS
#define ATT7053_EXT
#else
#define  ATT7053_EXT  extern
#endif

//ATT7053
#define Rms_I1_R		0X06
#define Rms_I2_R		0X07
#define Rms_U_R			0X08
#define Power_P_R		0X0A
#define Power_Q_R		0X0B
#define Freq_U			0X09
#define Power_S_R		0X0C
#define Energy_P_R		0X0D
#define Energy_Q_R		0X0E
#define Energy_S_R		0X0F	
#define PowerP1_R		0X10
#define	PowerQ1_R		0X11
#define BackupData      0x16	
#define SUMChecksum_R 	0X18
#define EMUSR_R			0X19	

//**˜Ž¡‰ŽŽÂŠä“õ////////////////////////////////////////////////////
#define EMUIE 			0x30		// 00 1Byte  EMU›ŽÎ‹ŽÍ•Ž·’ŽÚŽŽÂŠä“?
#define EUMIF 			0X31		// 00 1Byte  EMU›ŽÎ‹ŽÍ‰Š›Ž¼ŽŽÂŠä“õ
#define WPREG_W 		0XB2      // 00 1Byte  ˜Ž²‰BŽCŽŽÂŠä“?

#define Wenable     	0xB2    //**0xBC Ž’›~0XA6 */
#define Wdisable    	0xB2	//**0xBD Ž’›~0xA7*/

//#define IA				0xEA	/*0x5A*/
//#define IB				0xEA	/*0xA5*/


#define EMUCFG_W 		0XC0	//** EMU “„›ŽÁŽŽÂŠä“õ
#define FreqCFG_W 		0XC1	
#define ModuleEn_W 		0xC2	
#define	ANAEN_W			0XC3
#define	STATUSCFG_W		0XC4
#define IOCFG_W       	0XC5	//** ›‚˜Ž§•Ž·šbWPREG==0XBC 

#define	GP1_W			0XD0
#define	GQ1_W			0XD1
#define	GS1_W			0XD2
#define	Phasel_W		0xD3
#define	GP2_W			0XD4
#define	GQ2_W			0XD5
#define GS2_W			0xD6
#define Phase2_W		0xD7
#define QPhsCal_W		0xD8
#define	ADCCON_W		0XD9
#define	AllGain_W		0xDa
#define	I2Gain_W		0xDb
#define	I1Off_W			0xDc
#define	I2Off_W			0xDd
#define	UOff_W			0xDe
#define	PQStart_W		0xDf
#define	RMSStart_W		0xE0
#define	HFconst_W		0xE1
#define	ICHK_W			0xE2	
#define	ITAMP_W			0XE3
#define	UCONST_W		0XE4
#define	P1OFFSET_W		0XE5	
#define	P2OFFSET_W		0XE6
#define	Q1OFFSET_W		0XE7
#define	Q2OFFSET_W		0XE8
#define	I1RMSOFFSET_W 	0XE9
#define	I2RMSOFFSET_W	0XEA
#define	URMSOFFSET_W	0XEB
#define	ZCrossCurrent_W 0xEc
#define GPhs1_W		  	0xEd
#define GPhs2_W		  	0xEe	//** ›‚˜Ž§•Ž·šbWPREG==0XA6

#define	PFCnt_W			0xEF
#define QFCnt_W			0xF0
#define SFCnt_W			0xF1

//**ŒŽ²—Ž¹›Ž?/////////////
#define DF_IOCFG  			0x0024
#define DF_QPhsCal			0xFF00
#define DF_RMSStart			0x0040
#define DF_ICHK				0X0010
#define DF_ITAMP			0X0020
#define DF_ZcrossCurrent	        0x0004


#define IA				0x5A	/*0x5A*/
#define IB				0xA5	/*0xA5*/

#define ATT7053_CS_H         {GPIOC->BSRRL  = GPIO_Pin_7;}//GPIOD->BSRRH  = GPIO_PIN_0GPIO_PIN_10
#define ATT7053_CS_L         {GPIOC->BSRRH  = GPIO_Pin_7;}
#define ATT7053_DIN_H        {GPIOC->BSRRL = GPIO_Pin_12;}
#define ATT7053_DIN_L        {GPIOC->BSRRH = GPIO_Pin_12;}
#define ATT7053_SCLK_H       {GPIOC->BSRRL = GPIO_Pin_10;}
#define ATT7053_SCLK_L       {GPIOC->BSRRH = GPIO_Pin_10;}  
#define ATT7053_DOUT         (GPIOC->IDR & GPIO_Pin_11)

#define BIT24_VAL             0xFFFFFF
#define ATT7053_ZEROPOINT     0x800000
#define ATT7053_HRMS          0x7FFFFF
#define ATT7053_K_Urms        60
#define ATT7053_K_I1rms_Den   2
#define ATT7053_K_I1rms_Mol   3
#define ATT7053_K_I2rms       35964

uint32_t SPI_Read(uint8_t address);
void SPI_Write(uint8_t address, uint32_t write_data);
void ATT7053_RESET(void);

#endif


