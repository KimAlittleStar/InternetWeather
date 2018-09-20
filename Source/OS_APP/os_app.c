#define _OS_APP_EXT_
#include "os_app.h"
/*!< 创建任务的 开始任务函数  */

/* !< 开始任务的创建 ，在里面激活其他的需要的任务 */
void start_task(void *arg)
{
    OS_CPU_SR cpu_sr = 0;
    arg = arg;
    Eve_mBox_key = OSMboxCreate(NULL);
    Eve_mBox_uart = OSMboxCreate(NULL);
    Eve_mBox_lcd = OSMboxCreate(NULL);
    Eve_mBox_wifi = OSMboxCreate(NULL);
	Eve_mBox_Weather = OSMboxCreate(NULL);
    Eve_mBox_UT = OSMboxCreate(NULL);
    Eve_Q_UT = OSQCreate((void **)Q_UT_BUFF, 25);
    Eve_Q_CPU = OSQCreate((void **)Q_CPUusage_BUFF, 20);
#ifdef _USARTDRIVE_H_
    Eve_mBox_UsartPort = OSMboxCreate(NULL);
#endif
#ifdef _WIFIDRIVE_H_
    Eve_mBox_WIFIBUFF = OSMboxCreate(NULL);
    if (Eve_mBox_WIFIBUFF != NULL)
    {
        printf("wifi mbox ok\n");
    }
    else
    {
        printf("wifi mbox defeat\n");
    }
#endif
    OSStatInit();
    OS_ENTER_CRITICAL(); //进入临界区(无法被中断打断)
                         /*!< 创建 真正的任务  */

    //    OSTaskCreate(task_uart,(void *)0,(OS_STK *)&task_uart_STK_BUFF[task_uart_STK_SIZE-1],task_uart_PRIO);
    //    OSTaskCreate(task_usartRec, (void *)0, (OS_STK *)&task_usartRec_STK_BUFF[task_usartRec_STK_SIZE - 1], task_usartRec_PRIO);

    OSTaskCreate(task_uart, (void *)0, (OS_STK *)&task_uart_STK_BUFF[task_uart_STK_SIZE - 1], task_uart_PRIO);

    OSTaskCreate(task_lcd, (void *)0, (OS_STK *)&task_lcd_STK_BUFF[task_lcd_STK_SIZE - 1], task_lcd_PRIO);
    OSTaskCreate(task_key, (void *)0, (OS_STK *)&task_key_STK_BUFF[task_key_STK_SIZE - 1], task_key_PRIO);
    OSTaskCreate(task_main, (void *)0, (OS_STK *)&task_main_STK_BUFF[task_main_STK_SIZE - 1], task_main_PRIO);

    OSTaskCreate(task_wifi, (void *)0, (OS_STK *)&task_wifi_STK_BUFF[task_wifi_STK_SIZE - 1], task_wifi_PRIO);
    //    OSTaskCreate(task_wifiRec, (void *)0, (OS_STK *)&task_wifiRec_STK_BUFF[task_wifiRec_STK_SIZE - 1], task_wifiRec_PRIO);
    OSTaskCreate(task_UtDistance, (void *)0, (OS_STK *)&task_UtDistance_STK_BUFF[task_UtDistance_STK_SIZE - 1], task_UtDistance_PRIO);
    /*!< 创建任务结束     */
    OSTaskSuspend(10);  //挂起起始任务.
    OS_EXIT_CRITICAL(); //退出临界区(可以被中断打断)
}

/**********************************************************************************
* 函数名称: tmr1_callback
* 函数功能: ucos 软件定时器 回调函数
* 函数输入: void input:
* 函数输出: void output:
* 作者 :author:@Kim_alittle_star
**********************************************************************************/

void tmr1_callback(OS_TMR *ptmr, void *p_arg)
{
    static u32 cpuusage = 0, temp;
    static u8 tcnt = 0;
    if (tcnt == 5)
    {
        temp = cpuusage;
        OSQPost(Eve_Q_CPU, (void *)(temp / 5));
        cpuusage = 0;
        tcnt = 0;
    }
    cpuusage += OSCPUUsage;
    tcnt++;
}
/*
{
    "results":
    [
        {
            "location":
            {
                "id":"WTMKQ069CCJ7",
                "name":"杭州",
                "country":"CN",
                "path":"杭州,杭州,浙江,中国",
                "timezone":"Asia/Shanghai",
                "timezone_offset":"+08:00"
            },
            "daily":
            [
                {
                    "date":"2018-08-25",
                    "text_day":"中雨",
                    "code_day":"14",
                    "text_night":"阵雨",
                    "code_night":"10",
                    "high":"29","low":"23",
                    "precip":"",
                    "wind_direction":"东",
                    "wind_direction_degree":"90",
                    "wind_speed":"20",
                    "wind_scale":"4"
                },
                {"date":"2018-08-26",
                "text_day":"阵雨",
                "code_day":"10",
                "text_night":"阴",
                "code_night":"9",
                "high":"31",
                "low":"25",
                "precip":"",
                "wind_direction":"东",
                "wind_direction_degree":"90",
                "wind_speed":"15",
                "wind_scale":"3"
                }
            ],
            "last_update":"2018-08-25T08:00:00+08:00"
        }
    ]
}
*/
void Weather_init(WEATHER_DEV *noinit)
{
    noinit->daily = NULL;
}

void Weather_free(WEATHER_DEV *inited)
{
    DAILY_DEV* temp1;
    DAILY_DEV* temp2;
    if(inited != NULL)
    {
        temp1 = inited->daily;
        while(temp1 != NULL)
        {
            temp2 = temp1->next;
            myfree(0,temp1);
            temp1 = temp2;
        }
        myfree(0,inited);
    }
}

