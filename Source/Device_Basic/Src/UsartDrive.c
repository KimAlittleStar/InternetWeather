/******************************************************************************************************
* �ļ���: UsartDrive.c
* �����ļ���: Device_Basic
* �ļ�����: UsartDrive.h stdio.h ���ڳ�ʼ���ļ�
* �ļ��༭��: kim_alittel_star
*******************************************************************************************************/
#include "UsartDrive.h"

#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0)
	{}//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

#if SYSTEM_SUPPORT_OS == 1   /*!< ���֧�� UCOS ϵͳ  */
OS_EVENT* Eve_mBox_UsartPort; 
#endif

u16_usart UsartPort_Rx_STA = 0;
u8_usart UsartPort_Rx_BUF [UsartPort_Rx_BUF_LEN];

void usartDrive_init(u32_usart bound)
{

	/*
	*	��Ҫ���裺
	*		������ռ�õ�IO��ʱ��ʹ��
	*		���ڵĸ��ù���ʱ��ʹ��
	*		IO��ģʽ ��� ����ģʽ ��ʼ��
	*		����н����ź�
	*			��ʼ���ж�ʱ�ӣ�
	*			ȷ���жϵȼ�
	*			ʹ���ж�
	*		ʹ�ܴ��ڹ���
	*/
	GPIO_InitTypeDef GPIO_initStructure;
	USART_InitTypeDef USART_initStructure;
	NVIC_InitTypeDef NVIC_initStructure;
	
	//ʱ��ʹ��
	RCC_APB2PeriphClockCmd(UsartPortCLK | UsartGPIOCLK,ENABLE);
	//���ڸ�λ
	USART_DeInit(UsartPort);
	//�˿�ģʽȷ��
		//���÷��Ͷ˿� Tx ��transmitter��ģʽ 
	GPIO_initStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_initStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_initStructure .GPIO_Pin = PortTx;
	GPIO_Init(UsartGPIO,&GPIO_initStructure);
		//���ý��ն˿� Rx ��receiver��ģʽ
	GPIO_initStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_initStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_initStructure .GPIO_Pin = PortRx;
	GPIO_Init(UsartGPIO,&GPIO_initStructure);
	//����ģʽȷ��	
	USART_initStructure.USART_BaudRate = bound;
	USART_initStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_initStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_initStructure.USART_Parity = USART_Parity_No;
	USART_initStructure.USART_StopBits = USART_StopBits_1;
	USART_initStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(UsartPort,&USART_initStructure);
	//����н��գ���ô��Ҫ�����жϣ���������յ��ַ�
//	#if PortRx_ENABLE
	//�жϵȼ�ȷ��
		//�жϲ�ѯƵ��Ϊ 
	NVIC_initStructure.NVIC_IRQChannel = UsartPort_IRQn;
	NVIC_initStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_initStructure.NVIC_IRQChannelPreemptionPriority = UsartPort_PreemptionPriority;
	NVIC_initStructure.NVIC_IRQChannelSubPriority = UsartPort_SubPriority;
	NVIC_Init(&NVIC_initStructure);
	UsartPort_Rx_STA = 0;
		//�����ж�	
	USART_ITConfig(UsartPort,USART_IT_RXNE,ENABLE);
//	#endif
	//����ʹ��
	USART_Cmd(UsartPort,ENABLE);
}

/*
*	���������
*		UsartPort_IRQHandler 
*	��Ϊ�����жϵķ�װ
*	���ԣ�������ɹ�����ô�Ͱ����ָ�Ϊ
*	USART1_IRQHandler
*	��װ�ɹ�
*
*/

void UsartPort_IRQHandler(void)
{
	u8_usart receiveData;
	/*
	*	�ݲ���Ӷ� OS �����֧��
	*/
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
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
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();    
#endif
	/*
	*	�ݲ���� OS �˳� ֧��
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
		//�ȴ����ݷ������ 
	}
	for(i = 0;i < stringLenth;i++)
	{
		USART_SendData(UsartPort,*(dat+i));
		while(USART_GetFlagStatus(UsartPort,USART_FLAG_TC) != SET)
		{	
			//�ȴ����ݷ������ 
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
