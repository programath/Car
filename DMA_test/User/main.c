
#include "stm32f10x.h"
#include <stdio.h>
#include "usart.h"
#include "dma.h"
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
/**
*printf的重映像
**/
extern int fputc(int ch,FILE *f)
{
	USART_SendData(USART1,ch);//ch发送给USART1
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);//wait for the end of sending
	return(ch);
}


void USART1_Config(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		
		/* config USART1 clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
		
		/* USART1 GPIO config */
		/* Configure USART1 Tx (PA.09) as alternate function push-pull */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	
		/* Configure USART1 Rx (PA.10) as input floating */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
			
		/* USART1  */
		USART_InitStructure.USART_BaudRate = 115200;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No ;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(USART1, &USART_InitStructure); 
		USART_Cmd(USART1, ENABLE);
}
extern u32 ADC_ConvertedValue[1005];


int main(void)
{
	ADC1_Init(); //ADC和DMA的同时开启  
//	USARTConfig();  //USART和DMA应用的同时开启  PA9 Tx  PA10 Rx   
	
	while(1)
	{		
		int i =  0;
		USART1_Config();	
		printf("%d\t",ADC_ConvertedValue[i]);
		i++;
//		DMA_Cmd(DMA1_Channel4,ENABLE);
	}
	  
}

/*********************************************END OF FILE**********************/

