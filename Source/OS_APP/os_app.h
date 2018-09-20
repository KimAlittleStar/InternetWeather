#ifndef _OS_APP_H_
#define _OS_APP_H_

#include "include.h"

#ifdef _OS_APP_EXT_
#define OS_APP_EXT 

#else
#define OS_APP_EXT extern

#endif

/*!< -------------------------�궨�� ��Ϣ ������--------------------------------------- */

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
	DAILY_DEV* daily; /*!< ĳ���ص� �� ����Ԥ�� */
	u8 updata_hour;	/*!< ����ʱ�� */
	u8 updata_min;
	u8 updata_day;	
	u8 location[15];	/*!< �ص� */
	
	
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













/* !< -------------------------�궨��������  �����ʽ--------------------------------   */
/* !< 

void TASK_NAME(void *p_arg);													//the task function name
#define TASK_NAME_STK_SIZE 64													//the task stake size 
#define TASK_NAME_PRIO 251														//the task priority 1~253  @version :u2.51 or later
OS_APP_EXT OS_STK TASK_NAME_STK_BUFF[TASK_NAME_STK_SIZE]; //declare the task stack


//created new stack  just in ucos 
	OSTaskCreate(TASK_NAME,(void *)0,(OS_STK *)&TASK_NAME_STK_BUFF[TASK_NAME_STK_SIZE-1],TASK_NAME_PRIO);

 
*/

/*!< ��ʼ ����� �������� */
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



	/* !< �������� ��һ wifi ���� �Զ� ��¼�� www.bilibili.com ��ȡ����ʱ����� ͬʱҲ��wifi�������ݵ�����
					�ڶ� Usart ���Զ�����������ӿ� ����Ϊ����ʱ�� ͬʱҲ�� ���ڷ������ݵ����� 
					���� ���ڽ����������� ���ݽ����������ݽ��в�ͬ����Ϣͷ������������ͬ����Ӧ 
					���� wifi ������������ ��wifi ���ڽ��յ������ݷ��͸� �������� �������������� ���߷��͸� ���Դ��� ��ʾ
					���� ��ʾ�� ��ʾ���� ��ʾ��ÿ��0.5����ʾ ��ǰCPU ռ�ö� �� �ڴ�ʹ���� �� ��ǰʱ�� 
					���� ����ɨ�� ����ɨ����� ������ �ź��� ת�� ���ݸ� ������ 
					���� �������� ���������ź������������ַ�����������*/




					
#endif
