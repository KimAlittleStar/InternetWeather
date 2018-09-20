/****************************************************************************
* 文件名称 : wifi 模块控制 模块
* 文件功能 : 相关wifi 函数将在这里被实现
* 注意事项 :
*****************************************************************************/
#include "WIFIDrive.h"




u8 WIFI_Tx_BUF[480] = {0};
u8 mesage[30];
u8 WIFI_Rx_BUF[Rx_BUF_MAX_SIZE] = {0};
u8 WIFI_RX_STA = 0;		/*<! 要求 MAX SIZE 小于等于 63 */
volatile WIFI_STATU Wifi_sta = WIFI_NO_init;

/****************************************************************************
* 函数名称: WIFI 模块初始化 
* 函数功能 : WIFI 模块初始化程序将在这里被执行
* 输入 :void(传参暂时不明，但函数功能不变)
* 输出 :初始化状态 ：成功 0，失败 1
* 作者 :
*****************************************************************************/
u8 WIFI_Init(void)
{
// #if WIFI_FUNCTION == 1
// 	#warning "函数未重写"		/*!< 重写后请删除此段 */
// #endif
	u8 quit_count = 15;
	if(Wifi_sta == WIFI_NO_init)
	{
		usart3_init(115200);
		while(atk_8266_send_cmd("AT","OK",20) && quit_count)					/*!< 测试 wifi 响应函数 */
		{
			WIFI_prinft("+++");
			DELAY_MS(50);
			atk_8266_send_cmd("AT+CIPMODE=0","OK",20);
			printf("defeat\r\n");
			quit_count--;
		}
		if(quit_count==0)
		{
			DEBUG("quit_file was :\" %s \",line was : \" %d\"\n",__FILE__,__LINE__);
			return Wifi_sta;
		}else
		{
			Wifi_sta = WIFI_OK;
			return Wifi_sta;
		}

	}
	return Wifi_sta;
	
}


/****************************************************************************
* 函数名称: 将数组中的数据发送到 某个电脑上
* 函数功能 : 将数组中的数据发送到电脑上
* 输入 :数组名  数组长度
* 输出 :void
* 作者 :
*****************************************************************************/
void WIFI_SentArrayToComputer(u8* ArrayList,u16 ListLength)
{
	u8 temp[20];
// #if WIFI_FUNCTION == 1
// 	#warning "函数未重写"		/*!< 重写后请删除此段 */
// #endif
	sprintf((char*)temp,"AT+CIPSEND=0,%d",ListLength);
	WIFI_CMD(temp,"OK",20);
        WIFI_prinft("%s\r\n",ArrayList);

}

/****************************************************************************
* 函数名称: WIFI 串口打印函数
* 函数功能 : 使用 printf 格式 发送数据
* 输入 :与 Printf 相同格式
* 输出 :void
* 作者 :kim_alittle_star
*****************************************************************************/
//void WIFI_prinft(char* fmt,...)
//{
//	va_list ap; 
//	va_start(ap,fmt);
//	vsprintf((char*)WIFI_Tx_BUF,fmt,ap);
//	va_end(ap);
//	uart_putstr(WIFI_Uart,(const u8*)WIFI_Tx_BUF);
//}

/****************************************************************************
* 函数名称: WiFi 模块发送 wifi  指令 函数
* 函数功能 :设置 指令 CMD 
* 输入 :u8 * cmd， Ack 期望回答 ，waittime 等待时间
* 输出 :成功 返回 0 ，失败 返回 1
* 作者 :Kim_alitle_star
*****************************************************************************/
u8 atk_8266_send_cmd(u8 *cmd,u8* ack,u16 waittime )
{
	u8 res=0; 
	u8 * msg;
	WIFI_prinft("%s\r\n",cmd);
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			DELAY_MS(10);
//			printf("Rx_%d\n",USART3_RX_STA);
			msg = WIFi_Get_Rx_BUF();		
			if(msg != NULL)
			{
				DEBUG((const char*)msg);
				if(strstr((const char*)msg,(const char*)ack))
				break;
			}else
			{
			//	printf("mes was NULL\n");
			}
		}
		
		if(waittime==0)res=1; 
	}
	return res;
}

