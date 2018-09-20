#include "stirng_Process.h"
#include "ff.h"
u8 *StrProcess_dat[7] = {
	"Sun", /*!< 星期日 */
	"Mon",
	"Tue",
	"Wed",
	"Thu",
	"Fri",
	"Sat"

};
u8 *StrProcess_mon[12] =
	{
		"Dec", /*!< 12月 */
		"Jan",
		"Feb",
		"Mar",
		"Apr",
		"May",
		"Jun",
		"Jul",
		"Aug",
		"Sep",
		"Oct",
		"Nov"

};
u32 StrProcess_getNumFormStr(const u8 *str, const u8 *key, u32 oldValue)
{

	u8 *pp;
	u32 const_p = NULL;
	pp = (u8 *)strstr(str, key);
	pp+= strlen(key);
	if (pp != NULL)
	{
		const_p = 0;
		//		pp = (String_Pro)str;
		while (*(pp) != '\0' && (*(pp) < '0' || *(pp) > '9'))
		{

			pp++;
		}
		while (*pp != '\0' && *pp >= '0' && *pp <= '9')
		{
			const_p = const_p * 10 + (*pp - '0');
			pp++;
		}
	}

	if (const_p == 0)
	{
		const_p = oldValue;
	}
	return const_p;
}

u8 *StrProcess_gettime(const u8 *str, u8 *timebuff)
{
	u8 i, day=0,month =0 ,hour = 0,min= 0,sec = 0,week = 0;
	u16 year = 0;
	u8 *ptemp;
	u8 strbuff[10];
	strcpy(timebuff, "YYYY-MM-DD HH:mm:ss Dat:X");
	if (strstr(str, "Date"))
	{
		for (i = 0; i < 7; i++)
		{ /*!< dat  */
			ptemp = strstr(str, StrProcess_dat[i]);
			if (ptemp != NULL)
			{
				week = i;
				break; /*!< 保存 星期几的变量 */
			}
		}
		/*!< 星期后面  就是 日期 所以现在 寻找日期  */
		ptemp = strstr(str,StrProcess_dat[i]);
		day = StrProcess_getNumFormStr(ptemp,StrProcess_dat[i],day);
		for (i = 0; i < 12; i++)
		{
			ptemp = strstr(str, StrProcess_mon[i]);
			if (ptemp != NULL)
			{
				month = i;
				break; /*!< 保存 月份的变量 */
			}
		}
		ptemp = strstr(str,StrProcess_mon[i]);
		year = StrProcess_getNumFormStr(ptemp,StrProcess_mon[i],year);

		
		StrProcess_NumtoString(strbuff,year,4);		
		hour = StrProcess_getNumFormStr(ptemp,strbuff,hour);
		ptemp = strstr(ptemp,strbuff);
		if(ptemp != NULL)
		ptemp+= strlen(strbuff);

		StrProcess_NumtoString(strbuff,hour,2);
		min = StrProcess_getNumFormStr(ptemp,strbuff,min);
		ptemp = strstr(ptemp,strbuff);
		if(ptemp != NULL)
		ptemp+= strlen(strbuff);

		StrProcess_NumtoString(strbuff,min,2);
		sec = StrProcess_getNumFormStr(ptemp,strbuff,sec);
		
		
		SYS_SEC = sec;
		SYS_MIN = min;
		SYS_HOUR = hour+8;
		SYS_MONTH = month;
		SYS_DAY =  day;	/* !< BUG 当天进位到 到月加加时 有BUG */
		SYS_YEAR = year;
		SYS_WEEK = week;
		
		TimeDrive_GMT_transform(SYS_HOUR);
//		sprintf(timebuff,"%04d-%02d-%02d %02d:%02d:%02d Dat:%01d",year,month,day,(hour+8)%24,min,sec,week);

	}

	return getSystemDateString(timebuff);
}


