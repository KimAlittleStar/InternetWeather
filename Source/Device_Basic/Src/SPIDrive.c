/*********************************************
*			SPI�����ļ�˵��
*	���ļ�Ϊ STM32f10x Ӳ�� SPI ���� ���ģ��SPI
*	���ļ�ֻʵ�� �������� ����
*	ռ�ö˿�Ϊ 
*		RST :��λ�ź� �͵�ƽ��λ
* 		DC  :����/ָ�� �ź� һ�� 0 Ϊָ� 1 Ϊ���� 
*		SCLK:ʱ���ź�  ����ʱ���ź� һ�������ض�ȡһλ����
*		DCIN:�����ź�  ��Ϊ 0  ��Ϊ 1
*		CS  :Ƭѡ�ź�  �͵�ƽ��Ч���ߵ�ƽʱ DCIN���� ��Ч
*		 
**********************************************/


/*********************************************
*			SPI�����ļ�˵��
*	
*		
* 		 
*		
*		 
**********************************************/



#include "SPIDrive.h"


#if SPI_MODE == 1

//��� SPI ģʽΪ ���ģʽ

/*********************************************
*	SPI ���� ��ʼ������ 
*	�������ܣ���ʼ�� RST��DC��DCLK��DCIN��CS IO ��״̬
*	��IO ��ģʽ ��Ϊ PP����OUT ���			  
*		 
**********************************************/
void SPIDrive_init(void)
{
	GPIO_InitTypeDef GPIO_InitConstruct;
	
	//ʹ��ʱ��
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPIDrive,ENABLE);
	
	//��ʼ��IO��
			//res IO ��
	GPIO_InitConstruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitConstruct.GPIO_Pin = PIN_SPI_RST;
	GPIO_InitConstruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(Port_SPI_RST,&GPIO_InitConstruct);
	
			//DC IO��
	GPIO_InitConstruct.GPIO_Pin = PIN_SPI_DC;
	
	GPIO_Init(Port_SPI_DC,&GPIO_InitConstruct);
	
			//clk
	GPIO_InitConstruct.GPIO_Pin = PIN_SPI_SCLK;
	
	GPIO_Init(Port_SPI_SCLK,&GPIO_InitConstruct);
	
			//SDIN 
	GPIO_InitConstruct.GPIO_Pin = PIN_SPI_SDIN;
	
	GPIO_Init(Port_SPI_SDIN,&GPIO_InitConstruct);
	
			//CS
	GPIO_InitConstruct.GPIO_Pin = PIN_SPI_CS;
	GPIO_InitConstruct.GPIO_Mode = GPIO_Mode_Out_PP;
	
	GPIO_Init(Port_SPI_CS,&GPIO_InitConstruct);

		
}



#else
//���ΪӲ��SPI ����ֻ��� STM 32f10xϵ�У�
/*********************************************
*	SPI ���� ��ʼ������ 
*	�������ܣ���ʼ�� RST��DC��DCLK��DCIN��CS IO ��״̬
*	���ڶ������ SPI2 �� ֻ�� SPI1 ������ ABP2 �ϣ���������ABP1��
*	RST DC CS Ϊ������� PP����Out ģʽ
*	DCLK DCIN Ϊ AF����PPģʽ
*	�����Ӧ�ܽ���Ҫ�޸����޸� SPIDrive.h �ļ� �� #define ����
**********************************************/