u8 *app_WeatherToString(WEATHER_DEV *dev)
{
    u8 *ret, *buff;
    u8 i;
    DAILY_DEV *tempdev = dev->daily;
    while(tempdev != NULL)
    {
        i++;
        tempdev = tempdev->next;
    }
    tempdev = dev->daily;
	
    ret = mymalloc(0,sizeof(u8)*130*i);
    sprintf(ret,"%s \n",dev->location);
    buff = mymalloc(0,sizeof(u8)*100);
    while(tempdev != NULL)
    {

        sprintf(buff, "\t%02d-%02d %s 转 %s %d~%d℃\n\t%s风 %d级 %d m/s\n\n",
            tempdev->month,
            tempdev->day,
            tempdev->weather_day,
            tempdev->weather_night,
            tempdev->temprature_low,
            tempdev->temprature_high,
            tempdev->wind_dir,
            tempdev->wind_scale,
            tempdev->wind_speed);
            strcat(ret,buff);
    	tempdev = tempdev->next;
    }
	sprintf(buff,"\n更新时间: %s %02d:%02d\n\n",
            (getSystemDay() == 1)?((dev->updata_day > getSystemDay())?("昨天"):("")):((getSystemDay()>dev->updata_day)?("昨天"):("")), 
            dev->updata_hour,
            dev->updata_min);
	strcat(ret,buff);
    myfree(0, buff);
    return ret;
}

DAILY_DEV *app_getDailyFormStr(u8 *str)
{
    u8 *subptemp;
    u8 i;
    u8 *pentry;
    u8 *ptemp = str;

    DAILY_DEV *newday;
	DAILY_DEV *nextday;
    DAILY_DEV *ret;
    subptemp = StrProcess_GetEntry(ptemp, "", "{}");
    if (subptemp != NULL)
    {
        ret = mymalloc(0, sizeof(DAILY_DEV));
        newday = ret;
        newday->next = NULL;
		nextday = newday;
    }
    while (subptemp != NULL)
    {
		DEBUG("LINE %d was run\n",__LINE__);
        if (nextday == NULL)
        {
            newday->next = mymalloc(0, sizeof(DAILY_DEV));
            newday = newday->next;
			newday->next = NULL;
        }
        pentry = StrProcess_GetEntry(subptemp, "date", "\"\"");
        DEBUG("pentry:: %s\n", pentry);
        /*!< pentry = "2018-mm-dd" */
        newday->month = StrProcess_Str2NUM((pentry + 5), 0);
        newday->day = StrProcess_Str2NUM((pentry + 8), 0);
        myfree(0, pentry);
        /*!< date get over */
        pentry = StrProcess_GetEntry(subptemp, "text_day", "\"\"");
        i = 0;
        while (*(pentry + i) != '\0' && i < 10)
        {
            newday->weather_day[i] = *(pentry + i);
            i++;
        }
        if (i < 10)
        {
            newday->weather_day[i] = '\0';
        }
        else
        {
            strcpy(newday->weather_day, "too long");
        }
        myfree(0, pentry);
        /*!< weather day get over */
        pentry = StrProcess_GetEntry(subptemp, "text_night", "\"\"");
        i = 0;
        while (*(pentry + i) != '\0' && i < 10)
        {
            newday->weather_night[i] = *(pentry + i);
            i++;
        }
        if (i < 10)
        {
            newday->weather_night[i] = '\0';
        }
        else
        {
            strcpy(newday->weather_night, "too long");
        }
        myfree(0, pentry);
        /*!< weather night get over */
        pentry = StrProcess_GetEntry(subptemp, "high", "\"\"");
        newday->temprature_high = StrProcess_Str2NUM(pentry, 1);
        myfree(0, pentry);
        /*!< get  high temprature over */
        pentry = StrProcess_GetEntry(subptemp, "low", "\"\"");
        newday->temprature_low = StrProcess_Str2NUM(pentry, 1);
        myfree(0, pentry);
        /*!< get low temprature over*/
        pentry = StrProcess_GetEntry(subptemp, "wind_direction", "\"\"");
        i = 0;
        while (*(pentry + i) != '\0' && i < 6)
        {
            newday->wind_dir[i] = *(pentry + i);
            i++;
        }
        if (i < 6)
        {
            newday->wind_dir[i] = '\0';
        }
        else
        {
            strcpy(newday->wind_dir, "too long");
        }
        myfree(0, pentry);
        /*!< get wind dir over */
        pentry = StrProcess_GetEntry(subptemp, "wind_speed", "\"\"");
        newday->wind_speed = StrProcess_Str2NUM(pentry, 0);
        myfree(0, pentry);
        /*!< get wind speed over */

        pentry = StrProcess_GetEntry(subptemp, "wind_scale", "\"\"");
        newday->wind_scale = StrProcess_Str2NUM(pentry, 0);
        myfree(0, pentry);
		
        /*!< get wind scale over */

        ptemp += strlen(subptemp);
		myfree(0,subptemp);
        subptemp = StrProcess_GetEntry(ptemp, "", "{}");
        nextday = newday->next;
        
    }/*!< get one daily over  */

	myfree(0,subptemp);
	myfree(0,pentry);
    return ret;
}