/**********************************************************************************
* 函数名称: StrProcess_StrReplace
* 函数功能: 替换字符串中的字符 
* 函数输入: void input:源字符串 dest 原有的字符，oldstr，新的字符 newstr
*						只是从oldstr 开始替换 不考虑后面的 内容 一般替换等长格式
* 函数输出: void output:替换后的源字符串指针 
* 作者 :author:@Kim_alittle_star
**********************************************************************************/
u8 *StrProcess_StrReplace(u8 *dest, const u8 *oldstr, const u8 *newstr)
{
	u8 *ptemp;
	u8 maxlen = 0;
	maxlen = (strlen(newstr)>strlen(oldstr))?(strlen(oldstr)):(strlen(newstr));
	ptemp = strstr(dest, oldstr);
	if (ptemp != NULL)
	{
		strncpy(ptemp, newstr, strlen(newstr));
	}
	return dest;
}

u8* StrProcess_NumtoString(u8* strbuff,u32 num,u8 lenth)
{
	strbuff[lenth] = '\0';
	for(;lenth>0;lenth--)
	{
		strbuff[lenth-1] = (num%10)+'0';
		num/=10;
	}
	
	return strbuff;
}

#if DEBUG_MODE
	u8 DEBUGBUFF[100];
#endif
void DEBUG_PRINTF(const char* ftm,...)
{
	u32 cpu_sr;
	u16 i,j;
#if SYSTEM_SUPPORT_OS == 1
	va_list ap; 
		 
	OS_ENTER_CRITICAL();
	va_start(ap,ftm);
	vsprintf((char*)DEBUGBUFF,ftm,ap);
	va_end(ap);
	printf(DEBUGBUFF);
	OS_ENTER_CRITICAL();
#else 
	va_list ap; 
	va_start(ap,ftm);
	vsprintf((char*)DEBUGBUFF,ftm,ap);
	va_end(ap);
	printf(DEBUGBUFF);
	
#endif
}


/**********************************************************************************
* 函数名称: StrProcess_UTF8toGBK
* 函数功能: 将网络上 utf8 的字符串转换为 GBK 字符串 使得兼容 原子LCD屏驱动程序
* 函数输入: void input: c_utf8 原utf8 字符串 同时 新的 gbk字符串会将其覆盖
*						length 你想要设置的 中间缓存块的大小 
* 函数输出: void output:字符总数 (一个中文算两个字节)
* 作者 :author:@Kim_alittle_star
* 文件依存: 
*	#include "ff.h"
*	#include "malloc.h"
* 	#include "stm32f10x.h"
**********************************************************************************/
u16 StrProcess_UTF8toGBK(u8* c_utf8,u16 length)
{
//	ff_uni2oem(,FF_CODE_PAGE);
	/* !< 首先 将 utf8 转换成标准 Unicode 然后查表将 Unicode 转化为GBK */
	u16 outputSize = 0; //记录转换后的gbk字符串长度
	u8 * pInput = c_utf8;
	u8* c_gbk = mymalloc(0,length);	 /* !< 申请内存,也可以外部传入一个 数据缓存空间 */
	u8* pOutput = c_gbk;
	u16* uni = (u16*)c_gbk;
	u16 gbk;
	/* !< 以下中间代码来自于 CSDN @bladeandmaster88 公开的源码 */
	while (*pInput)
	{
		if (*pInput > 0x00 && *pInput <= 0x7F) //处理单字节UTF8字符（英文字母、数字）
		{
			*pOutput = *pInput;
			 pOutput+= 1;
			*pOutput = 0; //小端法表示，在高地址填补0
		}
		else if (((*pInput) & 0xE0) == 0xC0) //处理双字节UTF8字符
		{
			char high = *pInput;
			pInput+=1;
			char low = *pInput;
			if ((low & 0xC0) != 0x80)  //检查是否为合法的UTF8字符表示
			{
				return -1; //如果不是则报错
			}
 
			*pOutput = (high << 6) + (low & 0x3F);
			pOutput++;
			*pOutput = (high >> 2) & 0x07;
		}
		else if (((*pInput) & 0xF0) == 0xE0) //处理三字节UTF8字符
		{
			char high = *pInput;
			pInput++;
			char middle = *pInput;
			pInput++;
			char low = *pInput;
			if (((middle & 0xC0) != 0x80) || ((low & 0xC0) != 0x80))
			{
				return -1;
			}
			*pOutput = (middle << 6) + (low & 0x3F);//取出middle的低两位与low的低6位，组合成unicode字符的低8位
			pOutput++;
			*pOutput = (high << 4) + ((middle >> 2) & 0x0F); //取出high的低四位与middle的中间四位，组合成unicode字符的高8位
		}
		else //对于其他字节数的UTF8字符不进行处理
		{
			return -1;
		}
		pInput++;//处理下一个utf8字符
		pOutput++;
	}
	//unicode字符串后面，有两个\0
	*pOutput = 0;
	 pOutput++;
	*pOutput = 0;
	/* !< 感谢 @bladeandmaster88 的开源支持 */
	pInput = c_utf8;
	while(*uni != 0)
	{
		gbk = ff_uni2oem(*uni,FF_CODE_PAGE);		/* !< Unicode 向 GBK 转换函数  */
		uni++;
		if(gbk&0xff00)
		{
			*pInput = ((gbk&0xff00)>>8);
			pInput++;
			*pInput = (gbk&0x00ff);
			pInput++;
			outputSize += 2;
		}else
		{
			*pInput = (gbk&0x00ff);
			pInput++;
			outputSize++;
		}
		
		
	}
	*pInput = '\0';			/* !< 加上结束符号 */
	myfree(0,c_gbk);		/* !< 释放内存 */
	return outputSize;

}


