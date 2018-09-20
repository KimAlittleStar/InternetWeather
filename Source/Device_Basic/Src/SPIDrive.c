/*********************************************
*			SPI驱动文件说明
*	此文件为 STM32f10x 硬件 SPI 或者 软件模拟SPI
*	该文件只实现 单工发送 数据
*	占用端口为 
*		RST :复位信号 低电平复位
* 		DC  :数据/指令 信号 一般 0 为指令， 1 为数据 
*		SCLK:时钟信号  产生时序信号 一般上升沿读取一位数据
*		DCIN:数据信号  低为 0  高为 1
*		CS  :片选信号  低电平有效，高电平时 DCIN数据 无效
*		 
**********************************************/


/*********************************************
*			SPI驱动文件说明
*	
*		
* 		 
*		
*		 
**********************************************/



#include "SPIDrive.h"


#if SPI_MODE == 1

//如果 SPI 模式为 软件模式

/*********************************************
*	SPI 驱动 初始化程序 
*	函数功能：初始化 RST，DC，DCLK，DCIN，CS IO 口状态
*	各IO 口模式 均为 PP——OUT 输出			  
*		 
**********************************************/
void SPIDrive_init(void)
{
	GPIO_InitTypeDef GPIO_InitConstruct;
	
	//使能时钟
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPIDrive,ENABLE);
	
	//初始化IO口
			//res IO 口
	GPIO_InitConstruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitConstruct.GPIO_Pin = PIN_SPI_RST;
	GPIO_InitConstruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(Port_SPI_RST,&GPIO_InitConstruct);
	
			//DC IO口
	GPIO_InitConstruct.GPIO_Pin = PIN_SPI_DC;
	
	GPIO_Init(Port_SPI_DC,&GPIO_InitConstruct);
	
			//clk
	GPIO_InitConstruct.GPIO_Pin = PIN_SPI_SCLK;
	
	GPIO_Init(Port_SPI_SCLK,&GPIO_InitConstruct);
	
			//SDIN 
	GPIO_InitConstruct.GPIO_Pin = PIN_SPI_SDIN;
	
	GPIO_Init(Port_SPI_SDIN,&GPIO_InitConstruct);
	
			//CS
	GPIO_InitConstruct.GPIO_Pin = PIN_SPI_CS;
	GPIO_InitConstruct.GPIO_Mode = GPIO_Mode_Out_PP;
	
	GPIO_Init(Port_SPI_CS,&GPIO_InitConstruct);

		
}



#else
//如果为硬件SPI （次只针对 STM 32f10x系列）
/*********************************************
*	SPI 驱动 初始化程序 
*	函数功能：初始化 RST，DC，DCLK，DCIN，CS IO 口状态
*	现在定义的是 SPI2 口 只有 SPI1 挂载在 ABP2 上，其他都在ABP1上
*	RST DC CS 为软件控制 PP——Out 模式
*	DCLK DCIN 为 AF——PP模式
*	如对相应管脚需要修改请修改 SPIDrive.h 文件 中 #define 部分
**********************************************/

