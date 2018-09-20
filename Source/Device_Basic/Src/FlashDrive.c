#include "FlashDrive.h"

u16 FlashDriveIC_TYPE;

void FlashDrive_init()
{
	FlashDriveIC_TYPE = 0x00;
    SPIDrive_init();
    FlashDrive_CS = 1;
    FlashDriveIC_TYPE = FlashDrive_ReadID();
    printf("Find Flash Ic %d\n" , FlashDriveIC_TYPE);
}
//读取W25QXX的状态寄存器
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
u8 FlashDrive_ReadSR(void)   
{  
	u8 byte=0;   
	FlashDrive_CS=0;                            //使能器件   
	SPIDrive_RxTx8BitData(W25X_ReadStatusReg); //发送读取状态寄存器命令    
	byte=SPIDrive_RxTx8BitData(0Xff);          //读取一个字节  
	FlashDrive_CS=1;                            //取消片选     
	return byte;   
} 
//写W25QXX状态寄存器
//只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写!!!
void FlashDrive_Write_SR(u8 sr)   
{   
	FlashDrive_CS=0;                            //使能器件   
	SPIDrive_RxTx8BitData(W25X_WriteStatusReg);//发送写取状态寄存器命令    
	SPIDrive_RxTx8BitData(sr);               	//写入一个字节  
	FlashDrive_CS=1;                            //取消片选     	      
}   
//W25QXX写使能	
//将WEL置位   
void FlashDrive_Write_Enable(void)   
{
	FlashDrive_CS=0;                          	//使能器件   
    SPIDrive_RxTx8BitData(W25X_WriteEnable); 	//发送写使能  
	FlashDrive_CS=1;                           	//取消片选     	      
} 
//W25QXX写禁止	
//将WEL清零  
void FlashDrive_Write_Disable(void)   
{  
	FlashDrive_CS=0;                            //使能器件   
    SPIDrive_RxTx8BitData(W25X_WriteDisable);  //发送写禁止指令    
	FlashDrive_CS=1;                            //取消片选     	      
} 		
//读取芯片ID
//返回值如下:				   
//0XEF13,表示芯片型号为W25Q80  
//0XEF14,表示芯片型号为W25Q16    
//0XEF15,表示芯片型号为W25Q32  
//0XEF16,表示芯片型号为W25Q64 
//0XEF17,表示芯片型号为W25Q128 	  
u16 FlashDrive_ReadID(void)
{
	u16 Temp = 0;	  
	FlashDrive_CS=0;				    
	SPIDrive_RxTx8BitData(0x90);//发送读取ID命令	    
	SPIDrive_RxTx8BitData(0x00); 	    
	SPIDrive_RxTx8BitData(0x00); 	    
	SPIDrive_RxTx8BitData(0x00); 	 			   
	Temp|=SPIDrive_RxTx8BitData(0xFF)<<8;  
	Temp|=SPIDrive_RxTx8BitData(0xFF);	 
	FlashDrive_CS=1;				    
	return Temp;
}   		    
//读取SPI FLASH  
//在指定地址开始读取指定长度的数据
//pBuffer:数据存储区
//ReadAddr:开始读取的地址(24bit)
//NumByteToRead:要读取的字节数(最大65535)
void FlashDrive_Read(u8* pBuffer,u32 ReadAddr,u32 NumByteToRead)   
{ 
 	u32 i;   										    
	FlashDrive_CS=0;                            	//使能器件
	if(pBuffer== NULL) return;
    SPIDrive_RxTx8BitData(W25X_ReadData);         	//发送读取命令   
    SPIDrive_RxTx8BitData((u8)((ReadAddr)>>16));  	//发送24bit地址    
    SPIDrive_RxTx8BitData((u8)((ReadAddr)>>8));   
    SPIDrive_RxTx8BitData((u8)ReadAddr);   
    for(i=0;i<NumByteToRead;i++)
	{ 
        pBuffer[i]=SPIDrive_RxTx8BitData(0XFF);   	//循环读数  
    }
	FlashDrive_CS=1;  				    	      
}  
//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!	 
void FlashDrive_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
 	u16 i;  
    FlashDrive_Write_Enable();//SET WEL 
		
	FlashDrive_CS=0;                            	//使能器件   
    SPIDrive_RxTx8BitData(W25X_PageProgram);      	//发送写页命令   
    SPIDrive_RxTx8BitData((u8)((WriteAddr)>>16)); 	//发送24bit地址    
    SPIDrive_RxTx8BitData((u8)((WriteAddr)>>8));   
    SPIDrive_RxTx8BitData((u8)WriteAddr);   
    for(i=0;i<NumByteToWrite;i++)SPIDrive_RxTx8BitData(pBuffer[i]);//循环写数  
	FlashDrive_CS=1;                            	//取消片选 
	FlashDrive_Wait_Busy();					   		//等待写入结束
} 
//无检验写SPI FLASH 
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能 
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
void FlashDrive_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //单页剩余的字节数		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//不大于256个字节
	while(1)
	{	   
		FlashDrive_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//写入结束了
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //减去已经写入了的字节数
			if(NumByteToWrite>256)pageremain=256; //一次可以写入256个字节
			else pageremain=NumByteToWrite; 	  //不够256个字节了
		}
	};	    
} 
//写SPI FLASH  
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)						
//NumByteToWrite:要写入的字节数(最大65535)   
u8 FlashDrive_BUFFER[4096];		 
void FlashDrive_Write(u8* pBuffer,u32 WriteAddr,u32 NumByteToWrite)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    
	u8 * FlashDrive_BUF;	  
   	FlashDrive_BUF=FlashDrive_BUFFER;
	if(pBuffer== NULL) return;	
 	secpos=WriteAddr/4096;//扇区地址  
	secoff=WriteAddr%4096;//在扇区内的偏移
	secremain=4096-secoff;//扇区剩余空间大小   
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//测试用
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//不大于4096个字节
	while(1) 
	{	
		FlashDrive_Read(FlashDrive_BUF,secpos*4096,4096);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(FlashDrive_BUF[secoff+i]!=0XFF)break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
			FlashDrive_Erase_Sector(secpos);		//擦除这个扇区
			for(i=0;i<secremain;i++)	   		//复制
			{
				FlashDrive_BUF[i+secoff]=pBuffer[i];	  
			}
			FlashDrive_Write_NoCheck(FlashDrive_BUF,secpos*4096,4096);//写入整个扇区  

		}else FlashDrive_Write_NoCheck(pBuffer,WriteAddr,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		if(NumByteToWrite==secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;//扇区地址增1
			secoff=0;//偏移位置为0 	 

		   	pBuffer+=secremain;  				//指针偏移
			WriteAddr+=secremain;				//写地址偏移	   
		   	NumByteToWrite-=secremain;			//字节数递减
			if(NumByteToWrite>4096)secremain=4096;//下一个扇区还是写不完
			else secremain=NumByteToWrite;		//下一个扇区可以写完了
		}	 
	};	 
}
//擦除整个芯片		  
//等待时间超长...
void FlashDrive_Erase_Chip(void)   
{                                   
    FlashDrive_Write_Enable();                 	 	//SET WEL 
    FlashDrive_Wait_Busy();   
  	FlashDrive_CS=0;                            	//使能器件   
    SPIDrive_RxTx8BitData(W25X_ChipErase);        	//发送片擦除命令  
	FlashDrive_CS=1;                            	//取消片选     	      
	FlashDrive_Wait_Busy();   				   		//等待芯片擦除结束
}   
//擦除一个扇区
//Dst_Addr:扇区地址 根据实际容量设置
//擦除一个山区的最少时间:150ms
void FlashDrive_Erase_Sector(u32 Dst_Addr)   
{  
	//监视falsh擦除情况,测试用   
 	printf("fe:%x\r\n",Dst_Addr);	  
 	Dst_Addr*=4096;
    FlashDrive_Write_Enable();                  	//SET WEL 	 
    FlashDrive_Wait_Busy();   
  	FlashDrive_CS=0;                            	//使能器件   
    SPIDrive_RxTx8BitData(W25X_SectorErase);      	//发送扇区擦除指令 
    SPIDrive_RxTx8BitData((u8)((Dst_Addr)>>16));  	//发送24bit地址    
    SPIDrive_RxTx8BitData((u8)((Dst_Addr)>>8));   
    SPIDrive_RxTx8BitData((u8)Dst_Addr);  
	FlashDrive_CS=1;                            	//取消片选     	      
    FlashDrive_Wait_Busy();   				   		//等待擦除完成
}  
//等待空闲
void FlashDrive_Wait_Busy(void)   
{   
	while((FlashDrive_ReadSR()&0x01)==0x01);  		// 等待BUSY位清空
}  
//进入掉电模式
void FlashDrive_PowerDown(void)   
{ 
  	FlashDrive_CS=0;                           	 	//使能器件   
    SPIDrive_RxTx8BitData(W25X_PowerDown);        //发送掉电命令  
	FlashDrive_CS=1;                            	//取消片选     	      
    delay_us(3);                               //等待TPD  
}   
//唤醒
void FlashDrive_WAKEUP(void)   
{  
  	FlashDrive_CS=0;                            	//使能器件   
    SPIDrive_RxTx8BitData(W25X_ReleasePowerDown);	//  send W25X_PowerDown command 0xAB    
	FlashDrive_CS=1;                            	//取消片选     	      
    delay_us(3);                            	//等待TRES1
}   
