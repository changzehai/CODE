/*****************************************************************************/
/* 文件名:    dev_usart.c                                                 */
/* 描  述:    串口相关处理                                                   */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include <includes.h>
#include "dev.h"
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "mtfs30_debug.h"
#include "util.h"




/*-------------------------------*/
/* 变量定义                      */
/*-------------------------------*/
//u8 gUSART1_tx_buf[USART1_BUF_SIZE]; /* 串口1发送缓冲区 */
//u8 gUSART2_tx_buf[USART2_BUF_SIZE]; /* 串口2发送缓冲区 */
u8 gusart1_rx_buf[USART1_BUF_SIZE]; /* 串口1接收缓冲区 */
ringbuffer_t pgusart1_rb;                  /* 串口1消息缓冲区 */

//u8 gUSART2_rx_buf[USART2_BUF_SIZE]; /* 串口2接收缓冲区 */


OS_Q    gusart1_msg;        /* 串口1消息队列     */
OS_MEM  gusart1_mem; /* 串口1用内存控制块 */
OS_SEM  gusart1_sem;     /* 串口1用信号量     */

/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
static void dev_rcc_configuration(void);
static void dev_gpio_configuration(void);
static void dev_nvic_configuration(void);
static void dev_usart_configuration(void);
static void dev_dma_configuration(void);
static void dev_usart1_send(uint8_t *ptr, uint16_t size);
static void dev_usart2_send(uint8_t *ptr, uint16_t size);
static void dev_usart3_send(uint8_t *ptr, uint16_t size);

/*****************************************************************************
 * 函  数:    dev_rcc_configuration                                                          
 * 功  能:    配置串口时钟、DMA时钟和GPIO时钟                                                                  
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static void dev_rcc_configuration(void)
{
    

	/* GPIO时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	/* DMA2时钟 */						   
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);  						
	/* USART1时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);				
	/* USART2时钟和USART3时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 | RCC_APB1Periph_USART3, ENABLE);
}



/*****************************************************************************
 * 函  数:    dev_gpio_configuration                                                          
 * 功  能:    配置串口1、串口2、串口3的配置GPIO引脚模式                                                                  
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static void dev_gpio_configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

  
    /* GPIO初始化 */
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    /*-------------------------------*/
    /* USART1                        */
    /*-------------------------------*/
    /* 配置Tx、Rx引脚为复用功能 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
      
    /* 对应引脚复用映射 */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

}



/*****************************************************************************
 * 函  数:    dev_nvic_configuration                                                          
 * 功  能:    配置NVIC中断优先级                                                                  
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static void dev_nvic_configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;



	/* Usart1 NVIC 配置 */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}



/*****************************************************************************
 * 函  数:    dev_usart_configuration                                                          
 * 功  能:    配置串口工作参数                                                                  
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static void dev_usart_configuration(void)
{
	USART_InitTypeDef USART_InitStructure;

	/* 配置USART1参数
	   - BaudRate = 115200 baud
	   - Word Length = 8 Bits
	   - One Stop Bit
	   - No parity
	   - Hardware flow control disabled (RTS and CTS signals)
	   - Receive and transmit enabled
	 */
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

    /* 使能串口1 */
    USART_Cmd(USART1, ENABLE);   
    
    /* 使能串口1的DMA接收 */
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
    
    /* 中断配置 */
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);/*  不使能接收中断 */
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE); /*  使能IDLE中断   */


     /* 设置USART1中断服务函数 */
     BSP_IntVectSet(BSP_INT_ID_USART1, USART1_IRQHandler);
     BSP_IntEn(BSP_INT_ID_USART1);  
    

}



/*****************************************************************************
 * 函  数:    dev_dma_configuration                                                          
 * 功  能:    配置串口DMA工作参数                                                                  
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static void dev_dma_configuration(void)
{
	DMA_InitTypeDef DMA_InitStructure;



	/*-------------------------------*/
	/* USART1_Rx                     */
	/*-------------------------------*/

    /* 恢复默认配置 */
	DMA_DeInit(DMA2_Stream5);
	while (DMA_GetCmdStatus(DMA2_Stream5) != DISABLE);//等待DMA可配置 
    
	/* 配置 DMA Stream */
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;  //通道选择
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART1->DR;//DMA外设地址
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)gusart1_rx_buf;//DMA 存储器0地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;//外设到存储器模式
	DMA_InitStructure.DMA_BufferSize = USART1_BUF_SIZE;//传输数据大小 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//不使能外设地址增量模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//内存地址为增量模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据长度:8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//存储器数据长度:8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// 使用普通模式 
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//中等优先级
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;/* 禁用FIFO模式 */         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;/* FIFO大小 */
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
	DMA_Init(DMA2_Stream5, &DMA_InitStructure);//初始
    
	
	DMA_Cmd(DMA2_Stream5, ENABLE);
//    DMA_ITConfig(DMA2_Stream5, DMA_IT_TC,ENABLE);

}


