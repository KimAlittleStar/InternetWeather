#ifndef _SPIDRIVE_H_
#define _SPIDRIVE_H_

#include "stm32f10x.h"

#define u8_spi unsigned char
#define u16_spi unsigned short int
	
#define SPI_MODE 0			//���SPI д 1  Ӳ�� д0 

//Ӳ��SPI�ɹ�����ԭ��ʹ�ù̼��������ṩ�Ŀ⺯���Ϳ���ʹ����~����ԭ����

/*******************************
*	�˿ڶ������������Ժ����ģʽ
*	��Ӧһ�� IO�� ��Ӧһ��Port 
********************************/

#define Port_SPI_RST GPIOB
#define  PIN_SPI_RST GPIO_Pin_7


#define Port_SPI_DC GPIOB
#define  PIN_SPI_DC GPIO_Pin_9

//ʱ��
#define Port_SPI_SCLK GPIOB
#define  PIN_SPI_SCLK GPIO_Pin_13

//��������MOSI
#define Port_SPI_SDIN GPIOB
#define  PIN_SPI_SDIN GPIO_Pin_15

//�������MISO
#define Port_SPI_SDOUT GPIOB
#define  PIN_SPI_SDOUT GPIO_Pin_14

//Ƭѡ
#define Port_SPI_CS GPIOB
#define  PIN_SPI_CS GPIO_Pin_12

//ʱ�� ʹ���������� ABP2�ţ�
#define RCC_APB2Periph_SPIDrive RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO

//�궨�����ùܽ�
#define SPI_SETDC(bit) (bit == 0 ? (GPIO_ResetBits(Port_SPI_DC,PIN_SPI_DC)):(GPIO_SetBits(Port_SPI_DC,PIN_SPI_DC)))

#define SPI_SETRST(bit) (bit == 0 ? (GPIO_ResetBits(Port_SPI_RST,PIN_SPI_RST)):(GPIO_SetBits(Port_SPI_RST,PIN_SPI_RST)))

#define SPI_SETCS(bit) (bit == 0 ? (GPIO_ResetBits(Port_SPI_CS,PIN_SPI_CS)):(GPIO_SetBits(Port_SPI_CS,PIN_SPI_CS)));


#if SPI_MODE == 1

//ʹ�����SPI
	


#define SPI_SETSCLK(bit) (bit == 0 ? (GPIO_ResetBits(Port_SPI_SCLK,PIN_SPI_SCLK)):(GPIO_SetBits(Port_SPI_SCLK,PIN_SPI_SCLK)))

#define SPI_SETSDIN(bit) (bit == 0 ? (GPIO_ResetBits(Port_SPI_SDIN,PIN_SPI_SDIN)):(GPIO_SetBits(Port_SPI_SDIN,PIN_SPI_SDIN)))


#else

//ʹ��Ӳ��SPI

#define SPIPort SPI2			//ʹ�õ�  SPI �˿� 
u8_spi SPIDrive_RxTx8BitData(u8_spi dat);

#endif

void SPIDrive_Sent8BitData(u8_spi dat);


void SPIDrive_init(void);


#endif
