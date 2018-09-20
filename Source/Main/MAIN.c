#include "include.h"
#include "ff.h"
FATFS fs;

OS_STK start_Task_stk[64];
OS_STK usart_Task_stk[64];
OS_STK lcd_Task_stk[64];


void start_task(void* arg);
void Uart_Task(void* msg);
void LCD_Task(void* msg);
int main(void)
{
	u8* message;
	u8 keyvalue = 0x00;
	u8* usarmessage;
	u32 bw ;
	u8* testarray[20];
	static u8 testnum = 0;
	
	FIL * fp = (FIL*) mymalloc(0,sizeof(FIL));
	
	message = (u8*)mymalloc(SRAMIN,sizeof(u8)*200);

//	f_open(fp,"0:/测试.txt",FA_CREATE_NEW|FA_WRITE);
	
	main_init();
	if(FlashDriveIC_TYPE == W25Q64)
	{
		printf("Flash init success Flash_ID %d",FlashDriveIC_TYPE);
	}else{
		printf("Flash init defult Flash_ID %d",FlashDriveIC_TYPE);
	}
	POINT_COLOR = BLUE;
	BACK_COLOR = WHITE;

/* !< 下午要做的事  
		完善 wifi 驱动，
		完成 wifi 连接热点 
		wifi 创建热点函数 
		wifi  连接 TCP函数 
		wifi 创建TCP 函数 
	
	超声波驱动函数 
		
		初始化 
		得到距离  */

	/* !< 任务建立： 第一 wifi 联网 自动 登录上 www.bilibili.com 获取网络时间参数 同时也是wifi发送数据的任务
					第二 Usart 电脑端做交互界面接口 可人为设置时间 同时也是 串口发送数据的任务 
					第三 串口接收数据任务： 根据接收来的数据进行不同的信息头检索，做出不同的响应 
					第四 wifi 接收数据任务 将wifi 串口接收到的数据发送给 其他任务， 便于其他任务处理 或者发送给 电脑串口 显示
					第五 显示屏 显示任务 显示屏每隔0.5秒显示 当前CPU 占用度 和 内存使用率 和 当前时间 
					第六 按键扫描 按键扫描出来 按键的 信号量 转移 传递给 主函数 
					第七 主任务函数 接收所有信号量，并处理，分发往各个任务*/
	
	
	OSInit();   
 	
	/* !< 起始任务 堆栈地址 自减 所以放在末尾 由CM3 内核决定 */
	OSTaskCreate(start_task,(void *)0,(OS_STK *)&start_Task_stk[63],10 );//创建起始任务
	
	OSStart();	
	
/*
//	while(1)
//	{
//		usarmessage = UsartPortGetReceiverString();
//		keyvalue = key_scan(KEY_SCAN_MODE_ONCE);
//		if(keyvalue == KEY1)
//		{
//			LCD_Clear(WHITE);
//			usarmessage = UsartPortGetReceiverString();
//			LCD_ShowString(0,0,lcddev.width-(lcddev.width%12),48,24,"Star reciver\n Usart infomation.");
//			while(usarmessage==NULL)
//			{
//				usarmessage = UsartPortGetReceiverString();
//				LCD_ShowString(0,48,10*12,24,24,"Waiting...");
//				delay_ms(300);
//				LCD_Fill(0,48,10*12,48+24,WHITE);
//				delay_ms(100);
//			}
//			FlashDrive_Write(usarmessage,0xFF,strlen((char*)usarmessage)+1);
//			printf("flash---:%s\n",usarmessage);
//			sprintf((char*)message,"test of Stm was %s",usarmessage);
//			StmFlash_Write(0X08070000,(u16*)message,strlen((char*)message)+1);
//			printf("stm---:%s\n",message);
//		}
//		if(keyvalue== KEY2)
//		{
//			LCD_Clear(WHITE);
//			FlashDrive_Read(message,0xFF,200);
//			LCD_ShowString(0,0,lcddev.width-(lcddev.width%12),((strlen(message))/(320/12)+1)*24,24,message);
//			printf("flash:%s\n",message);
//			StmFlash_Read(0X08070000,(u16*)message,200);
//			LCD_ShowString(0,400,lcddev.width-(lcddev.width%12),((strlen(message))/(320/12)+1)*24,24,message);
//			printf("stmflash :%s\n",message);
//			
//		}
//		if(keyvalue == KEY3)
//		{
//			testnum++;
//			testarray[testnum] =(u8*) mymalloc(0,sizeof(u8)*150*testnum+1);
//				sprintf(message,"the memrary usage %d %% \n        testnum :%d",my_mem_perused(0),testnum);
//			LCD_Fill(0,lcddev.height/2,lcddev.width,lcddev.height,WHITE);
//			LCD_ShowString(0,lcddev.height/2,lcddev.width,lcddev.height/2-1,16,message);
//			f_open(fp,"0:/test.txt",FA_OPEN_ALWAYS|FA_READ);
//			f_read(fp,message,5,&bw);
//			LCD_ShowString(0,lcddev.height/4*3,lcddev.width,lcddev.height/2-1,16,message);
//			f_close(fp);
//			
//			f_open(fp,"0:/测试.txt",FA_OPEN_ALWAYS|FA_READ);
//			f_read(fp,message,15,&bw);
//			printf("%s,\t mun: %d\n",message,bw);
//			f_close(fp);
////			myfree(0,fp);
//			
//		}else
//			if(keyvalue == KEY4 && testnum > 0)
//			{
//				
//				myfree(0,testarray[testnum]);
//				sprintf(testarray[testnum-1],"the memrary usage %d %% \n        testnum :%d",my_mem_perused(0),testnum);
//				LCD_Fill(0,lcddev.height/2,lcddev.width,lcddev.height,WHITE);
//				LCD_ShowString(0,lcddev.height/2,lcddev.width,lcddev.height/2-1,16,testarray[testnum-1]);
//				testnum--;
//				
//			}
//		
//	}*/
}


