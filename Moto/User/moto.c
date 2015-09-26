#include "moto.h"
#define INPort GPIOB
#define IN1 GPIO_Pin_10     
#define IN2 GPIO_Pin_2     //左边发电机用 1 2 
#define IN3 GPIO_Pin_3
#define IN4 GPIO_Pin_4
/* PWM信号电平跳变值  用于调控速度*/  
#define CCR1 60000*LeftD          
#define CCR2 60000*RightD 

//设置系统时钟为72MHz
void RCC_Configuration(void)
{
	RCC_DeInit();
	RCC_HSEConfig(RCC_HSE_ON);	//使能外部高速时钟
	while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET) ; //等待外部的高速时钟就绪

	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);      //使能指令预存取
	FLASH_SetLatency(FLASH_Latency_2);        

	RCC_HCLKConfig(RCC_SYSCLK_Div1); //设置AHB时钟为系统时钟
	RCC_PCLK2Config(RCC_HCLK_Div1);	//设置APB2的时钟为AHB时钟
	RCC_PCLK1Config(RCC_HCLK_Div1);	//设置APB1的时钟为AHB时钟
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9);	//设置PLL时钟为外部告诉时钟的9倍频
	RCC_PLLCmd(ENABLE);

	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); //配置PLL时钟为系统时钟
	while(RCC_GetSYSCLKSource()!=0x08); 

}

void GPIO_Config(void)  
{ 
  GPIO_InitTypeDef GPIO_InitStructure; 
 
  /* GPIOA and GPIOB clock enable */ 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);  
 
  /*GPIOA Configuration: TIM3 channel 1 and 2 as alternate function push-pull */ 
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;           // 复用推挽输出 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOB, &GPIO_InitStructure); 


  /* 配置L298N芯片IN对应的IO口的初始化*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); 
  GPIO_InitStructure.GPIO_Pin =  IN1 | IN2 | IN3 | IN4; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;           // 推挽输出 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOB, &GPIO_InitStructure); 

} 



float LeftD = 0.6;
float RightD = 0.6;

	/* PWM信号电平跳变值  用于调控速度*/  
    // u16 CCR1= 60000*LeftD;          
    // u16 CCR2= 60000*RightD; 



    /**************************************************************** 
     * 函数名：void TIM3_Config(void)  
     * 描述  ：配置TIM3输出的PWM信号的模式 
     *      CH1:输出 T=2.5ms(f=1/2.5ms=400Hz)  D=0.6的PWM波(高电平在前，低电平在后) 
     *      CH2:输出 T=2.5ms(f=1/2.5ms=400Hz)  D=0.4的PWM波(高电平在后，低电平在前) 
     *      步骤一：通过T和TIMxCLK的时钟源确定TIM_Period和TIM_Prescaler  
     *          T=(TIM_Period+1)*(TIM_Prescaler+1)/TIMxCLK=2.5ms  
     *          因为 TIM_Period<65535，所以 TIM_Prescaler>1,即 TIM_Prescaler=2 
     *          所以 TIM_Period=59999=0xEA5F 
     *      步骤二：根据TIM_Period的值，高低电平的先后D，确定CCR和TIM_OCPolarity 
     *          CH1：因为D=0.6，先高后低； 
     *              所以CCR1=(TIM_Period+1)* D=36000;TIM_OCPolarity=TIM_OCPolarity_High 
     *          CH2：因为D=0.4，先高后低； 
     *              所以CCR1=(TIM_Period+1)* (1-D)=36000;TIM_OCPolarity=TIM_OCPolarity_Low 
     *      步骤三：基础寄存器初始化 
     *      步骤四：通道寄存器初始化 
     *      步骤五：使能TIM3重载寄存器ARR 
     *      步骤六：使能TIM3  
     * 输入  ：无  
     * 输出  ：无  
     *
     ***************************************************************/  
    void TIM3_Config(void)  
    {  
        TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;  
        TIM_OCInitTypeDef  TIM_OCInitStructure;  
         
        /*PCLK1经过2倍频后作为TIM3的时钟源等于72MHz*/  
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  
        /* Time base configuration */                                            
        TIM_TimeBaseStructure.TIM_Period =0xEA5F;                                   //设置预装载的值
        TIM_TimeBaseStructure.TIM_Prescaler = 2;                                    //设置预分频：预分频=2，即为72/3=24MHz  
        TIM_TimeBaseStructure.TIM_ClockDivision = 0;                                //设置时钟分频系数：不分频  
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;                 //向上计数溢出模式  
        TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);  
        /* PWM1 Mode configuration: Channel1 */  
        TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                           //配置为PWM模式1  
        TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   
				//TIM_OCInitStructure.TIM_Channel = TIM_Channel_1;			
        TIM_OCInitStructure.TIM_Pulse = CCR1;                                       //设置跳变值，当计数器计数到这个值时，电平发生跳变  
        TIM_OCInitStructure.TIM_OCPolarity =TIM_OCPolarity_High;                    //当定时器计数值小于CCR1时为高电平  
        TIM_OC3Init(TIM3, &TIM_OCInitStructure);                                    //使能通道1      
        TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);  
        /* PWM1 Mode configuration: Channel2 */  
        TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  
        TIM_OCInitStructure.TIM_Pulse = CCR2;                                       //设置通道2的电平跳变值，输出另外一个占空比的PWM  
        TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;                   //当定时器计数值小于CCR2时为低电平 
        TIM_OC4Init(TIM3, &TIM_OCInitStructure);                                    //使能通道2  
        TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);  
        TIM_ARRPreloadConfig(TIM3, ENABLE);                                         //使能TIM3重载寄存器ARR  
        /* TIM3 enable counter */  
        TIM_Cmd(TIM3, ENABLE); 										                //使能TIM3   
    } 



void MotoInit(void)
{
	RCC_Configuration();
	GPIO_Config();
	TIM3_Config();
}


void LeftStop(void)
{
	GPIO_SetBits(INPort, IN1 | IN2);
}


void LeftBack(void)
{
	GPIO_SetBits(INPort, IN2);
	GPIO_ResetBits(INPort, IN1);
}


void LeftForward(void)
{
	GPIO_SetBits(INPort, IN1);
	GPIO_ResetBits(INPort, IN2);
}

void RightStop(void)
{
	GPIO_SetBits(INPort, IN3 | IN4);
}

void RightBack(void)
{
	GPIO_SetBits(INPort, IN4);
	GPIO_ResetBits(INPort, IN3);
}

void RightForward(void)
{
	GPIO_SetBits(INPort, IN3);
	GPIO_ResetBits(INPort, IN4);
}


void RightSetSpeed(float D)
{
		LeftD = D;                        
    TIM_SetCompare1(TIM3, CCR1);
}

void LeftSetSpeed(float D)
{
		LeftD = D;                        
    TIM_SetCompare1(TIM3, CCR1);
}









