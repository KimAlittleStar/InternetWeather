#include "UTDrive.h"

void UTDrive_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //ʹ��GPIOAʱ��
    GPIO_InitStructure.GPIO_Pin = UTCTRL_PIN;             
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(UTCTRL_PORT, &GPIO_InitStructure);
    UTCTRL_SET = 0;
    TIM5_Cap_Init(50000-1,72-1);


}

/**********************************************************************************
* ��������: UTDrive_getDistance
* ��������: �õ� ������ĺ��� 
* ��������: void input:
* �������: void output:�������Ƕ��� ��λ 1mm
* ���� :author:@Kim_alittle_star
**********************************************************************************/
u8 TIM5CH3_CAPTURE_STA = 0; //���벶��״̬
u16 TIM5CH3_CAPTURE_VAL;    //���벶��ֵ
TIM_ICInitTypeDef TIM5_ICInitStructure;
u16 period;
u32 UTDrive_getDistance(void)
{
    u32 temp = 0;
	TIM_Cmd(TIM5, DISABLE);
    if (TIM5CH3_CAPTURE_STA & 0X80) //�ɹ�������һ��������
    {
        
        temp = TIM5CH3_CAPTURE_STA & 0X3F;
        temp *= (period + 1);           //���ʱ���ܺ�
        temp += TIM5CH3_CAPTURE_VAL;    //�õ��ܵĸߵ�ƽʱ��
//        DEBUG("HIGH:%d us\r\n", temp); //��ӡ�ܵĸߵ�ƽʱ��
        TIM5CH3_CAPTURE_STA = 0;        //������һ�β���
        temp *= 0.172;
    }else
	{
		TIM5CH3_CAPTURE_STA = 0;
		TIM5CH3_CAPTURE_VAL = 0;
		TIM_SetCounter(TIM5,0);
	}
    return temp;
}

/**********************************************************************************
* ��������: UTDrive_Measuring
* ��������: ��ʼ�������� ���� 
* ��������: void input:
* �������: void output:
* ���� :author:@Kim_alittle_star
**********************************************************************************/
void UTDrive_Measuring(void)
{
    TIM_Cmd(TIM5, ENABLE);
    UTCTRL_SET = 1;
    delay_us(15);
    UTCTRL_SET = 0;
}

/**********************************************************************************
* ��������: TIM5_Cap_Init
* ��������: ��ʼ�� TIM5 �� ���벶����
* ��������: void input:arr ����ֵ psc ��Ƶֵ 
* �������: void output:
* ���� :author:@Kim_alittle_star
**********************************************************************************/

void TIM5_Cap_Init(u16 arr, u16 psc)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);  //ʹ��TIM5ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //ʹ��GPIOAʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;     //PA0 ���֮ǰ����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0 ����
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA, GPIO_Pin_2); //PA0 ����

    //��ʼ����ʱ��5 TIM5
    TIM_TimeBaseStructure.TIM_Period = arr;                     //�趨�������Զ���װֵ
    TIM_TimeBaseStructure.TIM_Prescaler = psc;                  //Ԥ��Ƶ��
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);             //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
    period = arr;
    //��ʼ��TIM5���벶�����
    TIM5_ICInitStructure.TIM_Channel = TIM_Channel_3;                //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
    TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;     //�����ز���
    TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
    TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;           //���������Ƶ,����Ƶ
    TIM5_ICInitStructure.TIM_ICFilter = 0x00;                        //IC1F=0000 ���������˲��� ���˲�
    TIM_ICInit(TIM5, &TIM5_ICInitStructure);

    //�жϷ����ʼ��
    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;           //TIM5�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //��ռ���ȼ�2��
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        //�����ȼ�0��
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //IRQͨ����ʹ��
    NVIC_Init(&NVIC_InitStructure);                           //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

    TIM_ITConfig(TIM5, TIM_IT_Update | TIM_IT_CC3, ENABLE); //���������ж� ,����CC1IE�����ж�

    TIM_Cmd(TIM5, ENABLE); //ʹ�ܶ�ʱ��5
}

//��ʱ��5�жϷ������
/**********************************************************************************
* ��������: TIM5_IRQHandler
* ��������: tim5 ���벶���жϷ����ӳ���
* ��������: void input:
* �������: void output:
* ���� :author:@Kim_alittle_star
**********************************************************************************/
void TIM5_IRQHandler(void)
{

//#if SYSTEM_SUPPORT_OS == 1 //���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
//    OSIntEnter();
//#endif

    if ((TIM5CH3_CAPTURE_STA & 0X80) == 0) //��δ�ɹ�����
    {
        if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)

        {
            if (TIM5CH3_CAPTURE_STA & 0X40) //�Ѿ����񵽸ߵ�ƽ��
            {
                if ((TIM5CH3_CAPTURE_STA & 0X3F) == 0X3F) //�ߵ�ƽ̫����
                {
                    TIM5CH3_CAPTURE_STA |= 0X80; //��ǳɹ�������һ��
                    TIM5CH3_CAPTURE_VAL = 0XFFFF;
                }
                else
                    TIM5CH3_CAPTURE_STA++;
            }
        }
        if (TIM_GetITStatus(TIM5, TIM_IT_CC3) != RESET) //����1���������¼�
        {
            if (TIM5CH3_CAPTURE_STA & 0X40) //����һ���½���
            {
                TIM5CH3_CAPTURE_STA |= 0X80; //��ǳɹ�����һ��������
                TIM5CH3_CAPTURE_VAL = TIM_GetCapture3(TIM5);
                TIM_OC3PolarityConfig(TIM5, TIM_ICPolarity_Rising); //CC1P=0 ����Ϊ�����ز���
            }
            else //��δ��ʼ,��һ�β���������
            {
                TIM5CH3_CAPTURE_STA = 0; //���
                TIM5CH3_CAPTURE_VAL = 0;
                TIM_SetCounter(TIM5, 0);
                TIM5CH3_CAPTURE_STA |= 0X40;                         //��ǲ�����������
                TIM_OC3PolarityConfig(TIM5, TIM_ICPolarity_Falling); //CC1P=1 ����Ϊ�½��ز���
            }
        }
    }

    TIM_ClearITPendingBit(TIM5, TIM_IT_CC3 | TIM_IT_Update); //����жϱ�־λ
//#if SYSTEM_SUPPORT_OS == 1                                   //���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
//    OSIntExit();
//#endif
}