/**********************************************************************************
* 函数名称: StrProcess_GetEntry
* 函数功能: 从字符串中提取需要的数据 
*			EG: str = "loca":[234523];
*			"234523" = StrProcess_GetEntry(str,"loca","[]");
*			NULL = StrProcess_GetEntry(str,"loca","\"\"");	若是开始结束的标志相同,则遇到第二标志时就返回
*			NULL = StrProcess_GetEntry(str,"","\"\""); 空字符或 NULL 则从开始 搜索开始结束符 并返回中间的数据
* 函数输入: str 源字符串 head 元素字符串头  sing 字符串 开始结束标志 
* 函数输出: void output:元素字符串
* 作者 :author:@Kim_alittle_star
**********************************************************************************/
u8* StrProcess_GetEntry(u8* str,u8* head,u8* sign)
{/*!< Eg:head = " head "  sign = "{}"*/
	int8_t sublevel = 1;
	u8* ptemp;
	u8* pdata;
	u8*buff = mymalloc(0,sizeof(u8)*30);
	u8* retstr = mymalloc(0,sizeof(u8)*700);
	pdata = retstr;
	if(head == NULL || *head == '\0')
	{
		ptemp = str;
	}else
	{
		sprintf(buff,"\"%s\":",head);		/*!< buff = " "head": " */
		ptemp = strstr(str,buff);
		if(ptemp == NULL)
		{
			myfree(0,buff);
			myfree(0,retstr);
			return NULL;
		}
		ptemp +=strlen(buff);
		
	}
	
	while(*ptemp != *sign && *ptemp != '\0')
	{
		ptemp++;
	}
	if(*ptemp == '\0')
	{
		myfree(0,buff);
		myfree(0,retstr);
		return NULL;
	}
//	*pdata = *ptemp;
//	pdata++;
	ptemp++;
	while(sublevel != 0 && ptemp != '\0')
	{
		if(*ptemp == *sign && *sign != *(sign+1))
		{
			sublevel++;
		}else if(*ptemp == *(sign+1))
		{
			sublevel--;
		}
		*pdata = *ptemp; 
		pdata++;
		ptemp++;
	}
	
	myfree(0,buff);
	if(sublevel)
	{
		myfree(0,retstr);
		retstr = NULL;
	}else
	{
		*(pdata-1) = '\0';
	}
	return retstr;


}

int32_t StrProcess_Str2NUM(u8* str,u8 SignCtrl)
{
	int32_t ret = 0;
	int8_t sign = 1;
	while(*str != '\0')
	{
		if(SignCtrl == 0 && *str>='0' && *str<='9')
		{/*!< unsigned */
			break;

		}else if((*str>='0' && *str<='9' )||(*(str+1)>='0' && *(str+1)<='9') && (*str == '-' || *str == '+'))
		{
			break;
		}else
		{
			str++;
		}
	}
	if(*str == '-')
	{
		sign = -1;
		str++;
	}
	while(*str != '\0' && *str>='0' && *str<='9' )
	{
		ret = ret*10+(*str-'0');
		*str++;
	}
	ret *= sign;
	return ret;




}