/**********************************************************************************
* 函数名称: app_getWeatherFormStr
* 函数功能: 从字符串中 获取到 天气信息
* 函数输入: void input:
* 函数输出: void output:
* 作者 :author:@Kim_alittle_star
**********************************************************************************/
WEATHER_DEV *app_getWeatherFormStr(u8 *str)
{
    u8 *ptemp;
    u8 *subptemp;
    u8 i = 0;
    u8 numbuff[5];
    WEATHER_DEV *ret = mymalloc(0, sizeof(WEATHER_DEV));
    WEATHER_DEV *newWeather = ret;
    Weather_init(newWeather);
    /*!< get the location */
    ptemp = StrProcess_GetEntry(str, "location", "{}");
    DEBUG(ptemp);
    subptemp = StrProcess_GetEntry(ptemp, "name", "\"\"");
    while (*(subptemp + i) != '\0' && i < 15)
    {
        newWeather->location[i] = *(subptemp + i);
        i++;
    }
    if (i < 15)
    {
        newWeather->location[i] = '\0';
    }
    else
    {
        strcpy(newWeather->location, "too long");
    }
    myfree(0, ptemp);
    myfree(0, subptemp);
    /*!< location  get over */

    ptemp = StrProcess_GetEntry(str, "daily", "[]");
    newWeather->daily = app_getDailyFormStr(ptemp);

    myfree(0, subptemp);
    myfree(0, ptemp);

    ptemp = StrProcess_GetEntry(str,"results","[]");
    subptemp = StrProcess_GetEntry(ptemp,"last_update","\"\"");
    /*!< subptemp = "2018-08-25T08:00:00+08:00" */
    newWeather->updata_day = StrProcess_Str2NUM((subptemp+8),0);
    newWeather->updata_hour = StrProcess_Str2NUM((subptemp+11),0);
    newWeather->updata_min = StrProcess_Str2NUM((subptemp+14),0);


    myfree(0, subptemp);
    myfree(0, ptemp);

    return ret;
}

/**********************************************************************************
* 函数名称: app_wifi_updataWeather
* 函数功能: 更新天气函数
* 函数输入: void input:
* 函数输出: void output:
* 作者 :author:@Kim_alittle_star
**********************************************************************************/
void app_wifi_updataWeather(void)
{
    u8 *wifimsg = NULL;
    WEATHER_DEV *weather_dev;
    WIFI_JoinServer(WEATHER_SIT, 80);
    if (Wifi_sta == WIFI_Client)
	{
		WIFI_prinft("GET https://api.seniverse.com/v3/weather/daily.json?key=siftyp2lrtujgut2&location=ip&language=zh-Hans&unit=c&start=0&days=2\r\n");
		while (wifimsg == NULL)
		{
			wifimsg = WIFi_Get_Rx_BUF();
		}
		DEBUG(wifimsg);
		StrProcess_UTF8toGBK(wifimsg, USART3_MAX_RECV_LEN);
		weather_dev = app_getWeatherFormStr(wifimsg);
		OSMboxPost(Eve_mBox_Weather,weather_dev);
		wifimsg = app_WeatherToString(weather_dev);
		DEBUG(wifimsg);
		myfree(0,wifimsg);
	}
	else
        DEBUG("weather defeat\n");
    
}

/**********************************************************************************
* 函数名称: app_wifi_update
* 函数功能: 更新时间函数
* 函数输入: void input:
* 函数输出: void output:
* 作者 :author:@Kim_alittle_star
**********************************************************************************/
void app_wifi_update(void)
{
    WIFI_JoinServer(DATE_SIT, 80);
    if (Wifi_sta == WIFI_Client)
        WIFI_prinft("http\r\n");
    else
        DEBUG("update defeat\n");
}

