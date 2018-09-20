#ifndef _TIMERDRIVER_H_
#define _TIMERDRIVER_H_

#include "_Global.h"
#include "sys.h"

extern vu16 SYS_YEAR;
extern vu8 SYS_MONTH;
extern vu8 SYS_DAY;
extern vu8 SYS_HOUR;
extern vu8 SYS_MIN;
extern vu8 SYS_SEC;
extern vu8 SYS_WEEK;

void TimerDrive_init_ms(u16 arr, u16 psc);
u8 *getSystemDateString(u8 *datebuff);
u8 getSystemWeek(void);
u8 getSystemSec(void);
u8 getSystemMin(void);
u8 getSystemHour(void);
u8 getSystemDay(void);
u8 getSystemMonth(void);
u32 getSystemYear(void);

void TimeDrive_GMT_transform(u8 hour);

#endif
