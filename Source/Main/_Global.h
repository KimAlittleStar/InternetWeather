#ifndef __GLOBAL_H_
#define __GLOBAL_H_
/*!< DEGUB of arm STM32  */
#ifndef USE_FULL_ASSERT
#define  USE_FULL_ASSERT

#define DEBUG_MODE 1


#endif



#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include "stdio.h"
#include "stdarg.h"
#include "string.h"
#include "malloc.h"
#include "stirng_Process.h"


#if DEBUG_MODE 
#define DEBUG(fmt,...) printf(fmt,##__VA_ARGS__)

#else
#define DEBUG(fmt,...) 

#endif


#endif