/**********************************************************************************
* 函数名称: task_wifi
* 函数功能: wifi   任务具体的执行函数
* 函数输入: void input:
* 函数输出: void output:
* 作者 :author:@Kim_alittle_star
**********************************************************************************/
//OSTaskCreate(task_wifi,(void *)0,(OS_STK *)&task_wifi_STK_BUFF[task_wifi_STK_SIZE-1],task_wifi_PRIO);
void task_wifi(void *p_arg)
{
    u8 err, i = 0;
    u8 ssid[36];
    u8 pwd[36];
    u8 websit[64];
    u8 *msg = NULL;
    u8 *ptemp;
    u8 count = 10;
    /*!< 判断wifi 是否初始化 初始化是否成功  */
    if (Wifi_sta == WIFI_NO_init)
    {
        //发送消息给lcd屏幕 显示 消息 "wifi init\n...ing"
        OSMboxPost(Eve_mBox_wifi, "wifi Inition...\n");
        while (WIFI_Init() != WIFI_OK && count)
        {
            count--;
        }
        if (count)
        {
            //发送消息 初始化成功
            OSMboxPost(Eve_mBox_wifi, "success!");
        }
        else
        {
            OSMboxPost(Eve_mBox_wifi, "defeat!");
            //send msg  init defult;
        }
    }
    if (Wifi_sta != WIFI_NO_init)
        OSMboxPost(Eve_mBox_wifi, "success!");
    while (1)
    {
        i = 0;
        if (count) /*!< 成功初始化完成之后 ，向Usart 串口发送任务发送消息 请求接收 wifi 名称 和密码  */
        {          /*!< if the wifi was ok  */
            msg = OSMboxPend(Eve_mBox_wifi, 20, &err);
            if (err == OS_ERR_NONE && msg != NULL)
            {
                ptemp = strstr(msg, "SSID:");
                if (ptemp != NULL)
                {
                    ptemp += 5;
                    while (*ptemp >= 32 && *ptemp <= 127 && i < 36)
                    {
                        ssid[i] = *ptemp;
                        i++;
                        ptemp++;
                    }
                    ssid[i] = '\0';
                }
                ptemp = NULL;
                i = 0;
                ptemp = strstr(msg, "PWD:");

                if (ptemp != NULL)
                {
                    ptemp += 4;
                    while (*ptemp >= 32 && *ptemp <= 127 && i < 36)
                    {
                        pwd[i] = *ptemp;
                        i++;
                        ptemp++;
                    }
                    pwd[i] = '\0';
                }
                if (i != 0)
                {
                    WIFI_JoinAP(ssid, pwd, 30);
                    if (Wifi_sta == WIFI_Connected)
                    {
                        DEBUG("connected!\n");
                    }
                    else
                    {
                        DEBUG("#######!\n");
                    }
                }
                if (Wifi_sta == WIFI_Connected || Wifi_sta == WIFI_Created_AP || Wifi_sta == WIFI_Server || Wifi_sta == WIFI_Client)
                {
                    ptemp = strstr(msg, "SERVER:");
                    if (ptemp != NULL)
                    {
                        WIFI_CreateSevrver(3344);
                        if (Wifi_sta == WIFI_Server)
                        {
                        }
                    }
                    else
                    {
                        i = 0;
                        ptemp = strstr(msg, "CLIENT:");
                        if (ptemp != NULL)
                        {
                            ptemp = strstr(msg, "WEBSIT:");

                            if (ptemp != NULL)
                            {
                                ptemp += 7;
                                //                                while (*ptemp != '\n' && *ptemp != '\r' && *ptemp != '\0' &&  *ptemp != ' ' && i < 64)
                                while (*ptemp >= 32 && *ptemp <= 127 && i < 64)
                                {
                                    websit[i] = *ptemp;
                                    i++;
                                    ptemp++;
                                }
                                websit[i] = '\0';
                            }

                            if (i != 0)
                            {
                                DEBUG("join websit\n");
                                WIFI_JoinServer(websit, 80);
                            }
                        }
                    }
                    if (strstr(msg, "UPDATE"))
                    {
                        app_wifi_update();
                    }
                    else if (strstr(msg, "UPWeather"))
                    {
                        app_wifi_updataWeather();
                    }
                    else if (Wifi_sta == WIFI_Client)
                    {
                        if (strstr(ptemp, "#quit"))
                            WIFI_QuitTCP();
                        else if (err == OS_ERR_NONE && ptemp != NULL)
                        {
                            WIFI_prinft(ptemp);
                        }
                    }
                }
                else
                {
					
//                    WIFI_JoinAP("FAST_309A", "fzh040502", 30);
                }
            }
        }
        delay_ms(100);
    }

    /*!< 接收到wifi 名称和密码之后 连接到目标为wifi ，连接成功后 请求 接收 目标网址 和端口号  */
    /*!< 成功连接TCP 后 等待 消息队列 有消息直接发往tcp队列 */
    /*!< 对方如果需要重连TCP 即退出 重新连接新的TCP客户端  */
}

/**********************************************************************************
* 函数名称: task_uart
* 函数功能: 串口 任务具体的执行函数
* 函数输入: void input:
* 函数输出: void output:
* 作者 :author:@Kim_alittle_star
**********************************************************************************/
void task_uart(void *p_arg)
{
    u8 *msg = NULL;
    u8 err;
    /*!< 接收来自系统的消息 发送出去 */
    while (1)
    {
        msg = OSMboxPend(Eve_mBox_uart, 0, &err);
        if (err == OS_ERR_NONE && msg != NULL)
        {
            printf(msg);
        }
        delay_ms(50);
    }
}

/**********************************************************************************
* 函数名称: task_usartRec
* 函数功能: 串口接收函数的真正执行函数
* 函数输入: void input:
* 函数输出: void output:
* 作者 :author:@Kim_alittle_star
**********************************************************************************/
void task_usartRec(void *p_arg)
{
    /*!< 接收到了不同的消息命令 解析之后发送到各个应有的任务中去  */
    u8 *msg = NULL;
    while (1)
    {
        msg = UsartPortGetReceiverString();
        if (msg != NULL)
        {
//            OSMboxPost(Eve_mBox_uartRec, msg);
        }
        delay_ms(100);
    }
}

/**********************************************************************************
* 函数名称: task_wifiRec
* 函数功能: wifi接收函数的真正执行函数
* 函数输入: void input:
* 函数输出: void output:
* 作者 :author:@Kim_alittle_star
**********************************************************************************/
void task_wifiRec(void *p_arg)
{
    /*!< 接收到了不同的消息命令 解析之后发送到各个应有的任务中去  */
    u8 *msg;
    u8 err;
    while (1)
    {
        if (Wifi_sta == WIFI_Client || Wifi_sta == WIFI_Server)
        {
            msg = WIFi_Get_Rx_BUF();
            if (msg != NULL)
            {
                OSMboxPost(Eve_mBox_uart, msg);
                OSMboxPost(Eve_mBox_wifiRec, msg);
            }
        }

        delay_ms(100);
    }
}

