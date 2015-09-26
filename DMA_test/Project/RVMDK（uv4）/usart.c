//dma和usart一起使用
// 有问题： usart是8位的  而dma一般都是16位和32位的 数据不会丢失吗

#include "usart.h"
extern u32 ADC_ConvertedValue[1005] ;

void Dma_Init(void)  
{  
		DMA_InitTypeDef  DMA_InitStructure;  
					
	  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  
				 /* DMA channel6configuration */  
		DMA_DeInit(DMA1_Channel4);  
	  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);    
	  DMA_InitStructure.DMA_MemoryBaseAddr  =(u32)ADC_ConvertedValue;  
		DMA_InitStructure.DMA_DIR  =DMA_DIR_PeripheralDST;  //设置以外设为数据传输的目标地址
	  DMA_InitStructure.DMA_BufferSize  =  1000 ; //BufferSize;    
		DMA_InitStructure.DMA_PeripheralInc  = DMA_PeripheralInc_Disable; //设置外设地址存储不增加
	  DMA_InitStructure.DMA_MemoryInc   =DMA_MemoryInc_Enable;   //设置内存地址要增加 
	  DMA_InitStructure.DMA_PeripheralDataSize  = DMA_PeripheralDataSize_Byte; 
	  DMA_InitStructure.DMA_MemoryDataSize =DMA_PeripheralDataSize_Byte;  
		DMA_InitStructure.DMA_Mode =DMA_Mode_Circular;  
	  DMA_InitStructure.DMA_Priority  =DMA_Priority_High;  
		DMA_InitStructure.DMA_M2M  =DMA_M2M_Disable;      
		DMA_Init(DMA1_Channel4,&DMA_InitStructure);  
		 
		/* Enable DMA Channel4Transfer Complete interrupt */  
		DMA_ITConfig(DMA1_Channel4,DMA_IT_TC, ENABLE);  
	 
		USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);   
					 
		USART_Cmd(USART1,ENABLE);   		 
		 
		DMA_ITConfig(DMA1_Channel4,DMA_IT_TC, ENABLE);	//传输完成就进入dma的中断
}  




void USART1_Init(u32 baud)  
{  
   GPIO_InitTypeDef       GPIO_InitStructure;  
   USART_InitTypeDef      USART_InitStructure;  
   USART_ClockInitTypeDef USART_ClockInitStructure;  
        
   
        
       //开启时钟
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1 , (FunctionalState)ENABLE);  
        
       // A9 USART1_Tx  
   GPIO_InitStructure.GPIO_Pin   =GPIO_Pin_9;  
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
   GPIO_InitStructure.GPIO_Mode  =GPIO_Mode_AF_PP;  
   GPIO_Init(GPIOA,&GPIO_InitStructure);  
   
    // A10 USART1_Rx  
   GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;  
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
   GPIO_Init(GPIOA,&GPIO_InitStructure);  
        
   USART_InitStructure.USART_BaudRate    = baud;  
   USART_InitStructure.USART_WordLength    = USART_WordLength_8b;  
   USART_InitStructure.USART_StopBits     = USART_StopBits_1;  
   USART_InitStructure.USART_Parity       = USART_Parity_No;  
   USART_InitStructure.USART_HardwareFlowControl= USART_HardwareFlowControl_None;  
   USART_InitStructure.USART_Mode= USART_Mode_Rx | USART_Mode_Tx;  
        
   USART_ClockInitStructure.USART_Clock   = USART_Clock_Disable;  
   USART_ClockInitStructure.USART_CPOL    = USART_CPOL_Low;  
   USART_ClockInitStructure.USART_CPHA    = USART_CPHA_2Edge;  
   USART_ClockInitStructure.USART_LastBit= USART_LastBit_Disable;  
   USART_ClockInit(USART1,&USART_ClockInitStructure);  
        
   USART_Init(USART1,&USART_InitStructure);  
   USART_Cmd(USART1,(FunctionalState)ENABLE);     
      
   // USART_ITConfig(USART1,USART_IT_RXNE,(FunctionalState)ENABLE);  
}  

void NVIC_Config(void)  
{  
         NVIC_InitTypeDef NVIC_InitStructure;    
          
         //DMA          
         NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);  
         NVIC_InitStructure.NVIC_IRQChannel= DMA1_Channel4_IRQn;   
         NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 2;   
         NVIC_InitStructure.NVIC_IRQChannelSubPriority= 1;   
         NVIC_InitStructure.NVIC_IRQChannelCmd= ENABLE;   
         NVIC_Init(&NVIC_InitStructure);              
                 
}  



void USARTConfig()
{
		USART1_Init(115200);
		Dma_Init();
		
		NVIC_Config();
	
}



   

