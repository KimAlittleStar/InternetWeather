/******************************************************************************************************
* 文件名: UsartDrive.c
* 所在文件夹: Device_Basic
* 文件关联: UsartDrive.h stdio.h 串口初始化文件
* 文件编辑者: kim_alittel_star
*******************************************************************************************************/
#include "UsartDrive.h"

#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0)
	{}//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

#if SYSTEM_SUPPORT_OS == 1   /*!< 如果支持 UCOS 系统  */
OS_EVENT* Eve_mBox_UsartPort; 
#endif

u16_usart UsartPort_Rx_STA = 0;
u8_usart UsartPort_Rx_BUF [UsartPort_Rx_BUF_LEN];

void usartDrive_init(u32_usart bound)
{

	/*
	*	主要步骤：
	*		串口所占用的IO口时钟使能
	*		串口的复用功能时钟使能
	*		IO口模式 输出 输入模式 初始化
	*		如果有接收信号
	*			初始化中断时钟，
	*			确定中断等级
	*			使能中断
	*		使能串口功能
	*/
	GPIO_InitTypeDef GPIO_initStructure;
	USART_InitTypeDef USART_initStructure;
	NVIC_InitTypeDef NVIC_initStructure;
	
	//时钟使能
	RCC_APB2PeriphClockCmd(UsartPortCLK | UsartGPIOCLK,ENABLE);
	//串口复位
	USART_DeInit(UsartPort);
	//端口模式确定
		//设置发送端口 Tx （transmitter）模式 
	GPIO_initStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_initStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_initStructure .GPIO_Pin = PortTx;
	GPIO_Init(UsartGPIO,&GPIO_initStructure);
		//设置接收端口 Rx （receiver）模式
	GPIO_initStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_initStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_initStructure .GPIO_Pin = PortRx;
	GPIO_Init(UsartGPIO,&GPIO_initStructure);
	//串口模式确定	
	USART_initStructure.USART_BaudRate = bound;
	USART_initStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_initStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_initStructure.USART_Parity = USART_Parity_No;
	USART_initStructure.USART_StopBits = USART_StopBits_1;
	USART_initStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(UsartPort,&USART_initStructure);
	//如果有接收，那么就要定义中断，来处理接收的字符
//	#if PortRx_ENABLE
	//中断等级确定
		//中断查询频道为 
	NVIC_initStructure.NVIC_IRQChannel = UsartPort_IRQn;
	NVIC_initStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_initStructure.NVIC_IRQChannelPreemptionPriority = UsartPort_PreemptionPriority;
	NVIC_initStructure.NVIC_IRQChannelSubPriority = UsartPort_SubPriority;
	NVIC_Init(&NVIC_initStructure);
	UsartPort_Rx_STA = 0;
		//开启中断	
	USART_ITConfig(UsartPort,USART_IT_RXNE,ENABLE);
//	#endif
	//串口使能
	USART_Cmd(UsartPort,ENABLE);
}

/*
*	现在我想把
*		UsartPort_IRQHandler 
*	作为串口中断的封装
*	测试，如果不成功，那么就把名字改为
*	USART1_IRQHandler
*	封装成功
*
*/

void UsartPort_IRQHandler(void)
{
	u8_usart receiveData;
	/*
	*	暂不添加对 OS 进入的支持
	*/
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	if (USART_GetITStatus(UsartPort,USART_IT_RXNE) != RESET)
	{
		receiveData = USART_ReceiveData(UsartPort);
		
		if((UsartPort_Rx_STA & 0x8000) == 0)
		{
			if ((UsartPort_Rx_STA & 0x4000))
			{
				if(receiveData == ReceiverOverFlag2)
				{
					UsartPort_Rx_BUF[UsartPort_Rx_STA & 0x3fff] = '\0';
					UsartPort_Rx_STA |= 0x8000;
#if SYSTEM_SUPPORT_OS
					OSMboxPost(Eve_mBox_UsartPort,UsartPort_Rx_BUF);
#endif
					
				}
				else
				{
					UsartPort_Rx_STA = 0;
				}
			}
			else
			{
				if(receiveData == ReceiverOverFlag1)
				{
					UsartPort_Rx_STA |= 0x4000;
				}
				else
				{
					UsartPort_Rx_BUF[UsartPort_Rx_STA & 0x3fff] = receiveData;
					UsartPort_Rx_STA++;
					if(UsartPort_Rx_STA > (UsartPort_Rx_BUF_LEN-2))
					{
						UsartPort_Rx_STA = 0;
					}
				}
			}
		}
		
	}
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();    
#endif
	/*
	*	暂不添加 OS 退出 支持
	*/
}

void UsartPortSentDataStream(u8_usart *dat)
{
	u16_usart stringLenth = 0;
	u16_usart i;
	
	while(*(dat + stringLenth) != '\0')
	{
		stringLenth++;
	}
	
	USART_SendData(UsartPort,0);
	while(USART_GetFlagStatus(UsartPort,USART_FLAG_TC) != SET)
	{	
		//等待数据发送完成 
	}
	for(i = 0;i < stringLenth;i++)
	{
		USART_SendData(UsartPort,*(dat+i));
		while(USART_GetFlagStatus(UsartPort,USART_FLAG_TC) != SET)
		{	
			//等待数据发送完成 
		}
	}
}


u8_usart* UsartPortGetReceiverString(void)
{
	if(UsartPort_Rx_STA & 0x8000)
	{
		UsartPort_Rx_STA = 0;
		return UsartPort_Rx_BUF;
	}
	else
	{
		return NULL;
	}
	
}