void app_showDaily2LCD(u16 sx,u16 sy,u16 weight,u16 hight,DAILY_DEV *daily)
{
    sx += 5;
    sy += 5;
    weight-=5;
    hight -= 5;
    u8 charsize;
    u8* buff = mymalloc(0,sizeof(u8)*50);
	u8* imgbuff;
    charsize = (hight >= (16*4))?(16):(12);
    POINT_COLOR = BLUE;
    LCD_DrawRectangle(sx,sy,sx+64,sy+64);
    POINT_COLOR = BLACK;
    /*!< 温度 */
    sprintf(buff,"%d~%d℃",daily->temprature_low,daily->temprature_high);
    Show_Str(sx+(hight*3/4)+2,sy,weight-(sx+(hight*3/4)+5),charsize,buff,charsize,1);

    /*!< 风向 */
    sprintf(buff,"%s风",daily->wind_dir);
    Show_Str(sx+(hight*3/4)+2,sy+(hight*1/4),weight-(sx+(hight*3/4)+5),charsize,buff,charsize,1);

    /*!< 风力等级  */
    sprintf(buff,"  等级:%d",daily->wind_scale);
    Show_Str(sx+(hight*3/4)+2,sy+(hight*2/4),weight-(sx+(hight*3/4)+5),charsize,buff,charsize,1);
    /*!< 风速 */
    sprintf(buff,"%d m/s",daily->wind_speed);
    Show_Str(sx+weight-(strlen(buff)*12/2),sy+(hight*3/4),strlen(buff)*12/2,12,buff,12,1);

    /*!< 天气 */
    if(strcmp(daily->weather_day,daily->weather_night) == 0)
    {
        sprintf(buff,"%s",daily->weather_day);
    }else{
        sprintf(buff,"%s转%s",daily->weather_day,daily->weather_night);
    }
    Show_Str_Mid(sx,sy+(hight*3/4),buff,charsize,hight);

    LCD_Fill(sx,sy,sx+64,sy+64,RED);
	
	imgbuff = mymalloc(0,sizeof(u8)*50);
	 POINT_COLOR = WHITE;
	sprintf(imgbuff,"日期:%02d-%02d这是一张%s的图片",daily->month,daily->day, buff);
	Show_Str(sx+5,sy+5,62,62,imgbuff,12,1);

    POINT_COLOR = BLACK;
    myfree(0,buff);
	myfree(0,imgbuff);
}

/**********************************************************************************
* 函数名称: app_showWeather2LCD
* 函数功能: 将 天气 显示到屏幕当中去 
* 函数输入: void input:屏幕窗口的 x,y  宽  高  天气结构体 指针 
* 函数输出: void output:
* 作者 :author:@Kim_alittle_star
**********************************************************************************/
void app_showWeather2LCD(u16 sx,u16 sy,u16 weight,u16 hight,WEATHER_DEV *dev)
{
    u16 d_x,d_y,d_weight,d_hight;
    u8 day_ = 0;
    DAILY_DEV* daytemp;
    u8* buff;
	LCD_Fill(sx,sy,sx+weight,sy+hight,BACK_COLOR);
    sx += 1;
    sy += 1;
    hight -=1;
    weight -=1;
	
    POINT_COLOR = LIGHTBLUE;
    LCD_DrawRectangle(sx,sy,sx+weight,sy+hight);
    LCD_DrawLine(sx,sy+26,sx+weight,sy+26); /*!< 地名栏  */
   
    LCD_DrawLine(sx,sy+hight-14,sx+weight,sy+hight-14); /*!< 最后更新时间栏 */
    POINT_COLOR = BLACK;
    if(dev == NULL) return;         /*!< 放错检测 */
    else
    {
        buff = mymalloc(0,sizeof(u8)*50);
        sprintf(buff,"更新时间: %s %02d:%02d",
            (getSystemDay() == 1)?((dev->updata_day > getSystemDay())?("昨天"):("")):((getSystemDay()>dev->updata_day)?("昨天"):("")), 
            dev->updata_hour,
            dev->updata_min);
        Show_Str_Mid(sx,sy+hight-13,buff,12,weight);
        Show_Str_Mid(sx,sy,dev->location,24,weight);

        /*!< d */
        daytemp = dev->daily;
        while(daytemp != NULL)
        {
            day_++;
            daytemp = daytemp->next;
        }
        d_x = sx;
        d_y = sy+26;
        d_weight  = weight;
        d_hight = (hight-26-14) /day_;
        daytemp = dev->daily;
        while(day_ != 0)
        {
            app_showDaily2LCD(d_x,d_y,d_weight,d_hight,daytemp);
            LCD_DrawLine(sx,d_y+d_hight-2,sx+weight,d_y+d_hight-2);
            daytemp = daytemp->next;
            d_y += d_hight+1;
            day_--;
        }




    }
	myfree(0,buff);
    


}

