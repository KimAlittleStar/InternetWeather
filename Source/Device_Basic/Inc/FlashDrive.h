#ifndef _FLASHDRIVE_H_
#define  _FLASHDRIVE_H_
#include "_Global.h"
#include "SPIDrive.h"
#define FlashDrive_TYPE_W25QXX   0
#define FlashDrive_TYPE_OTHER    1
#define FlashDrive_TYPE FlashDrive_TYPE_W25QXX



#if FlashDrive_TYPE == FlashDrive_TYPE_W25QXX
//W25X系列/Q系列芯片列表	   
//W25Q80  ID  0XEF13
//W25Q16  ID  0XEF14
//W25Q32  ID  0XEF15
//W25Q64  ID  0XEF16	
//W25Q128 ID  0XEF17	
#define W25Q80 	0XEF13 	
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16
#define W25Q128	0XEF17

#define FlashDrive_CS PBout(12)

extern u16 FlashDriveIC_TYPE;					//定义W25QXX芯片型号		   


				 
////////////////////////////////////////////////////////////////////////////
 
//指令表
#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 


#endif

void FlashDrive_init(void);
u16 FlashDrive_ReadID(void);
u8	 FlashDrive_ReadSR(void);        		//读取状态寄存器 
void FlashDrive_Write_SR(u8 sr);  			//写状态寄存器
void FlashDrive_Write_Enable(void);  		//写使能 
void FlashDrive_Write_Disable(void);		//写保护
void FlashDrive_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);
void FlashDrive_Read(u8* pBuffer,u32 ReadAddr,u32 NumByteToRead);   //读取flash
void FlashDrive_Write(u8* pBuffer,u32 WriteAddr,u32 NumByteToWrite);//写入flash
void FlashDrive_Erase_Chip(void);    	  	//整片擦除
void FlashDrive_Erase_Sector(u32 Dst_Addr);	//扇区擦除
void FlashDrive_Wait_Busy(void);           	//等待空闲
void FlashDrive_PowerDown(void);        	//进入掉电模式
void FlashDrive_WAKEUP(void);				//唤醒


#if FlashDrive_TYPE == FlashDrive_TYPE_W25QXX

#define W25QXX_Init FlashDrive_init
#define W25QXX_Write FlashDrive_Write
#define W25QXX_Read FlashDrive_Read
#define W25QXX_Erase_Sector FlashDrive_Erase_Sector

#endif

#endif
