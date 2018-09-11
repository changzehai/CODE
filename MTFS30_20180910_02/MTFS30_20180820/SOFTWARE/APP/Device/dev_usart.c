/*****************************************************************************/
/* �ļ���:    dev_usart.c                                                 */
/* ��  ��:    ������ش���                                                   */
/* ��  ��:    2018-07-20 changzehai(DTT)                                     */
/* ��  ��:    ��                                                             */
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
/* ��������                      */
/*-------------------------------*/
//u8 gUSART1_tx_buf[USART1_BUF_SIZE]; /* ����1���ͻ����� */
//u8 gUSART2_tx_buf[USART2_BUF_SIZE]; /* ����2���ͻ����� */
u8 gusart1_rx_buf[USART1_BUF_SIZE]; /* ����1���ջ����� */
ringbuffer_t pgusart1_rb;                  /* ����1��Ϣ������ */

//u8 gUSART2_rx_buf[USART2_BUF_SIZE]; /* ����2���ջ����� */


OS_Q    gusart1_msg;        /* ����1��Ϣ����     */
OS_MEM  gusart1_mem; /* ����1���ڴ���ƿ� */
OS_SEM  gusart1_sem;     /* ����1���ź���     */

/*-------------------------------*/
/* ��������                      */
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
 * ��  ��:    dev_rcc_configuration                                                          
 * ��  ��:    ���ô���ʱ�ӡ�DMAʱ�Ӻ�GPIOʱ��                                                                  
 * ��  ��:    ��                          
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/
static void dev_rcc_configuration(void)
{
    

	/* GPIOʱ�� */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	/* DMA2ʱ�� */						   
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);  						
	/* USART1ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);				
	/* USART2ʱ�Ӻ�USART3ʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 | RCC_APB1Periph_USART3, ENABLE);
}



/*****************************************************************************
 * ��  ��:    dev_gpio_configuration                                                          
 * ��  ��:    ���ô���1������2������3������GPIO����ģʽ                                                                  
 * ��  ��:    ��                          
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/
static void dev_gpio_configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

  
    /* GPIO��ʼ�� */
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    /*-------------------------------*/
    /* USART1                        */
    /*-------------------------------*/
    /* ����Tx��Rx����Ϊ���ù��� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
      
    /* ��Ӧ���Ÿ���ӳ�� */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

}



/*****************************************************************************
 * ��  ��:    dev_nvic_configuration                                                          
 * ��  ��:    ����NVIC�ж����ȼ�                                                                  
 * ��  ��:    ��                          
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/
static void dev_nvic_configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;



	/* Usart1 NVIC ���� */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}



/*****************************************************************************
 * ��  ��:    dev_usart_configuration                                                          
 * ��  ��:    ���ô��ڹ�������                                                                  
 * ��  ��:    ��                          
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/
static void dev_usart_configuration(void)
{
	USART_InitTypeDef USART_InitStructure;

	/* ����USART1����
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

    /* ʹ�ܴ���1 */
    USART_Cmd(USART1, ENABLE);   
    
    /* ʹ�ܴ���1��DMA���� */
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
    
    /* �ж����� */
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);/*  ��ʹ�ܽ����ж� */
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE); /*  ʹ��IDLE�ж�   */


     /* ����USART1�жϷ����� */
     BSP_IntVectSet(BSP_INT_ID_USART1, USART1_IRQHandler);
     BSP_IntEn(BSP_INT_ID_USART1);  
    

}



/*****************************************************************************
 * ��  ��:    dev_dma_configuration                                                          
 * ��  ��:    ���ô���DMA��������                                                                  
 * ��  ��:    ��                          
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/
static void dev_dma_configuration(void)
{
	DMA_InitTypeDef DMA_InitStructure;



	/*-------------------------------*/
	/* USART1_Rx                     */
	/*-------------------------------*/

    /* �ָ�Ĭ������ */
	DMA_DeInit(DMA2_Stream5);
	while (DMA_GetCmdStatus(DMA2_Stream5) != DISABLE);//�ȴ�DMA������ 
    
	/* ���� DMA Stream */
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;  //ͨ��ѡ��
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART1->DR;//DMA�����ַ
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)gusart1_rx_buf;//DMA �洢��0��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;//���赽�洢��ģʽ
	DMA_InitStructure.DMA_BufferSize = USART1_BUF_SIZE;//�������ݴ�С 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//��ʹ�������ַ����ģʽ
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�ڴ��ַΪ����ģʽ
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//�������ݳ���:8λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//�洢�����ݳ���:8λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// ʹ����ͨģʽ 
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//�е����ȼ�
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;/* ����FIFOģʽ */         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;/* FIFO��С */
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//�洢��ͻ�����δ���
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//����ͻ�����δ���
	DMA_Init(DMA2_Stream5, &DMA_InitStructure);//��ʼ
    
	
	DMA_Cmd(DMA2_Stream5, ENABLE);
//    DMA_ITConfig(DMA2_Stream5, DMA_IT_TC,ENABLE);

}


