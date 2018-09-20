#ifndef _SPIDRIVE_H_
#define _SPIDRIVE_H_

#include "stm32f10x.h"

#define u8_spi unsigned char
#define u16_spi unsigned short int
	
#define SPI_MODE 0			//软件SPI 写 1  硬件 写0 

//硬件SPI成功，，原因：使用固件库中所提供的库函数就可以使用了~具体原因不明

/*******************************
*	端口定义区：建议以后定义的模式
*	对应一个 IO口 对应一个Port 
********************************/

#define Port_SPI_RST GPIOB
#define  PIN_SPI_RST GPIO_Pin_7


#define Port_SPI_DC GPIOB
#define  PIN_SPI_DC GPIO_Pin_9

//时钟
#define Port_SPI_SCLK GPIOB
#define  PIN_SPI_SCLK GPIO_Pin_13

//数据输入MOSI
#define Port_SPI_SDIN GPIOB
#define  PIN_SPI_SDIN GPIO_Pin_15

//数据输出MISO
#define Port_SPI_SDOUT GPIOB
#define  PIN_SPI_SDOUT GPIO_Pin_14

//片选
#define Port_SPI_CS GPIOB
#define  PIN_SPI_CS GPIO_Pin_12

//时钟 使能区（仅限 ABP2桥）
#define RCC_APB2Periph_SPIDrive RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO

//宏定义设置管脚
#define SPI_SETDC(bit) (bit == 0 ? (GPIO_ResetBits(Port_SPI_DC,PIN_SPI_DC)):(GPIO_SetBits(Port_SPI_DC,PIN_SPI_DC)))

#define SPI_SETRST(bit) (bit == 0 ? (GPIO_ResetBits(Port_SPI_RST,PIN_SPI_RST)):(GPIO_SetBits(Port_SPI_RST,PIN_SPI_RST)))

#define SPI_SETCS(bit) (bit == 0 ? (GPIO_ResetBits(Port_SPI_CS,PIN_SPI_CS)):(GPIO_SetBits(Port_SPI_CS,PIN_SPI_CS)));


#if SPI_MODE == 1

//使用软件SPI
	


#define SPI_SETSCLK(bit) (bit == 0 ? (GPIO_ResetBits(Port_SPI_SCLK,PIN_SPI_SCLK)):(GPIO_SetBits(Port_SPI_SCLK,PIN_SPI_SCLK)))

#define SPI_SETSDIN(bit) (bit == 0 ? (GPIO_ResetBits(Port_SPI_SDIN,PIN_SPI_SDIN)):(GPIO_SetBits(Port_SPI_SDIN,PIN_SPI_SDIN)))


#else

//使用硬件SPI

#define SPIPort SPI2			//使用的  SPI 端口 
u8_spi SPIDrive_RxTx8BitData(u8_spi dat);

#endif

void SPIDrive_Sent8BitData(u8_spi dat);


void SPIDrive_init(void);


#endif
