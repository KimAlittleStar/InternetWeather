#ifndef _STIRNG_PROCESS_H_
#define _STIRNG_PROCESS_H_


#include "include.h"





u32 StrProcess_getNumFormStr(const u8* str,const u8*  key,u32 oldValue );
u8* StrProcess_StrReplace(u8* dest,const u8* oldstr,const u8* newstr);
u8 *StrProcess_gettime(const u8 *str, u8 *timebuff);

u8* StrProcess_NumtoString(u8* strbuff,u32 num,u8 lenth);
u16 StrProcess_UTF8toGBK(u8* c_utf8,u16 length);
u8* StrProcess_GetEntry(u8* str,u8* head,u8* sign);
int32_t StrProcess_Str2NUM(u8* str,u8 SignCtrl);



void DEBUG_PRINTF(const char* ftm,...);




#endif
