/*****************************************************
*	�첽����ͨ��ͷ�ļ�-c�ļ� ����
*	ʵ��ȫ��װ�����޸�ֻ��Ҫ�޸�
*	ͷ�ʼ��е����ݼ���
*	BUG1 ������һ��ʼ�����ݻᶪ������ʧһ������
*	BUG2 �����Ҫ����������Usart �˿���ô��Ҫ�����ܴ��޸�
*	������д��@author kim_С����
*	last edit��2017-11-29 19:05:07
******************************************************/
#ifndef _USARTDRIVE_H_
#define _USARTDRIVE_H_

#include "stm32f10x.h"
#include "_Global.h"

#define UsartPort USART1	//�첽�˿ڶ���
#define UsartGPIO GPIOA		//�첽�˿���ռ�õ�IO�ڶ���
#define UsartPortCLK RCC_APB2Periph_USART1	//�첽�˿�ʱ��ʹ�ܵ�ַ
#define UsartGPIOCLK RCC_APB2Periph_GPIOA	//IO��ʱ��ʹ�ܵ�ַ
#define PortTx GPIO_Pin_9	//ͨѶTxλ
#define PortRx GPIO_Pin_10	//ͨѶRxλ
#define PortRx_ENABLE 1	//Rx ʹ�� ��־λ  //Ĭ��Ϊ����
#define UsartPort_IRQn USART1_IRQn	//�ж�ѡ��
#define UsartPort_PreemptionPriority 3	//��ռ���ȼ���0���
#define UsartPort_SubPriority 3		//��Ӧ���ȼ� 0 ���
#define ReceiverOverFlag1 '*'		//�Զ���Э�飬�涨 �� "*/" ��Ϊ �źŽ�β
#define ReceiverOverFlag2 '/'
#define UsartPort_Rx_BUF_LEN 400	//��߿ɴ洢 199 ���ַ� ����Ϊ���һ���ַ���'\0'��

#if SYSTEM_SUPPORT_OS == 1  /*!< ���֧�� UCOS ϵͳ  */

extern OS_EVENT* Eve_mBox_UsartPort; 

#endif

#define UsartPort_IRQHandler(void) USART1_IRQHandler(void)  //��װ�жϺ�����usart1 ���ж���Ӧ����

/*
*	�Զ�����������
*/

#define u8_usart unsigned char
#define u16_usart unsigned int
#define u32_usart u32

extern u16_usart UsartPort_Rx_STA;			//����״̬������ ǰ��λ ��ʾ�Ƿ������� ��14λ ��Ϊʮ���Ʊ�ʾ �ַ�����
extern u8_usart UsartPort_Rx_BUF [UsartPort_Rx_BUF_LEN];	//���յĻ�������

void usartDrive_init(u32_usart bound);
/************************************
*	Usart �첽����ͨѶЭ�� ��ʼ��
*	�������ã������� USARTPort ��ʼ����Ϊ
*		��ռ��Ӧ��3�� ������Ӧ3����
*		��/�� ���ܼ汸�Ĵ����豸
*	���룻���ڲ����� bound
*	�������
*************************************/

/*		void USART1_IRQHandler(void);		*/
/*********************************************
*	�����жϷ�����˵��
*	�����ն˷������� startp_stm32f10x_md.h ���Ѿ�Ԥ����
*	�����ٴ�����
*	�������ܣ������յ������ݴ��� �������� UsartPort_Rx_BUF ��
*			  ���ڽ��յ�����ĩβ����'\0'
*	//����Ԥ�� û��ȡһ��buf ����� sta ��ֵ
**********************************************/

void UsartPortSentDataStream(u8_usart *dat);
/*******************************************
*	��������������
*	�������ܣ�����һ���ַ���
*	�Զ�����ַ������ȣ��Զ����͡�
*	���룺�ַ�����u8_usart������ dat
*	�������
********************************************/

u8_usart* UsartPortGetReceiverString(void);
/*****************************************
*	���ͽ��յ���������
*	�������ܣ������յ������ݷ���ȥ
*		������ղ��ɹ�������ֵ�� ���� ""
*	���룺��
*	�����u8 �ַ���ָ��
******************************************/



#endif