/**********************************************************************************
* 函数名称: lcd_load
* 函数功能: LCD任务执行前的加载界面程序
* 函数输入: void input:
* 函数输出: void output:
* 作者 :author:@Kim_alittle_star
**********************************************************************************/
void lcd_load(void)
{
    u16 i;
    u8 j;
    POINT_COLOR = BLACK;
    LCD_DrawLine(0, 16 * 5 + 24 * 6 + 3, lcddev.width, 16 * 5 + 24 * 6 + 4);
    LCD_DrawLine(0, 16 * 5 + 24 * 6 + 7, lcddev.width, 16 * 5 + 24 * 6 + 6);

    LCD_DrawLine(0, 16 * 5 + 24 * 4 + 3, lcddev.width, 16 * 5 + 24 * 4 + 3);

    POINT_COLOR = BLUE;
    LCD_DrawLine(0, 16 * 5 + 24 * 6 + 5, lcddev.width, 16 * 5 + 24 * 6 + 5);

    LCD_DrawLine(0, 16 * 5 + 24 * 4, lcddev.width, 16 * 5 + 24 * 4);
    LCD_DrawLine(0, 16 * 5 + 24 * 4 + 5, lcddev.width, 16 * 5 + 24 * 4 + 5);
    for (i = 1; i < 20; i++)
    {
        POINT_COLOR = BLUE;
        LCD_DrawLine(0, 16 * 5 + 24 * 2, 2 * i, 16 * 5 + 24 * 4);
        LCD_DrawLine(lcddev.width, 16 * 5 + 24 * 2, lcddev.width - 2 * i, 16 * 5 + 24 * 4);
        //		POINT_COLOR = GRED;
        //		LCD_DrawLine(0,16*5+24*2-2,i*i-2,16*5+24*4);
        //		LCD_DrawLine(lcddev.width,16*5+24*2-2,lcddev.width-i*i+2,16*5+24*4);
        //
    }
    POINT_COLOR = RED;
    LCD_DrawLine(24 * 2 * 3 - 30, 16 * 5 + 24 * 4 + 5, 24 * 2 * 3 - 30, 16 * 5 + 24 * 6 + 3);
    LCD_DrawLine(24 * 2 * 3 + 40 + 30, 16 * 5 + 24 * 4 + 5, 24 * 2 * 3 + 40 + 30, 16 * 5 + 24 * 6 + 3);

    Show_Str(24 * 2 * 3 + 40 + 32, 16 * 5 + 24 * 4 + 20, lcddev.width - (24 * 2 * 3 + 40 + 32), 16, (u8 *)"测距 :", 16, 0);
    LCD_ShowString(216 + 8 * 11, 16 * 5 + 24 * 4 + 20, lcddev.width - (216 + 8 * 11), 16, 16, (u8 *)"cm");

    LCD_ShowString(2, 16 * 5 + 24 * 4 + 12, 32, 16, 16, (u8 *)"CPU:");
    LCD_ShowString(50, 16 * 5 + 24 * 4 + 12, 8, 16, 16, (u8 *)"%");
    LCD_ShowString(2, 16 * 5 + 24 * 4 + 28, 32, 16, 16, (u8 *)"ROM:");
    LCD_ShowString(50, 16 * 5 + 24 * 4 + 28, 8, 16, 16, (u8 *)"%");

    for (i = 0; i * 48 <= (lcddev.width - 29); i++)
    {

        POINT_COLOR = GRED;
        LCD_Draw_Circle(15 + (i * 48), 24, 24);
        POINT_COLOR = YELLOW;
        LCD_Draw_Circle(15 + (i * 48), 25, 24);
        POINT_COLOR = GREEN;
        LCD_Draw_Circle(15 + (i * 48), 26, 24);
        POINT_COLOR = BLUE;
        LCD_Draw_Circle(15 + (i * 48), 27, 24);
        for (j = 6; j < 12; j++)
        {
            switch (j)
            {
            case 6:
                POINT_COLOR = GRED;
                break;
            case 7:
                POINT_COLOR = MAGENTA;
                break;
            case 8:
                POINT_COLOR = CYAN;
                break;
            case 9:
                POINT_COLOR = BROWN;
                break;
            case 10:
                POINT_COLOR = BRRED;
                break;
            case 11:
                POINT_COLOR = GREEN;
                break;
            }
            LCD_Draw_Circle(15 + i * 48, 24 + j, 24);
        }
    }
    LCD_Fill(0, 15, 3, 16 * 4, WHITE);
	POINT_COLOR = DARKBLUE;
	LCD_DrawLine(lcddev.width/2,16 * 5 + 24 * 6 + 7,lcddev.width/2,lcddev.height);
	
    POINT_COLOR = BLACK;
}