void SPIDrive_init(void)
{
	GPIO_InitTypeDef GPIO_InitConstruct;
	SPI_InitTypeDef SPI_InitConstruct;

	//使能时钟 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_SPIDrive,ENABLE);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);		//spi2 时钟挂载在 1桥， 只有 SPI 1 在2桥，其他都在 1 桥
	
	GPIO_InitConstruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	//reset
	GPIO_InitConstruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitConstruct.GPIO_Pin = PIN_SPI_RST;
	
	GPIO_Init(Port_SPI_RST,&GPIO_InitConstruct);
	
	//DC
	GPIO_InitConstruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitConstruct.GPIO_Pin = PIN_SPI_DC;
	
	GPIO_Init(Port_SPI_DC,&GPIO_InitConstruct);
	
	//SCLK		复用推挽输出
	GPIO_InitConstruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitConstruct.GPIO_Pin = PIN_SPI_SCLK;
	
	GPIO_Init(Port_SPI_SCLK,&GPIO_InitConstruct);
	
	//SDIN
	GPIO_InitConstruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitConstruct.GPIO_Pin = PIN_SPI_SDIN;
	
	GPIO_Init(Port_SPI_SDIN,&GPIO_InitConstruct);
	
	GPIO_InitConstruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitConstruct.GPIO_Pin = PIN_SPI_SDOUT;

	GPIO_Init(Port_SPI_SDOUT,&GPIO_InitConstruct);

	//CS
	GPIO_InitConstruct.GPIO_Pin = PIN_SPI_CS;
	GPIO_InitConstruct.GPIO_Mode = GPIO_Mode_Out_PP;
	
	GPIO_Init(Port_SPI_CS,&GPIO_InitConstruct);

	
	
	
	//配置硬件 SPI
	
	SPI_InitConstruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;	//串口 速率分频，36M / 2
	SPI_InitConstruct.SPI_CPHA = SPI_CPHA_2Edge;		//存储数据 在 第二次跳变时候存储
	SPI_InitConstruct.SPI_CPOL = SPI_CPOL_High;			//空闲时候  时钟状态为 高
	SPI_InitConstruct.SPI_CRCPolynomial = 7;			//数据校验码？不肯定
	SPI_InitConstruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitConstruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitConstruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitConstruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitConstruct.SPI_NSS = SPI_NSS_Soft;			//控制选择 拿一个 SPI设备有用
	
	SPI_Init(SPIPort,&SPI_InitConstruct);		//修改响应端口
	
	SPI_Cmd(SPIPort,ENABLE);					//同上

	
}

#endif



#if SPI_MODE == 1
//软件  模拟模式下的 发送八位数据
void SPIDrive_Sent8BitData(u8_spi dat)
{

	//软件SPI
	
	u8_spi i;
	
	SPI_SETCS(0);
	for(i = 0;i < 8;i++)
	{
		SPI_SETSCLK(0);		
		if(dat & 0x80)
		{
			SPI_SETSDIN(1);
		}else
		{
			SPI_SETSDIN(0);
		}
		dat <<= 1;
		
		SPI_SETSCLK(1);

		
	}
	SPI_SETCS(1);
}
#else
//硬件 SPI 模式下的 发送 八位数据
void SPIDrive_Sent8BitData(u8_spi dat)
{
	//硬件SPI
	SPI_SETCS(0);
	
	SPI_I2S_SendData(SPIPort,dat);
						//等待语句 与发送语句的顺序具体情况请参考 硬件SPI 时序图做出比较
	
	
	while(SPI_I2S_GetFlagStatus(SPIPort,SPI_I2S_FLAG_BSY) == RESET)
	{
		//空语句等待
	}
	SPI_SETCS(1);
	
		
}

u8_spi SPIDrive_RxTx8BitData(u8_spi dat)
{
	u16_spi i = 1000;
	u8_spi ret = 0; 
	while(SPI_I2S_GetFlagStatus(SPIPort,SPI_I2S_FLAG_TXE ) == RESET && i)
	{
		i--;
	}
	if(i) i= 1000;
	else return 0;
	SPI_I2S_SendData(SPIPort,dat);
	
	
	
	while(SPI_I2S_GetFlagStatus(SPIPort,SPI_I2S_FLAG_RXNE) == RESET && i)
	{
		i--;
	}
	if(i ==0 )
	 return 0;
	ret = (u8_spi)SPI_I2S_ReceiveData(SPIPort);
	return ret;
//		u8 retry=0;				 	
//	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
//		{
//		retry++;
//		if(retry>200)return 0;
//		}			  
//	SPI_I2S_SendData(SPI2, dat); //通过外设SPIx发送一个数据
//	retry=0;

//	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) //检查指定的SPI标志位设置与否:接受缓存非空标志位
//		{
//		retry++;
//		if(retry>200)return 0;
//		}	  						    
//	return SPI_I2S_ReceiveData(SPI2); //返回通过SPIx最近接收的数据	
}


#endif




