#include "UTDrive.h"

void UTDrive_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //使能GPIOA时钟
    GPIO_InitStructure.GPIO_Pin = UTCTRL_PIN;             
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(UTCTRL_PORT, &GPIO_InitStructure);
    UTCTRL_SET = 0;
    TIM5_Cap_Init(50000-1,72-1);


}

/**********************************************************************************
* 函数名称: UTDrive_getDistance
* 函数功能: 得到 测绘距离的函数 
* 函数输入: void input:
* 函数输出: void output:测绘距离是多少 单位 1mm
* 作者 :author:@Kim_alittle_star
**********************************************************************************/
u8 TIM5CH3_CAPTURE_STA = 0; //输入捕获状态
u16 TIM5CH3_CAPTURE_VAL;    //输入捕获值
TIM_ICInitTypeDef TIM5_ICInitStructure;
u16 period;
u32 UTDrive_getDistance(void)
{
    u32 temp = 0;
	TIM_Cmd(TIM5, DISABLE);
    if (TIM5CH3_CAPTURE_STA & 0X80) //成功捕获到了一次上升沿
    {
        
        temp = TIM5CH3_CAPTURE_STA & 0X3F;
        temp *= (period + 1);           //溢出时间总和
        temp += TIM5CH3_CAPTURE_VAL;    //得到总的高电平时间
//        DEBUG("HIGH:%d us\r\n", temp); //打印总的高点平时间
        TIM5CH3_CAPTURE_STA = 0;        //开启下一次捕获
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
* 函数名称: UTDrive_Measuring
* 函数功能: 开始测量距离 函数 
* 函数输入: void input:
* 函数输出: void output:
* 作者 :author:@Kim_alittle_star
**********************************************************************************/
void UTDrive_Measuring(void)
{
    TIM_Cmd(TIM5, ENABLE);
    UTCTRL_SET = 1;
    delay_us(15);
    UTCTRL_SET = 0;
}

/**********************************************************************************
* 函数名称: TIM5_Cap_Init
* 函数功能: 初始化 TIM5 的 输入捕获功能
* 函数输入: void input:arr 周期值 psc 分频值 
* 函数输出: void output:
* 作者 :author:@Kim_alittle_star
**********************************************************************************/

void TIM5_Cap_Init(u16 arr, u16 psc)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);  //使能TIM5时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //使能GPIOA时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;     //PA0 清除之前设置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0 输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA, GPIO_Pin_2); //PA0 下拉

    //初始化定时器5 TIM5
    TIM_TimeBaseStructure.TIM_Period = arr;                     //设定计数器自动重装值
    TIM_TimeBaseStructure.TIM_Prescaler = psc;                  //预分频器
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);             //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
    period = arr;
    //初始化TIM5输入捕获参数
    TIM5_ICInitStructure.TIM_Channel = TIM_Channel_3;                //CC1S=01 	选择输入端 IC1映射到TI1上
    TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;     //上升沿捕获
    TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
    TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;           //配置输入分频,不分频
    TIM5_ICInitStructure.TIM_ICFilter = 0x00;                        //IC1F=0000 配置输入滤波器 不滤波
    TIM_ICInit(TIM5, &TIM5_ICInitStructure);

    //中断分组初始化
    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;           //TIM5中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //先占优先级2级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        //从优先级0级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);                           //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

    TIM_ITConfig(TIM5, TIM_IT_Update | TIM_IT_CC3, ENABLE); //允许更新中断 ,允许CC1IE捕获中断

    TIM_Cmd(TIM5, ENABLE); //使能定时器5
}

//定时器5中断服务程序
/**********************************************************************************
* 函数名称: TIM5_IRQHandler
* 函数功能: tim5 输入捕获中断服务子程序
* 函数输入: void input:
* 函数输出: void output:
* 作者 :author:@Kim_alittle_star
**********************************************************************************/
void TIM5_IRQHandler(void)
{

//#if SYSTEM_SUPPORT_OS == 1 //如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
//    OSIntEnter();
//#endif

    if ((TIM5CH3_CAPTURE_STA & 0X80) == 0) //还未成功捕获
    {
        if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)

        {
            if (TIM5CH3_CAPTURE_STA & 0X40) //已经捕获到高电平了
            {
                if ((TIM5CH3_CAPTURE_STA & 0X3F) == 0X3F) //高电平太长了
                {
                    TIM5CH3_CAPTURE_STA |= 0X80; //标记成功捕获了一次
                    TIM5CH3_CAPTURE_VAL = 0XFFFF;
                }
                else
                    TIM5CH3_CAPTURE_STA++;
            }
        }
        if (TIM_GetITStatus(TIM5, TIM_IT_CC3) != RESET) //捕获1发生捕获事件
        {
            if (TIM5CH3_CAPTURE_STA & 0X40) //捕获到一个下降沿
            {
                TIM5CH3_CAPTURE_STA |= 0X80; //标记成功捕获到一次上升沿
                TIM5CH3_CAPTURE_VAL = TIM_GetCapture3(TIM5);
                TIM_OC3PolarityConfig(TIM5, TIM_ICPolarity_Rising); //CC1P=0 设置为上升沿捕获
            }
            else //还未开始,第一次捕获上升沿
            {
                TIM5CH3_CAPTURE_STA = 0; //清空
                TIM5CH3_CAPTURE_VAL = 0;
                TIM_SetCounter(TIM5, 0);
                TIM5CH3_CAPTURE_STA |= 0X40;                         //标记捕获到了上升沿
                TIM_OC3PolarityConfig(TIM5, TIM_ICPolarity_Falling); //CC1P=1 设置为下降沿捕获
            }
        }
    }

    TIM_ClearITPendingBit(TIM5, TIM_IT_CC3 | TIM_IT_Update); //清除中断标志位
//#if SYSTEM_SUPPORT_OS == 1                                   //如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
//    OSIntExit();
//#endif
}