/****************************************************************************
* 函数名称: WiFI发动 数据 
* 函数功能 :
* 输入 :要发送的数据 数组
* 输出 :0 即 成功， 非零 不成功
* 作者 :Kim_alitle_star
*****************************************************************************/
u8 atk_8266_send_data(u8 *data,u8 *ack,u16 waittime)
{
	u8 res=0;
	u8* msg; 
	WIFI_prinft("%s",data);	//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			DELAY_MS(10);
			msg = WIFi_Get_Rx_BUF();
			if(msg != NULL)//接收到期待的应答结果
			{
				if(strstr((const char*)msg,(const char*)ack))
				break;//得到有效数据 				
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
}


/****************************************************************************
* 函数名称: WIFi_Get_Rx_BUF
* 函数功能 :返回 接收 WIFI 收到的字符串的数组
* 输入 : void
* 输出 : u8* 
* 作者 :Kim_alitle_star
*****************************************************************************/
u8* WIFi_Get_Rx_BUF(void)
{
#if SYSTEM_SUPPORT_OS
	u8 err;
#endif
	
	if(USART3_RX_STA&0X8000)
	{
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
		USART3_RX_STA=0;
#if SYSTEM_SUPPORT_OS
		OSMboxPend(Eve_mBox_WIFIBUFF,0,&err);
#endif
		
		return  USART3_RX_BUF;
	}else
	{
		return NULL;
	}
}

//usmart支持部分
//将收到的AT指令应答数据返回给电脑串口
//mode:0,不清零USART3_RX_STA;
//     1,清零USART3_RX_STA;
void atk_8266_at_response(u8 mode)
{
	u32 i;
	if(USART3_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
		
		if(USART3_RX_BUF[0] ==0x01 && USART3_RX_BUF[1] ==0xfe)
		{
			USART1->DR = USART3_RX_BUF[0];
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕
			USART1->DR = USART3_RX_BUF[1];
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕
			for(i = 0;i<(CAMERA_H*CAMERA_W);i++)
			{
				USART1->DR = USART3_RX_BUF[i+2];
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
			}
			USART1->DR = USART3_RX_BUF[i+2];
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕
			USART1->DR = USART3_RX_BUF[i+3];
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕
	//		printf("cam\n");
		}else
		{
			printf("%s",USART3_RX_BUF);	//发送到串口
		}
		
		if(mode)USART3_RX_STA=0;
	} 
}


/****************************************************************************
* 函数名称: WIFI_JoinAP
* 函数功能 :返回 是否成功加入热点
* 输入 : void
* 输出 : u8* 
* 作者 :Kim_alitle_star
*****************************************************************************/
u8 WIFI_JoinAP(const u8* ssid,const u8* pwd,u8 timeout)
{
	u8 buff[100];
	WIFI_CMD("AT+CWMODE=1","OK",20);							/*!< 设置 wifi 模块为 AP 即 创建热点模式 */
	WIFI_CMD("AT+RST","OK",500);									/*!< 重启wifi							 */
//	delay_us(30*1000);							/*!< 关闭 回显 ，关闭 					 */
	WIFI_CMD("AT+CWMODE=1","OK",20);
	WIFI_CMD("AT+CIPMUX=0","OK",20);
	if(Wifi_sta != WIFI_Connected)
	{
		sprintf((char*)buff,"AT+CWJAP=\"%s\",\"%s\"",ssid,pwd);
		DEBUG("send to wifi was ----%s-----\n",buff);
		if(atk_8266_send_cmd(buff,"WIFI GOT IP",timeout*100) == 0)
		{
			Wifi_sta = WIFI_Connected;
		}else
		{
			Wifi_sta = WIFI_NOConneted;
		}
	}
	return Wifi_sta;
	
//	//	WIFI_CMD("AT+CWJAP=\"ATK_ESP\",\"123456789\",1,4","OK",20);	/*!< 设置 wifi 热点 密码 通道  加密方式	 */
//		delay_ms(20000);
//	//	WIFI_CMD("AT+CIFSR","OK",2000);	
//		while(WIFI_CMD("AT+CIPMUX=0","OK",20));	
//		while(WIFI_CMD("AT+CIPSTART=\"TCP\",\"192.168.4.1\",8086","OK",200));
//						//连接目标路由器,并且获得IP
//		delay_ms(10000);
//		WIFI_CMD("AT+CIPMODE=1","OK",20);
//		
//		while(WIFI_CMD("AT+CIPSEND","OK",20));	/*!< 开启多连接						   */
//		WIFI_prinft("33");
//		//WIFI_CMD("AT+CIPMUX=0","OK",20)
//		//	WIFI_CMD("AT+CIPSERVER=1,8086","OK",200);					/*!< 设置 为 TCP 服务器模式 对应的PC Android 端设为 
//																	/*!< TCP 客户端模式	
}

/****************************************************************************
* 函数名称: WIFI_CreateAP
* 函数功能 :返回 是否成功创建热点
* 输入 : 创建的 热点名ssid  热点密码pwd
* 输出 : u8 wifi statu
* 作者 :Kim_alitle_star
*****************************************************************************/
u8 WIFI_CreateAP(const u8* ssid,const u8* pwd)
{
	u8 buff[100];
	WIFI_CMD("AT+CWMODE=2","OK",20);							/*!< 设置 wifi 模块为 AP 即 创建热点模式 */
	WIFI_CMD("AT+RST","OK",500);
	DELAY_MS(1000);
//	WIFI_CMD("ATE0","OK",20);
	if(Wifi_sta != WIFI_Created_AP)
	{
		sprintf((char*)buff,"AT+CWSAP=\"%s\",\"%s\",1,4",ssid,pwd);
//		printf("%s------\n",buff);
		if(atk_8266_send_cmd(buff,"OK",500) == 0)
		{
			Wifi_sta = WIFI_Created_AP;
			WIFI_CMD("AT+RST","OK",500);
//			WIFI_CMD("ATE0","OK",20);
		}else
		{
			Wifi_sta = WIFI_Error;
		}
	}
	return Wifi_sta;
}
/****************************************************************************
* 函数名称: WIFI_JoinServer
* 函数功能 :返回 是否成功连接到服务器
* 输入 : 创建的 服务器ip 或网址 websit  服务器 端口 port
* 输出 : u8 wifi statu
* 作者 :Kim_alitle_star
*****************************************************************************/
u8 WIFI_JoinServer(const u8* websit,u32 port)
{
	u8 buff[60];
	port = (port == 0)?(80):(port);
	while(WIFI_CMD("AT","OK",20))
	{
		WIFI_prinft("+++");
		DELAY_MS(50);
		WIFI_prinft("\r\n");
	}
	if(WIFI_CMD("AT+CIPSTATUS","STATUS:3",25)==0)
	{/* !< 建立连接 先退出 在连接 */
		WIFI_CMD("AT+CIPCLOSE","OK",20);		
	}
	sprintf(buff,"AT+CIPSTART=\"TCP\",\"%s\",%d",websit,port);
	if(WIFI_CMD(buff,"OK",50) == 0)
	{
		Wifi_sta = WIFI_Client;
	}
	
	if(Wifi_sta == WIFI_Client)
	{
		if(WIFI_CMD("AT+CIPMODE=1","OK",25) == 0)
		{
			if(WIFI_CMD("AT+CIPSEND",">",25))
			{
//				Wifi_sta = WIFI_Error;
			}
		}else
		{
			Wifi_sta = WIFI_Error;
		}
		
	}else
	{
		Wifi_sta = WIFI_Error;
	}
	return Wifi_sta;
	
}
u8 WIFI_CreateSevrver(u32 port)
{
	u8 buff[100];
	port = (port == 0)?(80):(port);
	while(WIFI_CMD("AT","OK",20))
	{
		WIFI_prinft("+");
		WIFI_prinft("+");
		WIFI_prinft("+");
	}
	if(WIFI_CMD("AT+CIPSTATUS","STATUS:3",25))
	{/* !< 建立连接 先退出 在连接 */
		WIFI_CMD("AT+CIPCLOSE","OK",20);		
	}
	if(WIFI_CMD("AT+CIPMUX=1","OK",25) == 0)
	{
		sprintf(buff,"AT+CIPSERVER=1,%d",port);
		if(WIFI_CMD(buff,"OK",80) == 0)
		{
			Wifi_sta = WIFI_Server;
		}
	}
	Wifi_sta = (Wifi_sta == WIFI_Server)?(Wifi_sta):(WIFI_Error);
	return Wifi_sta;
}
u8 WIFI_Send_AS_Server(u8 clientID,const u8* data,u16 length)
{
	u8 buff[20];
	u8* ptemp = (u8*)data;
	sprintf(buff,"AT+CIPSEND=%d,%d",clientID,length);
	WIFI_CMD(buff,">",20);
	for(;length >0;length--)
	{
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET); 
		USART3->DR = ((*ptemp) & (uint16_t)0x01FF);
		ptemp++;		
	}
	return Wifi_sta;
}
u8 WIFI_Send_AS_Client(const u8* data,u16 length)
{
	u8* ptemp = (u8*)data;
	for(;length >0;length--)
	{
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET); 
		USART3->DR = ((*ptemp) & (uint16_t)0x01FF);
		ptemp++;		
	}
	return Wifi_sta;
}

/**********************************************************************************
* 函数名称: WIFI_QuitTCP
* 函数功能: 退出tcp 模式,如果是 客户端 那么就退出客户端,如果是服务器 就关闭服务器
* 函数输入: void input:
* 函数输出: void output:
* 作者 :author:@Kim_alittle_star
**********************************************************************************/
void WIFI_QuitTCP(void)
{
	u8 i = 20;
	u8* wifimsg;
	while(WIFI_CMD("AT","OK",20))
	{
		WIFI_prinft("+++");
		DELAY_MS(20);		
	}
	WIFI_prinft("AT+CIPSTATUS\r\n");
	while(i)
	{
		i--;
		DELAY_MS(10);
		wifimsg = WIFi_Get_Rx_BUF();
		if(wifimsg != NULL)
		{
			if(strstr(wifimsg,"STATUS:3"))
			{/*!< 有链接 退出连接  */

				if(	WIFI_CMD("AT+CIPCLOSE","OK",20) == 0)
				{
					DEBUG("close ok\n");
				}

			}
			break;			
		}
	}


}