/*****************************************************************************
 * ��  ��:    USART1_IRQHandler                                                          
 * ��  ��:    ����1���պ���                                                                  
 * ��  ��:    ��                          
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/
void USART1_IRQHandler(void)
{
	u16 num;
    u8  *pmsg = NULL;
    OS_ERR err;


	/* ���������ж� */
	if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
	{
		/* �ȶ�SR��Ȼ���DR������� */
		num = USART1->SR;		
		num = USART1->DR;	

		/* �ر�DMA,��ֹ������������� */
		DMA_Cmd(DMA2_Stream5, DISABLE);								 

		/* �õ������������ݸ��� */
		num = USART1_BUF_SIZE - DMA_GetCurrDataCounter(DMA2_Stream5); 	
		gusart1_rx_buf[num] = '\0';
        

#ifdef MTFS30_DEBUG_EN
             MTFS30_DEBUG("num = %d, ������Ϣ: %s\n", num, gusart1_rx_buf);
#endif        
        
        if (num != 0)
        {
            util_ringbuffer_write(&pgusart1_rb, gusart1_rx_buf, num);

        }
        
        /* �����־λ */
        DMA_ClearFlag(DMA2_Stream5, DMA_FLAG_TCIF5 | DMA_FLAG_FEIF5 | DMA_FLAG_DMEIF5 | DMA_FLAG_TEIF5 | DMA_FLAG_HTIF5);
		
        /* ��������ģʽ��Ҫÿ�ζ��������ý������ݸ��� */		
		DMA_SetCurrDataCounter(DMA2_Stream5, USART1_BUF_SIZE);
                
		/* ����DMA */
		DMA_Cmd(DMA2_Stream5, ENABLE);
	}
	

}




/*****************************************************************************
 * ��  ��:    dev_usart1_send                                                          
 * ��  ��:    ����1���ͺ���                                                                  
 * ��  ��:    ��                          
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/
static void dev_usart1_send(uint8_t *ptr, uint16_t size)
{
	uint16_t i;

	/* ��������жϱ�־,��ֹ��һ���ֽڶ�ʧ */
	USART_ClearFlag(USART1, USART_FLAG_TC);

	for (i = 0; i < size; i++)
	{	
		/* �������� */
		USART_SendData(USART1, (uint16_t)*ptr++);
		/* �ȴ����ݷ��ͷ������ */
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	}
}




/*****************************************************************************
 * ��  ��:    dev_usart_init                                                          
 * ��  ��:    ���ڳ�ʼ��                                                                  
 * ��  ��:    ��                          
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/
void dev_usart_init(void)
{

    OS_ERR err;
    
	/* ���ô���ʱ�ӡ�DMAʱ�Ӻ�GPIOʱ�� */
	dev_rcc_configuration();

	/* ���ô���1������2������3������GPIO����ģʽ */
	dev_gpio_configuration();

	/* ����NVIC�ж����ȼ� */
	dev_nvic_configuration();

	/* ���ô��ڹ������� */
	dev_usart_configuration();

	/* ���ô���DMA�������� */
	dev_dma_configuration();
    
    /* ��������1��Ϣ���� */
    util_ringbuffer_create(&pgusart1_rb, USART1_BUF_SIZE * 2);

     
	    
}









/*****************************************************************************
 * ��  ��:    usart_send                                                           
 * ��  ��:    ���մ������ݸ�ʽ��������                                                                  
 * ��  ��:    *pdata: Ҫ���͵�����
 *            com   : ���ʹ��ں� 
 * ��  ��:    ��                                                    
 * ����ֵ:    ��                                                    
 * ��  ��:    2018-07-18 changzehai(DTT)                            
 * ��  ��:    ��                                       
 ****************************************************************************/ 
void usart_send(u8_t *pdata, u8_t com)
{
	u8_t buf[USART_SEND_LEN_MAX];    /* ���ڴ�Ÿ�ʽ���ɴ������ݸ�ʽ������ */
	u8_t check = 0;                  /* У����                             */
	u8_t len = 0;                    /* ����������ַ�������               */
	u8_t *p = pdata;


	len = strlen((char *)pdata);
	/* ����"*\r\n"֮��ĳ��ȳ������ڷ�����󳤶� */
	if ((len + 3) > USART_SEND_LEN_MAX) 
	{
#ifdef TODO
		MTFS30_ERROR("�������ݹ���(len=%d)", len);
		return;
#endif		    
	}
    
#ifdef TODO
	/* ����У���� */
	while(*p)
	{
		check = check ^ (*p);      
		p++;
	}	
#endif
    
	/* ��ʽ���ɴ������ݸ�ʽ */
    //sprintf((char *)buf, "%s*%X\r\n", pdata, check);
    sprintf((char *)buf, "%s\r\n", pdata);
    
	/* �жϷ��ʹ��ں� */
	switch(com)
	{
		case USART_COM_1:    /* ����1 */
			/* ʹ�ô���1�������� */
			dev_usart1_send(buf, strlen((char*)buf));
			break;
#ifdef TODO
		case USART_COM_2:    /* ����2 */
			/* ʹ�ô���2�������� */
			dev_usart2_send(buf, strlen((char*)buf));			
			break;

		case USART_COM_3:    /* ����3 */
			/* ʹ�ô���3�������� */
			dev_usart3_send(buf, strlen((char*)buf));
			break;

#endif
		default:
#ifdef TODO
			MTFS30_ERROR("���ں�(%d)����", com);
#endif		
			break;
	}

}



