#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
#include "usart3.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//��ʱ�� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/6/16
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
#if SYSTEM_SUPPORT_OS
extern OS_EVENT* Eve_mBox_WIFIBUFF;

#endif

void TIM7_Int_Init(u16 arr,u16 psc);
#endif
