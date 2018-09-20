#ifndef _WIFIDRIVE_H_
#define _WIFIDRIVE_H_


#include "_Global.h"
#include "usart3.h"
#include "timer.h"
/*------------------------宏定义  区----------------------*/

#define WIFI_Uart UART3

#define Rx_BUF_MAX_SIZE 500
#define CAMERA_H 60
#define CAMERA_W 10
#define DELAY_MS(ms) delay_us((ms)*1000)
#define WIFI_prinft u3_printf

#define WEATHER_SIT "api.seniverse.com"
#define DATE_SIT "time.windows.com"

typedef enum
{
	WIFI_OK,
	WIFI_Connected,
	WIFI_NOConneted,
	WIFI_Created_AP,
	WIFI_Error,
	WIFI_Server,
	WIFI_Client,
	WIFI_NO_init
} WIFI_STATU;

/*------------------------函数声明区----------------------*/

extern volatile WIFI_STATU Wifi_sta;

u8 WIFI_Init(void);

void WIFI_SentArrayToComputer(u8* ArrayList,u16 ListLength);

void WIFI_prinft(char* fmt,...);

u8 atk_8266_send_cmd(u8 *cmd,u8* ack,u16 waittime );
u8 atk_8266_send_data(u8 *data,u8 *ack,u16 waittime);
u8* WIFi_Get_Rx_BUF(void);
void WIFI_PITIQR_Hander(void);
void WIFI_Rx_IRQHander(void);
void atk_8266_at_response(u8 mode);

#define WIFI_CMD atk_8266_send_cmd
#define WIFI_DATAS atk_8266_send_data

u8 WIFI_JoinAP(const u8* ssid,const u8* pwd,u8 timeout);
u8 WIFI_CreateAP(const u8* ssid,const u8* pwd);
u8 WIFI_JoinServer(const u8* websit,u32 port);
u8 WIFI_CreateSevrver(u32 port);
u8 WIFI_Send_AS_Server(u8 clientID,const u8* data,u16 length);

void WIFI_QuitTCP(void);

#endif

