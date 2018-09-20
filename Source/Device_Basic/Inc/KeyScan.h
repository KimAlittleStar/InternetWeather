#ifndef _KEYSCAN_H_
#define  _KEYSCAN_H_
#include "_Global.h"

#define KEY_SCAN_MODE_ONCE 0
#define KEY_SCAN_MODE_REPLEACE 1


#ifdef __STM32F10x_H
/*!< 如果是在 拥有STLib的情况下 */
#define RCC_APB2Periph_KEY RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOB

#define KEY1 0x01
#define KEY1_Port GPIOE
#define KEY1_PIN GPIO_Pin_4

#define KEY2 0x02
#define KEY2_Port GPIOE
#define KEY2_PIN GPIO_Pin_3

#define KEY3 0x04
#define KEY3_Port GPIOE
#define KEY3_PIN GPIO_Pin_6

#define KEY4 0x08
#define KEY4_Port GPIOA
#define KEY4_PIN GPIO_Pin_0

#define BEEP PBout(8)	// BEEP,蜂鸣器接口		   
#define BEEP_Port GPIOB
#define BEEP_PIN GPIO_Pin_8
  
#endif

u8 key_scan(u8 SCAN_MODE);
void key_init(void);



#endif
