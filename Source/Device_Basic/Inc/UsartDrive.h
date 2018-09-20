/*****************************************************
*	异步串行通信头文件-c文件 解释
*	实现全封装化，修改只许要修改
*	头问价中的内容即可
*	BUG1 ：传输一开始的数据会丢包，损失一个数据
*	BUG2 ：如果要想声明两个Usart 端口那么需要做出很大修改
*	驱动编写：@author kim_小星兴
*	last edit：2017-11-29 19:05:07
******************************************************/
#ifndef _USARTDRIVE_H_
#define _USARTDRIVE_H_

#include "stm32f10x.h"
#include "_Global.h"

#define UsartPort USART1	//异步端口定义
#define UsartGPIO GPIOA		//异步端口所占用的IO口定义
#define UsartPortCLK RCC_APB2Periph_USART1	//异步端口时钟使能地址
#define UsartGPIOCLK RCC_APB2Periph_GPIOA	//IO口时钟使能地址
#define PortTx GPIO_Pin_9	//通讯Tx位
#define PortRx GPIO_Pin_10	//通讯Rx位
#define PortRx_ENABLE 1	//Rx 使能 标志位  //默认为开启
#define UsartPort_IRQn USART1_IRQn	//中断选择
#define UsartPort_PreemptionPriority 3	//抢占优先级：0最高
#define UsartPort_SubPriority 3		//响应优先级 0 最高
#define ReceiverOverFlag1 '*'		//自定义协议，规定 以 "*/" 作为 信号结尾
#define ReceiverOverFlag2 '/'
#define UsartPort_Rx_BUF_LEN 400	//最高可存储 199 个字符 （因为最后一个字符是'\0'）

#if SYSTEM_SUPPORT_OS == 1  /*!< 如果支持 UCOS 系统  */

extern OS_EVENT* Eve_mBox_UsartPort; 

#endif

#define UsartPort_IRQHandler(void) USART1_IRQHandler(void)  //封装中断函数：usart1 的中断响应函数

/*
*	自定义数据类型
*/

#define u8_usart unsigned char
#define u16_usart unsigned int
#define u32_usart u32

extern u16_usart UsartPort_Rx_STA;			//接收状态控制字 前两位 表示是否接收完毕 后14位 化为十进制表示 字符长度
extern u8_usart UsartPort_Rx_BUF [UsartPort_Rx_BUF_LEN];	//接收的缓存数组

void usartDrive_init(u32_usart bound);
/************************************
*	Usart 异步串行通讯协议 初始化
*	函数作用：将串口 USARTPort 初始化成为
*		抢占响应：3级 优先响应3级，
*		收/发 功能兼备的串口设备
*	输入；串口波特率 bound
*	输出：无
*************************************/

/*		void USART1_IRQHandler(void);		*/
/*********************************************
*	串口中断服务函数说明
*	串口终端服务函数在 startp_stm32f10x_md.h 中已经预定义
*	不能再次声明
*	函数功能：将接收到的数据存入 缓存数组 UsartPort_Rx_BUF 中
*			  并在接收的数据末尾加上'\0'
*	//函数预期 没读取一次buf 就清空 sta 的值
**********************************************/

void UsartPortSentDataStream(u8_usart *dat);
/*******************************************
*	发送数据流函数
*	函数功能；发送一串字符串
*	自动检测字符串长度，自动发送。
*	输入：字符串（u8_usart）数组 dat
*	输出：无
********************************************/

u8_usart* UsartPortGetReceiverString(void);
/*****************************************
*	发送接收到的数据流
*	函数功能：将接收到的数据发出去
*		如果接收不成功或者无值则 返回 ""
*	输入：无
*	输出：u8 字符串指针
******************************************/



#endif