/* !< 串口程序 任务 */
void Uart_Task(void * msg)
{
	u16 count = 0;
	u8 info[60];
	strcpy((char *)info,msg);
	strcat((char *)info,"count was %d\n");
	while(1)
	{
		printf((const char*)info,count++);
		delay_ms(300);
	}
}

/* !< 屏幕程序任务 */
void LCD_Task(void* msg)
{
	u16 count = 0;
//	u8 info[40];
	u8 info2[60];

	while(1)
	{
		sprintf(info2,"%s :the message was %d",msg,count);
		count++;
		LCD_Fill(0,(count%29)*16,lcddev.width,(count%29)*16+16,BLUE);
		delay_ms(5);
//		printf("fill and count = %d",count);
		LCD_ShowString(0,(count%29)*16,lcddev.width,16,16,info2);
		delay_ms(10);
		
	}
}
void main_init(void)
{
	u8 res = 0xFF, fontflag;
	u8* word = (u8*)mymalloc(0,sizeof(u8)*4096);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级

	delay_init();
	usartDrive_init(USARTBOUND);
	LCD_Init();
	FlashDrive_init();
	if(font_init() == 0)
	{
		DEBUG("中文字库初始化成功\n");
		POINT_COLOR = RED;
		Show_Str(0,0,lcddev.width,16,"中文字库初始化成功",16,0);
		delay_ms(500);
		fontflag = 1;
	}
	if(fontflag)
	{
		POINT_COLOR = BLUE;
		Show_Str(0,fontflag*16,lcddev.width,16,"初始化按键驱动...",16,0);
		delay_ms(200);
		fontflag++;
	}
	key_init();
	if(fontflag)
	{
		LCD_Fill(lcddev.width-16,fontflag*16,lcddev.width,fontflag*16+16,GREEN);
		Show_Str(lcddev.width-16,fontflag*16,lcddev.width,16,"√",16,1);
	}
	
	
	if(fontflag)
	{
		Show_Str(0,fontflag*16,lcddev.width,16,"初始化内部Flash...",16,0);
		delay_ms(200);
		fontflag++;
	}
	while(res)
	{
		res = f_mount(&fs,"0:",1);
		printf("mount %d\n",res);
		if(res)
		{
			res=f_mkfs("0:",FM_SFD|FM_FAT,4096,word,4096);
			printf("mkfs %d\n",res);
		}
	}
	if(fontflag)
	{
		LCD_Fill(lcddev.width-16,fontflag*16,lcddev.width,fontflag*16+16,GREEN);
		Show_Str(lcddev.width-16,fontflag*16,lcddev.width,16,"√",16,1);
	}
	
	
	if(fontflag)
	{
		Show_Str(0,fontflag*16,lcddev.width,16,"初始化WIFI...",16,0);
		delay_ms(200);
		fontflag++;
	}
	res = 20;
	while(WIFI_Init() != WIFI_OK && res)
	{
		res--;
		if(fontflag)
		{
			LCD_Fill(lcddev.width-32,fontflag*16,lcddev.width,fontflag*16+16,RED);
			Show_Str(lcddev.width-16,fontflag*16,lcddev.width,16,"失败",16,1);
			delay_ms(200);
			Show_Str(lcddev.width-64,fontflag*16,lcddev.width,16,"正在重试",16,1);
			//fontflag++;
		}
		
	}
	if(res == 0)
	{
		printf("wifi was error\n");
		LCD_Fill(lcddev.width-64,fontflag*16,lcddev.width,fontflag*16+16,WHITE);
		POINT_COLOR = RED;
		Show_Str(0,fontflag*16,lcddev.width,16,"wifi初始化失败!",16,0);
		POINT_COLOR = BLUE;
		delay_ms(2000);
		
	}else
	{
		if(fontflag)
		{
			LCD_Fill(lcddev.width-16,fontflag*16,lcddev.width,fontflag*16+16,GREEN);
			Show_Str(lcddev.width-16,fontflag*16,lcddev.width,16,"√",16,1);
		}
		if(WIFI_CMD("AT+CIPSTATUS","STATUS:2",20))
		{
			if(fontflag)
			{
				Show_Str(0,fontflag*16,lcddev.width,16,"正在自动连接wifi...",16,0);
				delay_ms(200);
				fontflag++;
				Show_Str(0,fontflag*16,lcddev.width,16,"WIFI:FAST_309A",16,0);
				delay_ms(200);
				fontflag++;
			}
			if(WIFI_JoinAP("FAST_309A","fzh040502",30) == WIFI_Connected)
			{
				LCD_Fill(lcddev.width-16,fontflag*16,lcddev.width,fontflag*16+16,GREEN);
				Show_Str(lcddev.width-16,fontflag*16,lcddev.width,16,"√",16,1);
				delay_ms(200);
			}else
			{
				LCD_Fill(lcddev.width-32,(fontflag-1)*16,lcddev.width,fontflag*16+16,RED);
				Show_Str(lcddev.width-24,fontflag-1*16,lcddev.width,16,"×",16,1);
				Show_Str(lcddev.width-32,fontflag*16,lcddev.width,16,"失败",16,1);				
				delay_ms(200);
			}
		}
	}
	
	if(fontflag)
	{
		Show_Str(0,fontflag*16,lcddev.width,16,"初始化超声波模块...",16,0);
		delay_ms(200);
		fontflag++;
	}
	UTDrive_init();
	if(fontflag)
	{
		LCD_Fill(lcddev.width-16,fontflag*16,lcddev.width,fontflag*16+16,GREEN);
		Show_Str(lcddev.width-16,fontflag*16,lcddev.width,16,"√",16,1);
	}
	
	if(fontflag)
	{
		Show_Str(0,fontflag*16,lcddev.width,16,"初始化时钟模块...",16,0);
		delay_ms(200);
		fontflag++;
	}
	TimerDrive_init_ms((10*1000)-1,7200-1);
	if(fontflag)
	{
		LCD_Fill(lcddev.width-16,fontflag*16,lcddev.width,fontflag*16+16,GREEN);
		Show_Str(lcddev.width-16,fontflag*16,lcddev.width,16,"√",16,1);
	}
	
	BEEP = 1;
	delay_ms(50);
	printf("init_OK and %d\n",res);
	BEEP = 0;
	LCD_Clear(WHITE);
}

#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line)
{
	
	printf("the pars error in %s ,%d line\n",file,line);

}
#endif

