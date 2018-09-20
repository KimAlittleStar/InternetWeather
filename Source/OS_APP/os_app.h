#ifndef _OS_APP_H_
#define _OS_APP_H_

#include "include.h"

#ifdef _OS_APP_EXT_
#define OS_APP_EXT 

#else
#define OS_APP_EXT extern

#endif

/*!< -------------------------宏定义 消息 缓冲区--------------------------------------- */

typedef struct 
{
	u16 startx;
	u16 starty;
	u8 charsize;
	u8 *text;
	u16 length;
	u8 line;
} PRINTDEV;

typedef struct _DAILY_
{

	struct _DAILY_* next;
	u8 wind_speed;
	u8 wind_scale;
	u8 month;
	u8 day;

	int8_t temprature_high;
	int8_t temprature_low;
	
	u8 weather_day[10];	
	u8 weather_night[10];
	u8 wind_dir[6];

	
} DAILY_DEV;

typedef struct _WEATHER_
{
	DAILY_DEV* daily; /*!< 某个地点 的 天气预报 */
	u8 updata_hour;	/*!< 更新时间 */
	u8 updata_min;
	u8 updata_day;	
	u8 location[15];	/*!< 地点 */
	
	
} WEATHER_DEV;

//OS_APP_EXT u8 msg_buff[300];

OS_APP_EXT OS_EVENT* Eve_mBox_key;

OS_APP_EXT OS_EVENT* Eve_mBox_uart;

OS_APP_EXT OS_EVENT* Eve_mBox_wifi;

OS_APP_EXT OS_EVENT* Eve_mBox_lcd;

OS_APP_EXT OS_EVENT* Eve_mBox_Weather;

OS_APP_EXT OS_EVENT* Eve_mBox_wifiRec;

OS_APP_EXT OS_EVENT* Eve_mBox_UT;

OS_APP_EXT OS_EVENT* Eve_Q_UT;

OS_APP_EXT OS_EVENT* Eve_Q_CPU;

OS_APP_EXT u8* Q_UT_BUFF[25];

OS_APP_EXT u8* Q_CPUusage_BUFF[20];

OS_APP_EXT OS_TMR* tmr1;

OS_APP_EXT WEATHER_DEV Weather_dev;













/* !< -------------------------宏定义任务区  定义格式--------------------------------   */
/* !< 

void TASK_NAME(void *p_arg);													//the task function name
#define TASK_NAME_STK_SIZE 64													//the task stake size 
#define TASK_NAME_PRIO 251														//the task priority 1~253  @version :u2.51 or later
OS_APP_EXT OS_STK TASK_NAME_STK_BUFF[TASK_NAME_STK_SIZE]; //declare the task stack


//created new stack  just in ucos 
	OSTaskCreate(TASK_NAME,(void *)0,(OS_STK *)&TASK_NAME_STK_BUFF[TASK_NAME_STK_SIZE-1],TASK_NAME_PRIO);

 
*/

/*!< 开始 任务的 创建声明 */
void start_task(void* arg);


void task_wifi(void *p_arg);													//the task function name
#define task_wifi_STK_SIZE 150													//the task stake size 
#define task_wifi_PRIO 	8													//the task priority 1~253  @version :u2.51 or later
OS_APP_EXT OS_STK task_wifi_STK_BUFF[task_wifi_STK_SIZE]; //declare the task stack


//created new stack  just in ucos 
//	OSTaskCreate(task_wifi,(void *)0,(OS_STK *)&task_wifi_STK_BUFF[task_wifi_STK_SIZE-1],task_wifi_PRIO);

void task_uart(void *p_arg);													//the task function name
#define task_uart_STK_SIZE 64													//the task stake size 
#define task_uart_PRIO 	7													//the task priority 1~253  @version :u2.51 or later
OS_APP_EXT OS_STK task_uart_STK_BUFF[task_uart_STK_SIZE]; 						//declare the task stack


//created new stack  just in ucos 
//	OSTaskCreate(task_uart,(void *)0,(OS_STK *)&task_uart_STK_BUFF[task_uart_STK_SIZE-1],task_uart_PRIO);


void task_lcd(void *p_arg);													//the task function name
#define task_lcd_STK_SIZE 120													//the task stake size 
#define task_lcd_PRIO 	9													//the task priority 1~253  @version :u2.51 or later
OS_APP_EXT OS_STK task_lcd_STK_BUFF[task_lcd_STK_SIZE]; //declare the task stack


//created new stack  just in ucos 
//	OSTaskCreate(task_lcd,(void *)0,(OS_STK *)&task_lcd_STK_BUFF[task_lcd_STK_SIZE-1],task_lcd_PRIO);

 
void task_key(void *p_arg);													//the task function name
#define task_key_STK_SIZE 64													//the task stake size 
#define task_key_PRIO 	4													//the task priority 1~253  @version :u2.51 or later
OS_APP_EXT OS_STK task_key_STK_BUFF[task_key_STK_SIZE]; //declare the task stack


//created new stack  just in ucos 
//	OSTaskCreate(task_key,(void *)0,(OS_STK *)&task_key_STK_BUFF[task_key_STK_SIZE-1],task_key_PRIO);



void task_main(void *p_arg);													//the task function name
#define task_main_STK_SIZE 270													//the task stake size 
#define task_main_PRIO 	11													//the task priority 1~253  @version :u2.51 or later
OS_APP_EXT OS_STK task_main_STK_BUFF[task_main_STK_SIZE]; //declare the task stack



//created new stack  just in ucos 
//	OSTaskCreate(task_main,(void *)0,(OS_STK *)&task_main_STK_BUFF[task_main_STK_SIZE-1],task_main_PRIO);

void task_UtDistance(void *p_arg);													//the task function name
#define task_UtDistance_STK_SIZE 64													//the task stake size 
#define task_UtDistance_PRIO 5														//the task priority 1~253  @version :u2.51 or later
OS_APP_EXT OS_STK task_UtDistance_STK_BUFF[task_UtDistance_STK_SIZE]; //declare the task stack


//created new stack  just in ucos 
//	OSTaskCreate(task_UtDistance,(void *)0,(OS_STK *)&task_UtDistance_STK_BUFF[task_UtDistance_STK_SIZE-1],task_UtDistance_PRIO);



	/* !< 任务建立： 第一 wifi 联网 自动 登录上 www.bilibili.com 获取网络时间参数 同时也是wifi发送数据的任务
					第二 Usart 电脑端做交互界面接口 可人为设置时间 同时也是 串口发送数据的任务 
					第三 串口接收数据任务： 根据接收来的数据进行不同的信息头检索，做出不同的响应 
					第四 wifi 接收数据任务 将wifi 串口接收到的数据发送给 其他任务， 便于其他任务处理 或者发送给 电脑串口 显示
					第五 显示屏 显示任务 显示屏每隔0.5秒显示 当前CPU 占用度 和 内存使用率 和 当前时间 
					第六 按键扫描 按键扫描出来 按键的 信号量 转移 传递给 主函数 
					第七 主任务函数 接收所有信号量，并处理，分发往各个任务*/




					
#endif