/**********************************************************************************
* 函数名称: task_lcd
* 函数功能: 显示屏任务执行的函数
* 函数输入: void input:
* 函数输出: void output:
* 作者 :author:@Kim_alittle_star
**********************************************************************************/
void task_lcd(void *p_arg)
{
    PRINTDEV *dev_print;
    WEATHER_DEV * dev_weather;
    u8 err, line = 0;
    u8 maxline, maxlong, temphour;
    u32 year, distance, cpuusage;
    u8 month, day, hour, min, sec, week;
    /*!< 接收消息 按照规律显示文字信息在LCD屏幕上  */
    lcd_load();
    while (1)
    {
        dev_print = OSMboxPend(Eve_mBox_lcd, 20, &err);       
        if (err == OS_ERR_NONE && dev_print != NULL)
        {
            maxline = lcddev.height / dev_print->charsize;
            maxlong = (lcddev.width/2-2)/ (dev_print->charsize / 2);
            dev_print->line = (dev_print->line < (maxline / 2)) ? (maxline / 2) : (dev_print->line);
            dev_print->length = strlen(dev_print->text);
//            if (dev_print->startx == (lcddev.width/2+2))
//            {
//                dev_print->line++;
//            }
            line = dev_print->line;

            line += (dev_print->length / maxlong);
            if (dev_print->length % maxlong)
            {
                line++;
            }
            if (line > maxline - 1)
            {
                LCD_Fill(lcddev.width/2+1, maxline / 2 * dev_print->charsize, lcddev.width, lcddev.height, WHITE);
                dev_print->line = maxline / 2;

                line = dev_print->line;
                line += (dev_print->length / maxlong);
                if (dev_print->length % maxlong)
                {
                    line++;
                }
            }
            dev_print->starty = dev_print->charsize * dev_print->line;
 //           LCD_Fill(dev_print->startx, dev_print->starty, lcddev.width, (line + 1) * dev_print->charsize, WHITE);
            Show_Str(dev_print->startx,
                     dev_print->starty,
                     lcddev.width-dev_print->startx,
                     (line - dev_print->line) * dev_print->charsize,
                     dev_print->text,
                     dev_print->charsize, 1);
			dev_print->line = line;
        }
        dev_weather = OSMboxPend(Eve_mBox_Weather,10,&err);
        if(dev_weather != NULL && err == OS_ERR_NONE)
        {
            app_showWeather2LCD(0,16*5 +24*6 +7,lcddev.width/2-1,lcddev.height-(16*5 +24*6 +7)-1,dev_weather);
            Weather_free(dev_weather);
        }
        distance = (u32)OSQPend(Eve_Q_UT, 10, &err);
        if (distance != NULL && err == OS_ERR_NONE)
        {
            LCD_ShowNum(216 + 8 * 6, 16 * 5 + 24 * 4 + 20, distance / 10, 3, 16);
            LCD_ShowChar(216 + 8 * 9, 16 * 5 + 24 * 4 + 20, '.', 16, 0);
            LCD_ShowNum(216 + 8 * 10, 16 * 5 + 24 * 4 + 20, distance % 10, 1, 16);
        }
        cpuusage = (u32)OSQPend(Eve_Q_CPU, 10, &err);
        if (cpuusage != NULL && err == OS_ERR_NONE)
        {
            LCD_ShowNum(34, 16 * 5 + 24 * 4 + 12, cpuusage, 2, 16);
            LCD_ShowNum(34, 16 * 5 + 24 * 4 + 28, my_mem_perused(0), 2, 16);
        }

        POINT_COLOR = RED;

        LCD_ShowChar(16 * 2 * 4 + 3, 15, '-', 16 * 4, 0);
        LCD_ShowChar(16 * 2 * 7 + 3, 15, '-', 16 * 4, 0);

        week = getSystemWeek();

        if (year != getSystemYear())
        {
            year = getSystemYear();
            LCD_ShowNum(3, 15, year, 4, 16 * 4);
        }

        if (month != getSystemMonth())
        {
            month = getSystemMonth();
            LCD_ShowChar(16 * 2 * 5 + 3, 15, '0' + month / 10, 16 * 4, 0);
            LCD_ShowChar(16 * 2 * 6 + 3, 15, '0' + month % 10, 16 * 4, 0);
        }

        if (day != getSystemDay())
        {
            day = getSystemDay();
            LCD_ShowChar(16 * 2 * 8 + 3, 15, '0' + day / 10, 16 * 4, 0);
            LCD_ShowChar(16 * 2 * 9 + 3, 15, '0' + day % 10, 16 * 4, 0);
        }

        if (hour != getSystemHour())
        {
            hour = getSystemHour();
            LCD_ShowChar(24 * 2 - 3, 16 * 5, '0' + hour / 10, 24 * 4, 0);
            LCD_ShowChar(24 * 4 - 3, 16 * 5, '0' + hour % 10, 24 * 4, 0);
            BEEP = (min == 0 && sec < hour * 2 && sec % 2 == 0) ? (1) : (0);
        }
        if (min != getSystemMin())
        {
            min = getSystemMin();
            LCD_ShowChar(24 * 2 * 4 - 3, 16 * 5, '0' + min / 10, 24 * 4, 0);
            LCD_ShowChar(24 * 2 * 5 - 3, 16 * 5, '0' + min % 10, 24 * 4, 0);
        }
        BEEP = ((min == 0 && sec < hour * 2 && sec % 2 == 0) || (sec == 0 && min == 30)) ? (1) : (0);
        if (sec != getSystemSec())
        {
            sec = getSystemSec();
            if (sec % 2)
            {
                LCD_ShowChar(24 * 2 * 3 - 3, 16 * 5, ' ', 24 * 4, 0);
            }
            else
            {
                LCD_ShowChar(24 * 2 * 3 - 3, 16 * 5, ':', 24 * 4, 0);
            }
            if (sec % 10 == 0)
            {
                LCD_ShowChar(24 * 2 * 3 - 3, 16 * 5 + 24 * 4, '0' + sec / 10, 24 * 2, 0);
            }
            LCD_ShowChar(24 * 2 * 3 + 24 - 3, 16 * 5 + 24 * 4, '0' + sec % 10, 24 * 2, 0);
        }

        //        LCD_Fill(0, 0, lcddev.width, 16*4, WHITE);
        //        LCD_ShowString(0, 0, lcddev.width, 16*4, 16*4, timebuff);
        POINT_COLOR = BLACK;
        delay_ms(100);
    }
}

/**********************************************************************************
* 函数名称: task_key
* 函数功能: 按键扫描任务
* 函数输入: void input:
* 函数输出: void output:
* 作者 :author:@Kim_alittle_star
**********************************************************************************/
void task_key(void *p_arg)
{
    /*!< 扫描按键 将按键信息 发送往主任务 */
    u8 keyvalue = 0;
    u8 send;
    while (1)
    {
        keyvalue = key_scan(KEY_SCAN_MODE_ONCE);
        if (keyvalue != 0)
        {
            send = keyvalue;
            OSMboxPost(Eve_mBox_key, &send);
        }
        delay_ms(100);
    }
}