void SPIDrive_init(void)
{
	GPIO_InitTypeDef GPIO_InitConstruct;
	SPI_InitTypeDef SPI_InitConstruct;

	//ʹ��ʱ�� 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_SPIDrive,ENABLE);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);		//spi2 ʱ�ӹ����� 1�ţ� ֻ�� SPI 1 ��2�ţ��������� 1 ��
	
	GPIO_InitConstruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	//reset
	GPIO_InitConstruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitConstruct.GPIO_Pin = PIN_SPI_RST;
	
	GPIO_Init(Port_SPI_RST,&GPIO_InitConstruct);
	
	//DC
	GPIO_InitConstruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitConstruct.GPIO_Pin = PIN_SPI_DC;
	
	GPIO_Init(Port_SPI_DC,&GPIO_InitConstruct);
	
	//SCLK		�����������
	GPIO_InitConstruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitConstruct.GPIO_Pin = PIN_SPI_SCLK;
	
	GPIO_Init(Port_SPI_SCLK,&GPIO_InitConstruct);
	
	//SDIN
	GPIO_InitConstruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitConstruct.GPIO_Pin = PIN_SPI_SDIN;
	
	GPIO_Init(Port_SPI_SDIN,&GPIO_InitConstruct);
	
	GPIO_InitConstruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitConstruct.GPIO_Pin = PIN_SPI_SDOUT;

	GPIO_Init(Port_SPI_SDOUT,&GPIO_InitConstruct);

	//CS
	GPIO_InitConstruct.GPIO_Pin = PIN_SPI_CS;
	GPIO_InitConstruct.GPIO_Mode = GPIO_Mode_Out_PP;
	
	GPIO_Init(Port_SPI_CS,&GPIO_InitConstruct);

	
	
	
	//����Ӳ�� SPI
	
	SPI_InitConstruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;	//���� ���ʷ�Ƶ��36M / 2
	SPI_InitConstruct.SPI_CPHA = SPI_CPHA_2Edge;		//�洢���� �� �ڶ�������ʱ��洢
	SPI_InitConstruct.SPI_CPOL = SPI_CPOL_High;			//����ʱ��  ʱ��״̬Ϊ ��
	SPI_InitConstruct.SPI_CRCPolynomial = 7;			//����У���룿���϶�
	SPI_InitConstruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitConstruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitConstruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitConstruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitConstruct.SPI_NSS = SPI_NSS_Soft;			//����ѡ�� ��һ�� SPI�豸����
	
	SPI_Init(SPIPort,&SPI_InitConstruct);		//�޸���Ӧ�˿�
	
	SPI_Cmd(SPIPort,ENABLE);					//ͬ��

	
}

#endif



#if SPI_MODE == 1
//���  ģ��ģʽ�µ� ���Ͱ�λ����
void SPIDrive_Sent8BitData(u8_spi dat)
{

	//���SPI
	
	u8_spi i;
	
	SPI_SETCS(0);
	for(i = 0;i < 8;i++)
	{
		SPI_SETSCLK(0);		
		if(dat & 0x80)
		{
			SPI_SETSDIN(1);
		}else
		{
			SPI_SETSDIN(0);
		}
		dat <<= 1;
		
		SPI_SETSCLK(1);

		
	}
	SPI_SETCS(1);
}
#else
//Ӳ�� SPI ģʽ�µ� ���� ��λ����
void SPIDrive_Sent8BitData(u8_spi dat)
{
	//Ӳ��SPI
	SPI_SETCS(0);
	
	SPI_I2S_SendData(SPIPort,dat);
						//�ȴ���� �뷢������˳����������ο� Ӳ��SPI ʱ��ͼ�����Ƚ�
	
	
	while(SPI_I2S_GetFlagStatus(SPIPort,SPI_I2S_FLAG_BSY) == RESET)
	{
		//�����ȴ�
	}
	SPI_SETCS(1);
	
		
}

u8_spi SPIDrive_RxTx8BitData(u8_spi dat)
{
	u16_spi i = 1000;
	u8_spi ret = 0; 
	while(SPI_I2S_GetFlagStatus(SPIPort,SPI_I2S_FLAG_TXE ) == RESET && i)
	{
		i--;
	}
	if(i) i= 1000;
	else return 0;
	SPI_I2S_SendData(SPIPort,dat);
	
	
	
	while(SPI_I2S_GetFlagStatus(SPIPort,SPI_I2S_FLAG_RXNE) == RESET && i)
	{
		i--;
	}
	if(i ==0 )
	 return 0;
	ret = (u8_spi)SPI_I2S_ReceiveData(SPIPort);
	return ret;
//		u8 retry=0;				 	
//	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
//		{
//		retry++;
//		if(retry>200)return 0;
//		}			  
//	SPI_I2S_SendData(SPI2, dat); //ͨ������SPIx����һ������
//	retry=0;

//	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) //���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
//		{
//		retry++;
//		if(retry>200)return 0;
//		}	  						    
//	return SPI_I2S_ReceiveData(SPI2); //����ͨ��SPIx������յ�����	
}


#endif




