/****************************************************************************
* �ļ����� : wifi ģ����� ģ��
* �ļ����� : ���wifi �����������ﱻʵ��
* ע������ :
*****************************************************************************/
#include "WIFIDrive.h"




u8 WIFI_Tx_BUF[480] = {0};
u8 mesage[30];
u8 WIFI_Rx_BUF[Rx_BUF_MAX_SIZE] = {0};
u8 WIFI_RX_STA = 0;		/*<! Ҫ�� MAX SIZE С�ڵ��� 63 */
volatile WIFI_STATU Wifi_sta = WIFI_NO_init;

/****************************************************************************
* ��������: WIFI ģ���ʼ�� 
* �������� : WIFI ģ���ʼ�����������ﱻִ��
* ���� :void(������ʱ���������������ܲ���)
* ��� :��ʼ��״̬ ���ɹ� 0��ʧ�� 1
* ���� :
*****************************************************************************/
u8 WIFI_Init(void)
{
// #if WIFI_FUNCTION == 1
// 	#warning "����δ��д"		/*!< ��д����ɾ���˶� */
// #endif
	u8 quit_count = 15;
	if(Wifi_sta == WIFI_NO_init)
	{
		usart3_init(115200);
		while(atk_8266_send_cmd("AT","OK",20) && quit_count)					/*!< ���� wifi ��Ӧ���� */
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
* ��������: �������е����ݷ��͵� ĳ��������
* �������� : �������е����ݷ��͵�������
* ���� :������  ���鳤��
* ��� :void
* ���� :
*****************************************************************************/
void WIFI_SentArrayToComputer(u8* ArrayList,u16 ListLength)
{
	u8 temp[20];
// #if WIFI_FUNCTION == 1
// 	#warning "����δ��д"		/*!< ��д����ɾ���˶� */
// #endif
	sprintf((char*)temp,"AT+CIPSEND=0,%d",ListLength);
	WIFI_CMD(temp,"OK",20);
        WIFI_prinft("%s\r\n",ArrayList);

}

/****************************************************************************
* ��������: WIFI ���ڴ�ӡ����
* �������� : ʹ�� printf ��ʽ ��������
* ���� :�� Printf ��ͬ��ʽ
* ��� :void
* ���� :kim_alittle_star
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
* ��������: WiFi ģ�鷢�� wifi  ָ�� ����
* �������� :���� ָ�� CMD 
* ���� :u8 * cmd�� Ack �����ش� ��waittime �ȴ�ʱ��
* ��� :�ɹ� ���� 0 ��ʧ�� ���� 1
* ���� :Kim_alitle_star
*****************************************************************************/
u8 atk_8266_send_cmd(u8 *cmd,u8* ack,u16 waittime )
{
	u8 res=0; 
	u8 * msg;
	WIFI_prinft("%s\r\n",cmd);
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ
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
* ��������: WiFI���� ���� 
* �������� :
* ���� :Ҫ���͵����� ����
* ��� :0 �� �ɹ��� ���� ���ɹ�
* ���� :Kim_alitle_star
*****************************************************************************/
u8 atk_8266_send_data(u8 *data,u8 *ack,u16 waittime)
{
	u8 res=0;
	u8* msg; 
	WIFI_prinft("%s",data);	//��������
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ
		{
			DELAY_MS(10);
			msg = WIFi_Get_Rx_BUF();
			if(msg != NULL)//���յ��ڴ���Ӧ����
			{
				if(strstr((const char*)msg,(const char*)ack))
				break;//�õ���Ч���� 				
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
}


/****************************************************************************
* ��������: WIFi_Get_Rx_BUF
* �������� :���� ���� WIFI �յ����ַ���������
* ���� : void
* ��� : u8* 
* ���� :Kim_alitle_star
*****************************************************************************/
u8* WIFi_Get_Rx_BUF(void)
{
#if SYSTEM_SUPPORT_OS
	u8 err;
#endif
	
	if(USART3_RX_STA&0X8000)
	{
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//��ӽ�����
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

//usmart֧�ֲ���
//���յ���ATָ��Ӧ�����ݷ��ظ����Դ���
//mode:0,������USART3_RX_STA;
//     1,����USART3_RX_STA;
void atk_8266_at_response(u8 mode)
{
	u32 i;
	if(USART3_RX_STA&0X8000)		//���յ�һ��������
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//��ӽ�����
		
		if(USART3_RX_BUF[0] ==0x01 && USART3_RX_BUF[1] ==0xfe)
		{
			USART1->DR = USART3_RX_BUF[0];
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������
			USART1->DR = USART3_RX_BUF[1];
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������
			for(i = 0;i<(CAMERA_H*CAMERA_W);i++)
			{
				USART1->DR = USART3_RX_BUF[i+2];
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
			}
			USART1->DR = USART3_RX_BUF[i+2];
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������
			USART1->DR = USART3_RX_BUF[i+3];
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������
	//		printf("cam\n");
		}else
		{
			printf("%s",USART3_RX_BUF);	//���͵�����
		}
		
		if(mode)USART3_RX_STA=0;
	} 
}


/****************************************************************************
* ��������: WIFI_JoinAP
* �������� :���� �Ƿ�ɹ������ȵ�
* ���� : void
* ��� : u8* 
* ���� :Kim_alitle_star
*****************************************************************************/
u8 WIFI_JoinAP(const u8* ssid,const u8* pwd,u8 timeout)
{
	u8 buff[100];
	WIFI_CMD("AT+CWMODE=1","OK",20);							/*!< ���� wifi ģ��Ϊ AP �� �����ȵ�ģʽ */
	WIFI_CMD("AT+RST","OK",500);									/*!< ����wifi							 */
//	delay_us(30*1000);							/*!< �ر� ���� ���ر� 					 */
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
	
//	//	WIFI_CMD("AT+CWJAP=\"ATK_ESP\",\"123456789\",1,4","OK",20);	/*!< ���� wifi �ȵ� ���� ͨ��  ���ܷ�ʽ	 */
//		delay_ms(20000);
//	//	WIFI_CMD("AT+CIFSR","OK",2000);	
//		while(WIFI_CMD("AT+CIPMUX=0","OK",20));	
//		while(WIFI_CMD("AT+CIPSTART=\"TCP\",\"192.168.4.1\",8086","OK",200));
//						//����Ŀ��·����,���һ��IP
//		delay_ms(10000);
//		WIFI_CMD("AT+CIPMODE=1","OK",20);
//		
//		while(WIFI_CMD("AT+CIPSEND","OK",20));	/*!< ����������						   */
//		WIFI_prinft("33");
//		//WIFI_CMD("AT+CIPMUX=0","OK",20)
//		//	WIFI_CMD("AT+CIPSERVER=1,8086","OK",200);					/*!< ���� Ϊ TCP ������ģʽ ��Ӧ��PC Android ����Ϊ 
//																	/*!< TCP �ͻ���ģʽ	
}

/****************************************************************************
* ��������: WIFI_CreateAP
* �������� :���� �Ƿ�ɹ������ȵ�
* ���� : ������ �ȵ���ssid  �ȵ�����pwd
* ��� : u8 wifi statu
* ���� :Kim_alitle_star
*****************************************************************************/
u8 WIFI_CreateAP(const u8* ssid,const u8* pwd)
{
	u8 buff[100];
	WIFI_CMD("AT+CWMODE=2","OK",20);							/*!< ���� wifi ģ��Ϊ AP �� �����ȵ�ģʽ */
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
* ��������: WIFI_JoinServer
* �������� :���� �Ƿ�ɹ����ӵ�������
* ���� : ������ ������ip ����ַ websit  ������ �˿� port
* ��� : u8 wifi statu
* ���� :Kim_alitle_star
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
	{/* !< �������� ���˳� ������ */
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
	{/* !< �������� ���˳� ������ */
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
* ��������: WIFI_QuitTCP
* ��������: �˳�tcp ģʽ,����� �ͻ��� ��ô���˳��ͻ���,����Ƿ����� �͹رշ�����
* ��������: void input:
* �������: void output:
* ���� :author:@Kim_alittle_star
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
			{/*!< ������ �˳�����  */

				if(	WIFI_CMD("AT+CIPCLOSE","OK",20) == 0)
				{
					DEBUG("close ok\n");
				}

			}
			break;			
		}
	}


}







