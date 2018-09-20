#include "TimerDriver.h"

vu16 SYS_YEAR = 2009;
vu8 SYS_MONTH = 12;
vu8 SYS_DAY = 31;
vu8 SYS_HOUR = 23;
vu8 SYS_MIN = 59;
vu8 SYS_SEC = 49;
vu8 SYS_WEEK = 4;

u8 yearof1_day[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30,31}; /*!< 0��Ԫ��Ϊ 12�µ�����  ƽ��*/
u8 yearof2_day[13] = {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30,31}; /*!< 0��Ԫ��Ϊ 12�µ�����  ����*/

void TimerDrive_init_ms(u16 arr, u16 psc)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); //TIM6ʱ��ʹ��

    //��ʱ��TIM6��ʼ��
    TIM_TimeBaseStructure.TIM_Period = arr;                     //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TIM_Prescaler = psc;                  //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);             //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE); //ʹ��ָ����TIM6�ж�,��������ж�

    TIM_Cmd(TIM6, ENABLE); //������ʱ��7

    NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //��ռ���ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;        //�����ȼ�2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);                           //����ָ���Ĳ�����ʼ��VIC�Ĵ���
    TIM_Cmd(TIM6,ENABLE);
}
//��ʱ��6�жϷ������
void TIM6_IRQHandler(void)
{
#if SYSTEM_SUPPORT_OS == 1 //���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
    OSIntEnter();
#endif
    if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET) //�Ǹ����ж�
    {
        TIM_ClearFlag(TIM6, TIM_FLAG_Update);
        SYS_SEC++;
        if (SYS_SEC >= 60)
        {
			
            SYS_SEC = 0;
            SYS_MIN++;
            if (SYS_MIN >= 60)
            {
                SYS_MIN = 0;
                SYS_HOUR++;
                if (SYS_HOUR >= 24)
                {
                    SYS_HOUR %= 24;
                    SYS_DAY++;
                    if ((SYS_YEAR % 400) == 0 || ((SYS_YEAR % 100) != 0 && (SYS_YEAR % 4) == 0))
                    { /*!< ���� 2000 */
                        if (SYS_DAY > yearof2_day[SYS_MONTH])
                        {
                            SYS_DAY = 1;
                            SYS_MONTH++;
                            if (SYS_MONTH >= 12)
                            {
                                SYS_YEAR++;
                            }
                        }
                    }
                    else
                    { /*!< ƽ�� */
                        if (SYS_DAY > yearof1_day[SYS_MONTH])
                        {
                            SYS_DAY = 1;
                            SYS_MONTH++;
                            if (SYS_MONTH > 12)
                            {
								SYS_MONTH = 1;
                                SYS_YEAR++;
                            }
                        }
                    }
                    SYS_WEEK = (SYS_WEEK + 1) % 7;
                }
            }
        }
    }
#if SYSTEM_SUPPORT_OS == 1 //���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
    OSIntExit();
#endif
}



void TimeDrive_GMT_transform(u8 hour)
{
    hour = SYS_HOUR;
    if (SYS_HOUR >= 24)
    {
        SYS_HOUR %= 24;
        SYS_DAY++;
        if ((SYS_YEAR % 400) == 0 || ((SYS_YEAR % 100) != 0 && (SYS_YEAR % 4) == 0))
        { /*!< ���� 2000 */
            if (SYS_DAY > yearof2_day[SYS_MONTH])
            {
                SYS_DAY = 1;
                SYS_MONTH++;
                if (SYS_MONTH >= 12)
                {
                    SYS_YEAR++;
                }
            }
        }
        else
        { /*!< ƽ�� */
            if (SYS_DAY > yearof1_day[SYS_MONTH])
            {
                SYS_DAY = 1;
                SYS_MONTH++;
                if (SYS_MONTH > 12)
                {
                    SYS_MONTH = 1;
                    SYS_YEAR++;
                }
            }
        }
        SYS_WEEK = (SYS_WEEK + 1) % 7;
    }
}



u32 getSystemYear(void)
{
    return SYS_YEAR;
}

u8 getSystemMonth(void)
{
    return SYS_MONTH;
}

u8 getSystemDay(void)
{
    return SYS_DAY;
}

u8 getSystemHour(void)
{
    return SYS_HOUR;
}

u8 getSystemMin(void)
{
    return SYS_MIN;
}
u8 getSystemSec(void)
{
    return SYS_SEC;
}
u8 getSystemWeek(void)
{
    return (SYS_WEEK== 0)?(7):(SYS_WEEK);
}
u8* getSystemDateString(u8* datebuff)
{

    sprintf(datebuff,"%04d-%02d-%02d %02d:%02d:%02d Dat:%01d",SYS_YEAR,SYS_MONTH,SYS_DAY,SYS_HOUR,SYS_MIN,SYS_SEC,(SYS_WEEK== 0)?(7):(SYS_WEEK));
    return datebuff;
}