/*****************************************************************************
 * 函  数:    USART1_IRQHandler                                                          
 * 功  能:    串口1接收函数                                                                  
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void USART1_IRQHandler(void)
{
	u16 num;
    u8  *pmsg = NULL;
    OS_ERR err;


	/* 产生空闲中断 */
	if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
	{
		/* 先读SR，然后读DR才能清除 */
		num = USART1->SR;		
		num = USART1->DR;	

		/* 关闭DMA,防止处理其间有数据 */
		DMA_Cmd(DMA2_Stream5, DISABLE);								 

		/* 得到真正接收数据个数 */
		num = USART1_BUF_SIZE - DMA_GetCurrDataCounter(DMA2_Stream5); 	
		gusart1_rx_buf[num] = '\0';
        

#ifdef MTFS30_DEBUG_EN
             MTFS30_DEBUG("num = %d, 卫星消息: %s\n", num, gusart1_rx_buf);
#endif        
        
        if (num != 0)
        {
            util_ringbuffer_write(&pgusart1_rb, gusart1_rx_buf, num);

        }
        
        /* 清除标志位 */
        DMA_ClearFlag(DMA2_Stream5, DMA_FLAG_TCIF5 | DMA_FLAG_FEIF5 | DMA_FLAG_DMEIF5 | DMA_FLAG_TEIF5 | DMA_FLAG_HTIF5);
		
        /* 正常传输模式需要每次都重新设置接收数据个数 */		
		DMA_SetCurrDataCounter(DMA2_Stream5, USART1_BUF_SIZE);
                
		/* 开启DMA */
		DMA_Cmd(DMA2_Stream5, ENABLE);
	}
	

}




/*****************************************************************************
 * 函  数:    dev_usart1_send                                                          
 * 功  能:    串口1发送函数                                                                  
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static void dev_usart1_send(uint8_t *ptr, uint16_t size)
{
	uint16_t i;

	/* 清除发送中断标志,防止第一个字节丢失 */
	USART_ClearFlag(USART1, USART_FLAG_TC);

	for (i = 0; i < size; i++)
	{	
		/* 发送数据 */
		USART_SendData(USART1, (uint16_t)*ptr++);
		/* 等待数据发送发送完成 */
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	}
}




/*****************************************************************************
 * 函  数:    dev_usart_init                                                          
 * 功  能:    串口初始化                                                                  
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void dev_usart_init(void)
{

    OS_ERR err;
    
	/* 配置串口时钟、DMA时钟和GPIO时钟 */
	dev_rcc_configuration();

	/* 配置串口1、串口2、串口3的配置GPIO引脚模式 */
	dev_gpio_configuration();

	/* 配置NVIC中断优先级 */
	dev_nvic_configuration();

	/* 配置串口工作参数 */
	dev_usart_configuration();

	/* 配置串口DMA工作参数 */
	dev_dma_configuration();
    
    /* 创建串口1消息缓冲 */
    util_ringbuffer_create(&pgusart1_rb, USART1_BUF_SIZE * 2);

     
	    
}









/*****************************************************************************
 * 函  数:    usart_send                                                           
 * 功  能:    按照串口数据格式发送数据                                                                  
 * 输  入:    *pdata: 要发送的数据
 *            com   : 发送串口号 
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/ 
void usart_send(u8_t *pdata, u8_t com)
{
	u8_t buf[USART_SEND_LEN_MAX];    /* 用于存放格式化成串口数据格式的数据 */
	u8_t check = 0;                  /* 校验码                             */
	u8_t len = 0;                    /* 函数输入的字符串长度               */
	u8_t *p = pdata;


	len = strlen((char *)pdata);
	/* 加上"*\r\n"之后的长度超过串口发送最大长度 */
	if ((len + 3) > USART_SEND_LEN_MAX) 
	{
#ifdef TODO
		MTFS30_ERROR("发送数据过长(len=%d)", len);
		return;
#endif		    
	}
    
#ifdef TODO
	/* 计算校验码 */
	while(*p)
	{
		check = check ^ (*p);      
		p++;
	}	
#endif
    
	/* 格式化成串口数据格式 */
    //sprintf((char *)buf, "%s*%X\r\n", pdata, check);
    sprintf((char *)buf, "%s\r\n", pdata);
    
	/* 判断发送串口号 */
	switch(com)
	{
		case USART_COM_1:    /* 串口1 */
			/* 使用串口1发送命令 */
			dev_usart1_send(buf, strlen((char*)buf));
			break;
#ifdef TODO
		case USART_COM_2:    /* 串口2 */
			/* 使用串口2发送命令 */
			dev_usart2_send(buf, strlen((char*)buf));			
			break;

		case USART_COM_3:    /* 串口3 */
			/* 使用串口3发送命令 */
			dev_usart3_send(buf, strlen((char*)buf));
			break;

#endif
		default:
#ifdef TODO
			MTFS30_ERROR("串口号(%d)有误", com);
#endif		
			break;
	}

}



