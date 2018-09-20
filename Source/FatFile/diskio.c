/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "FlashDrive.h"


/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */


#define FLASH_SECTOR_SIZE 	512			  
//对于W25Q128
//前12M字节给fatfs用,12M字节后,用于存放字库,字库占用3.09M.	剩余部分,给客户自己用	 			    
u16	    FLASH_SECTOR_COUNT=2048*4;	//W25Q64,共8M 前4M使用文件系统管理
#define FLASH_BLOCK_SIZE   	8     	//每个BLOCK有8个扇区

 

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat  = -1;
	int result;

	switch (pdrv) {
	case DEV_RAM :
		if(FlashDriveIC_TYPE != 0)
		result = 0;
		else
			result = STA_NODISK;
		// translate the reslut code here

		return result;

	case DEV_MMC :
		
		// translate the reslut code here

		return STA_NODISK;

	case DEV_USB :
	
		// translate the reslut code here

		return STA_NODISK;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	int result;

	switch (pdrv) {
	case DEV_RAM :
		FlashDrive_init();
		if(FlashDriveIC_TYPE != 0)
		stat=0;

		// translate the reslut code here

		return stat;

	case DEV_MMC :

		// translate the reslut code here
		stat = STA_NODISK;
		return stat;

	case DEV_USB :

		// translate the reslut code here
		stat = STA_NODISK;
		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	case DEV_RAM :
		// translate the arguments here

		FlashDrive_Read(buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE * count);

		// translate the reslut code here
		return RES_OK;

	case DEV_MMC :
		// translate the arguments here


		// translate the reslut code here

		return RES_ERROR;

	case DEV_USB :
		// translate the arguments here


		// translate the reslut code here

		return RES_ERROR;;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	case DEV_RAM :
		// translate the arguments here

		FlashDrive_Write((u8*)buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE*count);

		// translate the reslut code here

		return RES_OK;

	case DEV_MMC :
		// translate the arguments here


		// translate the reslut code here

		return RES_NOTRDY;

	case DEV_USB :
		// translate the arguments here


		// translate the reslut code here

		return RES_NOTRDY;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	int result;
	DWORD* b = (DWORD*) buff;
	switch (pdrv) {
	case DEV_RAM :
	{
		switch(cmd)
	    {
		    case CTRL_SYNC:
				res = RES_OK; 
		        break;	 
		    case GET_SECTOR_SIZE:
		        *b = FLASH_SECTOR_SIZE;
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
		        *b = FLASH_BLOCK_SIZE;
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:
		        *b = FLASH_SECTOR_COUNT;
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;	
		}
	// Process of the command for the RAM drive

		return res;
	}
	 

	case DEV_MMC :

		// Process of the command for the MMC/SD card

		return RES_PARERR;

	case DEV_USB :

		// Process of the command the USB drive

		return RES_PARERR;
	}

	return RES_PARERR;
}

QWORD get_fattime(void)
{
	static QWORD time = 0;
	return time++;
	
}

