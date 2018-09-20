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
//��ȡW25QXX��״̬�Ĵ���
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
u8 FlashDrive_ReadSR(void)   
{  
	u8 byte=0;   
	FlashDrive_CS=0;                            //ʹ������   
	SPIDrive_RxTx8BitData(W25X_ReadStatusReg); //���Ͷ�ȡ״̬�Ĵ�������    
	byte=SPIDrive_RxTx8BitData(0Xff);          //��ȡһ���ֽ�  
	FlashDrive_CS=1;                            //ȡ��Ƭѡ     
	return byte;   
} 
//дW25QXX״̬�Ĵ���
//ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
void FlashDrive_Write_SR(u8 sr)   
{   
	FlashDrive_CS=0;                            //ʹ������   
	SPIDrive_RxTx8BitData(W25X_WriteStatusReg);//����дȡ״̬�Ĵ�������    
	SPIDrive_RxTx8BitData(sr);               	//д��һ���ֽ�  
	FlashDrive_CS=1;                            //ȡ��Ƭѡ     	      
}   
//W25QXXдʹ��	
//��WEL��λ   
void FlashDrive_Write_Enable(void)   
{
	FlashDrive_CS=0;                          	//ʹ������   
    SPIDrive_RxTx8BitData(W25X_WriteEnable); 	//����дʹ��  
	FlashDrive_CS=1;                           	//ȡ��Ƭѡ     	      
} 
//W25QXXд��ֹ	
//��WEL����  
void FlashDrive_Write_Disable(void)   
{  
	FlashDrive_CS=0;                            //ʹ������   
    SPIDrive_RxTx8BitData(W25X_WriteDisable);  //����д��ָֹ��    
	FlashDrive_CS=1;                            //ȡ��Ƭѡ     	      
} 		
//��ȡоƬID
//����ֵ����:				   
//0XEF13,��ʾоƬ�ͺ�ΪW25Q80  
//0XEF14,��ʾоƬ�ͺ�ΪW25Q16    
//0XEF15,��ʾоƬ�ͺ�ΪW25Q32  
//0XEF16,��ʾоƬ�ͺ�ΪW25Q64 
//0XEF17,��ʾоƬ�ͺ�ΪW25Q128 	  
u16 FlashDrive_ReadID(void)
{
	u16 Temp = 0;	  
	FlashDrive_CS=0;				    
	SPIDrive_RxTx8BitData(0x90);//���Ͷ�ȡID����	    
	SPIDrive_RxTx8BitData(0x00); 	    
	SPIDrive_RxTx8BitData(0x00); 	    
	SPIDrive_RxTx8BitData(0x00); 	 			   
	Temp|=SPIDrive_RxTx8BitData(0xFF)<<8;  
	Temp|=SPIDrive_RxTx8BitData(0xFF);	 
	FlashDrive_CS=1;				    
	return Temp;
}   		    
//��ȡSPI FLASH  
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//pBuffer:���ݴ洢��
//ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
void FlashDrive_Read(u8* pBuffer,u32 ReadAddr,u32 NumByteToRead)   
{ 
 	u32 i;   										    
	FlashDrive_CS=0;                            	//ʹ������
	if(pBuffer== NULL) return;
    SPIDrive_RxTx8BitData(W25X_ReadData);         	//���Ͷ�ȡ����   
    SPIDrive_RxTx8BitData((u8)((ReadAddr)>>16));  	//����24bit��ַ    
    SPIDrive_RxTx8BitData((u8)((ReadAddr)>>8));   
    SPIDrive_RxTx8BitData((u8)ReadAddr);   
    for(i=0;i<NumByteToRead;i++)
	{ 
        pBuffer[i]=SPIDrive_RxTx8BitData(0XFF);   	//ѭ������  
    }
	FlashDrive_CS=1;  				    	      
}  
//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!	 
void FlashDrive_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
 	u16 i;  
    FlashDrive_Write_Enable();//SET WEL 
		
	FlashDrive_CS=0;                            	//ʹ������   
    SPIDrive_RxTx8BitData(W25X_PageProgram);      	//����дҳ����   
    SPIDrive_RxTx8BitData((u8)((WriteAddr)>>16)); 	//����24bit��ַ    
    SPIDrive_RxTx8BitData((u8)((WriteAddr)>>8));   
    SPIDrive_RxTx8BitData((u8)WriteAddr);   
    for(i=0;i<NumByteToWrite;i++)SPIDrive_RxTx8BitData(pBuffer[i]);//ѭ��д��  
	FlashDrive_CS=1;                            	//ȡ��Ƭѡ 
	FlashDrive_Wait_Busy();					   		//�ȴ�д�����
} 
//�޼���дSPI FLASH 
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ���� 
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
void FlashDrive_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //��ҳʣ����ֽ���		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//������256���ֽ�
	while(1)
	{	   
		FlashDrive_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//д�������
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite>256)pageremain=256; //һ�ο���д��256���ֽ�
			else pageremain=NumByteToWrite; 	  //����256���ֽ���
		}
	};	    
} 
//дSPI FLASH  
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)						
//NumByteToWrite:Ҫд����ֽ���(���65535)   
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
 	secpos=WriteAddr/4096;//������ַ  
	secoff=WriteAddr%4096;//�������ڵ�ƫ��
	secremain=4096-secoff;//����ʣ��ռ��С   
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//������
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//������4096���ֽ�
	while(1) 
	{	
		FlashDrive_Read(FlashDrive_BUF,secpos*4096,4096);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(FlashDrive_BUF[secoff+i]!=0XFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			FlashDrive_Erase_Sector(secpos);		//�����������
			for(i=0;i<secremain;i++)	   		//����
			{
				FlashDrive_BUF[i+secoff]=pBuffer[i];	  
			}
			FlashDrive_Write_NoCheck(FlashDrive_BUF,secpos*4096,4096);//д����������  

		}else FlashDrive_Write_NoCheck(pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(NumByteToWrite==secremain)break;//д�������
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 

		   	pBuffer+=secremain;  				//ָ��ƫ��
			WriteAddr+=secremain;				//д��ַƫ��	   
		   	NumByteToWrite-=secremain;			//�ֽ����ݼ�
			if(NumByteToWrite>4096)secremain=4096;//��һ����������д����
			else secremain=NumByteToWrite;		//��һ����������д����
		}	 
	};	 
}
//��������оƬ		  
//�ȴ�ʱ�䳬��...
void FlashDrive_Erase_Chip(void)   
{                                   
    FlashDrive_Write_Enable();                 	 	//SET WEL 
    FlashDrive_Wait_Busy();   
  	FlashDrive_CS=0;                            	//ʹ������   
    SPIDrive_RxTx8BitData(W25X_ChipErase);        	//����Ƭ��������  
	FlashDrive_CS=1;                            	//ȡ��Ƭѡ     	      
	FlashDrive_Wait_Busy();   				   		//�ȴ�оƬ��������
}   
//����һ������
//Dst_Addr:������ַ ����ʵ����������
//����һ��ɽ��������ʱ��:150ms
void FlashDrive_Erase_Sector(u32 Dst_Addr)   
{  
	//����falsh�������,������   
 	printf("fe:%x\r\n",Dst_Addr);	  
 	Dst_Addr*=4096;
    FlashDrive_Write_Enable();                  	//SET WEL 	 
    FlashDrive_Wait_Busy();   
  	FlashDrive_CS=0;                            	//ʹ������   
    SPIDrive_RxTx8BitData(W25X_SectorErase);      	//������������ָ�� 
    SPIDrive_RxTx8BitData((u8)((Dst_Addr)>>16));  	//����24bit��ַ    
    SPIDrive_RxTx8BitData((u8)((Dst_Addr)>>8));   
    SPIDrive_RxTx8BitData((u8)Dst_Addr);  
	FlashDrive_CS=1;                            	//ȡ��Ƭѡ     	      
    FlashDrive_Wait_Busy();   				   		//�ȴ��������
}  
//�ȴ�����
void FlashDrive_Wait_Busy(void)   
{   
	while((FlashDrive_ReadSR()&0x01)==0x01);  		// �ȴ�BUSYλ���
}  
//�������ģʽ
void FlashDrive_PowerDown(void)   
{ 
  	FlashDrive_CS=0;                           	 	//ʹ������   
    SPIDrive_RxTx8BitData(W25X_PowerDown);        //���͵�������  
	FlashDrive_CS=1;                            	//ȡ��Ƭѡ     	      
    delay_us(3);                               //�ȴ�TPD  
}   
//����
void FlashDrive_WAKEUP(void)   
{  
  	FlashDrive_CS=0;                            	//ʹ������   
    SPIDrive_RxTx8BitData(W25X_ReleasePowerDown);	//  send W25X_PowerDown command 0xAB    
	FlashDrive_CS=1;                            	//ȡ��Ƭѡ     	      
    delay_us(3);                            	//�ȴ�TRES1
}   