/**********************************************************************************
* 函数名称: void task_main	
* 函数功能: void
* 函数输入: void input:
* 函数输出: void output:
* 作者 :author:@Kim_alittle_star
**********************************************************************************/
void task_main(void *p_arg)
{
    /*!< 接收来自 按键 lcd  wifi 串口的信息 统一整合 设置 获取时间的模式 同时显示内存使用率 CPU使用率  */
    u8 *msg_wifi = NULL;
    u8 *msg_uart = NULL;
    u8 *keyvalue;
    u8 err;
    u8 *ptemp;
    u8 timebuff[45];
    u8 wifista;
    PRINTDEV dev_print;
    static u8 line = 0;
    dev_print.charsize = 12;
    dev_print.startx = (lcddev.width/2+2);
    dev_print.starty = 0;
    dev_print.length = 2;
    dev_print.text = "OK";
    dev_print.line = 0;
    tmr1 = OSTmrCreate(10, 10, OS_TMR_OPT_PERIODIC, (OS_TMR_CALLBACK)tmr1_callback, 0, "tmr1", &err);
    OSTmrStart(tmr1, &err);
    while (Wifi_sta == WIFI_NO_init)
    {
        msg_wifi = OSMboxPend(Eve_mBox_wifi, 15, &err);
        if (err == OS_ERR_NONE && msg_wifi != NULL)
        {
            dev_print.text = msg_wifi;
            dev_print.length = strlen(msg_wifi);
            dev_print.starty = line * 16;
            OSMboxPost(Eve_mBox_lcd, &dev_print);
        }
        if (strstr(msg_wifi, "success") != NULL)
        {
            break;
        }
        delay_ms(200);
    }
    while (1)
    {
        msg_uart = OSMboxPend(Eve_mBox_UsartPort, 15, &err);
        if (err == OS_ERR_NONE)
        {
//			DEBUG("\n\nLINE:%d:%s ",__LINE__,msg_uart);
            UsartPort_Rx_STA = 0;
        }
        msg_wifi = OSMboxPend(Eve_mBox_WIFIBUFF, 15, &err);
        if (err == OS_ERR_NONE)
        {
            DEBUG("wifi rec\n");
            USART3_RX_STA = 0;
        }
        keyvalue = OSMboxPend(Eve_mBox_key, 15, &err);
        if (msg_uart != NULL)
        {
            ptemp = strstr(msg_uart, "CMD:");
            
            if (ptemp != NULL)
            {
				ptemp += 4;
                OSMboxPost(Eve_mBox_wifi, ptemp);
            }
            else
            {
                dev_print.text = msg_uart;
                OSMboxPost(Eve_mBox_lcd, &dev_print);
            }
        }
        if (keyvalue != NULL)
        {
            switch (*keyvalue)
            {
            case KEY1:
                dev_print.text = "正在获取当地天气...";
				OSMboxPost(Eve_mBox_lcd,&dev_print);
				OSMboxPost(Eve_mBox_wifi,"\"UPWeather\":\"ip\"");
                break;

            case KEY2:

                if (Wifi_sta == WIFI_Client)
                {
                    wifista = 3;
                }
                else if (WIFI_CMD("AT+CIPSTATUS", "STATUS:4", 20) == 0)
                {
                    wifista = 4;
                }
                else if (WIFI_CMD("AT+CIPSTATUS", "STATUS:2", 20) == 0)
                {
                    wifista = 2;
                    Wifi_sta = WIFI_Connected;
                }
                switch (wifista)
                {
				case 4:
                    dev_print.text = "wifi未连接 X_X!";
                    OSMboxPost(Eve_mBox_lcd, &dev_print);
                    delay_ms(100);
					dev_print.text = "尝试自动重连...";
                    OSMboxPost(Eve_mBox_lcd, &dev_print);
					WIFI_JoinAP("FAST_309A","fzh040502",20);
                case 2:
                    dev_print.text = "连接服务器...";
                    OSMboxPost(Eve_mBox_lcd, &dev_print);

                    WIFI_JoinServer("www.seniverse.com", 80);
                    delay_ms(50);

                case 3:
                    //WIFI_prinft("http\n");
                    dev_print.text = "从网络获取时间...";
                    OSMboxPost(Eve_mBox_lcd, &dev_print);
					OSMboxPost(Eve_mBox_wifi,"UPDATE\n\n");
                    break;
                
                default:
                    DEBUG("arg illegal\n");
                    break;
                }
                break;
            case KEY4:
                OSMboxPost(Eve_mBox_UT, (void *)1);
                dev_print.text = "正在测量距离";
                OSMboxPost(Eve_mBox_lcd, &dev_print);

                break;
            default:
                DEBUG("key value was %03d\n", *keyvalue);
                break;
            }
        }
        if (msg_wifi != NULL)
        {
            if (Wifi_sta == WIFI_Client || Wifi_sta == WIFI_Server)
            {
                OSMboxPost(Eve_mBox_uart, msg_wifi);
                if (strstr(msg_wifi, "Date") || strstr(msg_wifi, "GMT"))
                {
                    StrProcess_gettime(msg_wifi, timebuff);
                    dev_print.text = timebuff;
                    OSMboxPost(Eve_mBox_lcd, &dev_print);
                }
            }
        }

        delay_ms(200);
    }
}

void task_UtDistance(void *p_arg)
{
    u8 err;
    u8 statu, i;
    u32 disdance, sum;
    for (;;)
    {
        statu = (u8)OSMboxPend(Eve_mBox_UT, 0, &err);
        if (statu != NULL && err == OS_ERR_NONE)
        {
            if (statu == 1)
            {
                sum = 0;
                for (i = 0; i < 3; i++)
                {
                    UTDrive_Measuring();
                    delay_ms(100);
                    disdance = UTDrive_getDistance();
                    if (disdance != 0)
                    {
                        sum += disdance;
                    }
                    else
                    {
                        sum = 0;
                        break;
                    }
                }

                OSQPost(Eve_Q_UT, (void *)(sum / 3));
            }
        }
    }
}
