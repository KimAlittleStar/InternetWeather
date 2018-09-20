/******************************************************************************************************
* 文件名: keyscan.c
* 所在文件夹: Src
* 文件关联: KeyScan.h 实现按键输入 返回值
* 文件编辑者: kim_alittel_star
*******************************************************************************************************/
#include "KeyScan.h"

/**********************************************************************************
* 函数名称: key_init(void)
* 函数功能: 初始化按键设置
* 函数输入: void input:
* 函数输出: void output:
* 作者 :author:@Kim_alittle_star
**********************************************************************************/
void key_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_KEY,ENABLE);
    /*!< KEY 1 init */
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Pin = KEY1_PIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KEY1_Port,& GPIO_InitStruct);
    /*!< KEY 2 init */
    
    GPIO_InitStruct.GPIO_Pin = KEY2_PIN;
    GPIO_Init(KEY2_Port,&GPIO_InitStruct);

    /*!< KEY3 init */

    GPIO_InitStruct.GPIO_Pin = KEY3_PIN;
    GPIO_Init(KEY3_Port,&GPIO_InitStruct);

    /*!< KEY4 init */
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStruct.GPIO_Pin = KEY4_PIN;
    GPIO_Init(KEY4_Port,&GPIO_InitStruct);
	
	/* !< BEEP init */
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = BEEP_PIN;
	 GPIO_Init(BEEP_Port,&GPIO_InitStruct);
	

    
}

/**********************************************************************************
* 函数名称: key_scan(SCAN_MODE)
* 函数功能: void
* 函数输入: SCAN_MODE 扫描模式 连续扫描模式 或者 单次扫描模式
* 函数输出: KEYValue output: 输出 键盘值
* 作者 :author:@Kim_alittle_star
**********************************************************************************/
u8 key_scan(u8 SCAN_MODE)
{
    static u8 KEY_PRESS_STA = 0;    /*!< 0 未按下 1 已按下 */
    u8 keyvalue = 0x00;
    if(SCAN_MODE == KEY_SCAN_MODE_REPLEACE)
    {
        KEY_PRESS_STA = 0;
    }
    if((KEY1_Port->IDR & KEY1_PIN) == 0)   keyvalue |= KEY1;
    if((KEY2_Port->IDR & KEY2_PIN) == 0)  keyvalue |= KEY2;
    if((KEY3_Port->IDR & KEY3_PIN) == 0)   keyvalue |= KEY3;
    if(KEY4_Port->IDR & KEY4_PIN)   keyvalue |= KEY4;
    delay_us(10000);
    keyvalue = 0;
   if((KEY1_Port->IDR & KEY1_PIN) == 0)   keyvalue |= KEY1;
    if((KEY2_Port->IDR & KEY2_PIN) == 0)  keyvalue |= KEY2;
    if((KEY3_Port->IDR & KEY3_PIN) == 0)   keyvalue |= KEY3;
    if(KEY4_Port->IDR & KEY4_PIN)   keyvalue |= KEY4;
    if(keyvalue == 0)
    {
        KEY_PRESS_STA = 0;

    }else
    {
        if(KEY_PRESS_STA != 0)
        {
            keyvalue = 0;   
        }
        KEY_PRESS_STA = 1;
    }
    return keyvalue;
    
}

 
