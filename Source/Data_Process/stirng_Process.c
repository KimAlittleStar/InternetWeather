#include "stirng_Process.h"
#include "ff.h"
u8 *StrProcess_dat[7] = {
	"Sun", /*!< ������ */
	"Mon",
	"Tue",
	"Wed",
	"Thu",
	"Fri",
	"Sat"

};
u8 *StrProcess_mon[12] =
	{
		"Dec", /*!< 12�� */
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
				break; /*!< ���� ���ڼ��ı��� */
			}
		}
		/*!< ���ں���  ���� ���� �������� Ѱ������  */
		ptemp = strstr(str,StrProcess_dat[i]);
		day = StrProcess_getNumFormStr(ptemp,StrProcess_dat[i],day);
		for (i = 0; i < 12; i++)
		{
			ptemp = strstr(str, StrProcess_mon[i]);
			if (ptemp != NULL)
			{
				month = i;
				break; /*!< ���� �·ݵı��� */
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
		SYS_DAY =  day;	/* !< BUG �����λ�� ���¼Ӽ�ʱ ��BUG */
		SYS_YEAR = year;
		SYS_WEEK = week;
		
		TimeDrive_GMT_transform(SYS_HOUR);
//		sprintf(timebuff,"%04d-%02d-%02d %02d:%02d:%02d Dat:%01d",year,month,day,(hour+8)%24,min,sec,week);

	}

	return getSystemDateString(timebuff);
}


/**********************************************************************************
* ��������: StrProcess_StrReplace
* ��������: �滻�ַ����е��ַ� 
* ��������: void input:Դ�ַ��� dest ԭ�е��ַ���oldstr���µ��ַ� newstr
*						ֻ�Ǵ�oldstr ��ʼ�滻 �����Ǻ���� ���� һ���滻�ȳ���ʽ
* �������: void output:�滻���Դ�ַ���ָ�� 
* ���� :author:@Kim_alittle_star
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
* ��������: StrProcess_UTF8toGBK
* ��������: �������� utf8 ���ַ���ת��Ϊ GBK �ַ��� ʹ�ü��� ԭ��LCD����������
* ��������: void input: c_utf8 ԭutf8 �ַ��� ͬʱ �µ� gbk�ַ����Ὣ�串��
*						length ����Ҫ���õ� �м仺���Ĵ�С 
* �������: void output:�ַ����� (һ�������������ֽ�)
* ���� :author:@Kim_alittle_star
* �ļ�����: 
*	#include "ff.h"
*	#include "malloc.h"
* 	#include "stm32f10x.h"
**********************************************************************************/
u16 StrProcess_UTF8toGBK(u8* c_utf8,u16 length)
{
//	ff_uni2oem(,FF_CODE_PAGE);
	/* !< ���� �� utf8 ת���ɱ�׼ Unicode Ȼ���� Unicode ת��ΪGBK */
	u16 outputSize = 0; //��¼ת�����gbk�ַ�������
	u8 * pInput = c_utf8;
	u8* c_gbk = mymalloc(0,length);	 /* !< �����ڴ�,Ҳ�����ⲿ����һ�� ���ݻ���ռ� */
	u8* pOutput = c_gbk;
	u16* uni = (u16*)c_gbk;
	u16 gbk;
	/* !< �����м���������� CSDN @bladeandmaster88 ������Դ�� */
	while (*pInput)
	{
		if (*pInput > 0x00 && *pInput <= 0x7F) //�����ֽ�UTF8�ַ���Ӣ����ĸ�����֣�
		{
			*pOutput = *pInput;
			 pOutput+= 1;
			*pOutput = 0; //С�˷���ʾ���ڸߵ�ַ�0
		}
		else if (((*pInput) & 0xE0) == 0xC0) //����˫�ֽ�UTF8�ַ�
		{
			char high = *pInput;
			pInput+=1;
			char low = *pInput;
			if ((low & 0xC0) != 0x80)  //����Ƿ�Ϊ�Ϸ���UTF8�ַ���ʾ
			{
				return -1; //��������򱨴�
			}
 
			*pOutput = (high << 6) + (low & 0x3F);
			pOutput++;
			*pOutput = (high >> 2) & 0x07;
		}
		else if (((*pInput) & 0xF0) == 0xE0) //�������ֽ�UTF8�ַ�
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
			*pOutput = (middle << 6) + (low & 0x3F);//ȡ��middle�ĵ���λ��low�ĵ�6λ����ϳ�unicode�ַ��ĵ�8λ
			pOutput++;
			*pOutput = (high << 4) + ((middle >> 2) & 0x0F); //ȡ��high�ĵ���λ��middle���м���λ����ϳ�unicode�ַ��ĸ�8λ
		}
		else //���������ֽ�����UTF8�ַ������д���
		{
			return -1;
		}
		pInput++;//������һ��utf8�ַ�
		pOutput++;
	}
	//unicode�ַ������棬������\0
	*pOutput = 0;
	 pOutput++;
	*pOutput = 0;
	/* !< ��л @bladeandmaster88 �Ŀ�Դ֧�� */
	pInput = c_utf8;
	while(*uni != 0)
	{
		gbk = ff_uni2oem(*uni,FF_CODE_PAGE);		/* !< Unicode �� GBK ת������  */
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
	*pInput = '\0';			/* !< ���Ͻ������� */
	myfree(0,c_gbk);		/* !< �ͷ��ڴ� */
	return outputSize;

}


/**********************************************************************************
* ��������: StrProcess_GetEntry
* ��������: ���ַ�������ȡ��Ҫ������ 
*			EG: str = "loca":[234523];
*			"234523" = StrProcess_GetEntry(str,"loca","[]");
*			NULL = StrProcess_GetEntry(str,"loca","\"\"");	���ǿ�ʼ�����ı�־��ͬ,�������ڶ���־ʱ�ͷ���
*			NULL = StrProcess_GetEntry(str,"","\"\""); ���ַ��� NULL ��ӿ�ʼ ������ʼ������ �������м������
* ��������: str Դ�ַ��� head Ԫ���ַ���ͷ  sing �ַ��� ��ʼ������־ 
* �������: void output:Ԫ���ַ���
* ���� :author:@Kim_alittle_star
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



