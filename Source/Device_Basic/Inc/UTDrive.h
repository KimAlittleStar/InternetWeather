#ifndef _TUDRIVE_H_
#define _TUDRIVE_H_

#include "_Global.h"

#define UTCTRL_PORT GPIOA
#define UTCTRL_PIN GPIO_Pin_3
#define UTCTRL_SET (PAout(3))

void UTDrive_init(void);
u32 UTDrive_getDistance(void);
void UTDrive_Measuring(void);


void TIM5_Cap_Init(u16 arr,u16 psc);



#endif
