#define _USINTERFACE_GLOBALS_

#include "usfunc.h"
#include "usinterface.h"

//----------ʹ�÷���������صļ����ӿں�����������Ӧ��λ�ã�ʹ��printf���е����������----------//
//----------���ڵ������ֱ�������Ϊ���Իس����ͣ������͵��ַ���0x0d 0x0a����������ʹ��HTerm------//

_STR_T str;
/*
    �ӿں���
    �������ݽ��ճ���
    ���ڽ����ж��ڵ���
*/
void getSerialData(char sdata)
{
    #if CR_LF
	if((str.UsRxSta&STA_LF)==0)         //����δ���
	{
		if(str.UsRxSta&STA_CR)          //���յ���0x0d
		{
			if(sdata!=ASCII_LF)
                str.UsRxSta |= STA_ERR; //���ճ���
			else 
                str.UsRxSta |= STA_LF;  //��������� 
		}
		else if(sdata)
		{	
			if(sdata==ASCII_CR)
			{
                if(strlen(str.rcvStr))
                {//ֻ�н��յ���Ч�������ܽ��ս�����
                    str.UsRxSta |= STA_CR;
                }
			}
            else if(sdata!=ASCII_CR && sdata!=ASCII_LF)
			{
                if(str.pRcvStr<&str.rcvStr[W_LEN-1])
                {//����Խ��
                    *str.pRcvStr = sdata;
                    ++str.pRcvStr;
                }
                else
                    str.UsRxSta |= STA_ERR; //���ճ���
			}		 
		}
	} 
    #else
	if((str.UsRxSta&STA_CR)==0)         //����δ���
	{
		if(sdata==ASCII_CR)
		{
            if(strlen(str.rcvStr))
            {//ֻ�н��յ���Ч�������ܽ��ս�����
                str.UsRxSta |= STA_CR;
            }
		}
		else if(sdata)
		{
            if(str.pRcvStr<&str.rcvStr[W_LEN-1])
            {//����Խ��
                *str.pRcvStr = sdata;
                ++str.pRcvStr;
            }
            else
                str.UsRxSta |= STA_ERR; //���ճ���
		}		 
	}
    #endif
    
}


/*
    �ӿں���
    ��������������
    
*/
void StrProc(void)
{    
    #if CR_LF
    if(str.UsRxSta&STA_LF||str.UsRxSta&STA_ERR)
    #else
    if(str.UsRxSta&STA_CR||str.UsRxSta&STA_ERR)
    #endif
    {
        if(str.UsRxSta&STA_CR)
        {
            if(strlen(str.rcvStr))
            {
                str.ChRunFunc(str.rcvStr);
            }
            if(str.fUsIRF)
                printd("\r RCV:\"%s\"", str.rcvStr);
        }
        else
        {
            printd("\r Receive Err");
        }
        //���str���յ����ַ������ض���ָ��
        memset(str.rcvStr, '\0', sizeof(str.rcvStr));
        str.pRcvStr = str.rcvStr;
        str.UsRxSta = 0;
    }
    TimeOutInt();
}


void TimeOutInt(void)
{
    #if CR_LF
    if(strlen(str.rcvStr) && !(str.UsRxSta&STA_LF))
    #else
    if(strlen(str.rcvStr) && !(str.UsRxSta&STA_CR))
    #endif
    {
        if(++str.TimeOut>RCV_TIME_OUT)
        {
            str.TimeOut = 0;
            if(str.fUsIRF)
                printd("\r TimeOut:\"%s\"", str.rcvStr);
            memset(str.rcvStr, '\0', sizeof(str.rcvStr));
            str.pRcvStr = str.rcvStr;
            str.UsRxSta = 0;
        }
    }
    else
    {
        str.TimeOut = 0;
    }
}

/*
    ��������
*/
void BootInterface(void)
{//ָ��ָ��
    str.pRcvStr = str.rcvStr;
    str.ChRunFunc = ChRUN;
    printd("\r %s", BOOT_STR);
}

//-------------------------�ַ����ڴ洦����-------------------------//

/*
    ����תΪ�ַ���
*/
void int2str(int n, char *s)
{
    char buf[10]="";
    int i=0, len=0;
    int temp=(n<0)?(-n):(n);      // tempΪn�ľ���ֵ
    if(s==NULL)
        return;
    
    while(temp)
    {
        buf[i++] = (temp%10)+'0';   //��temp��ÿһλ�ϵ�������buf
        temp = temp/10;
    }

    len = (n<0)?(++i):(i);              //���n�Ǹ����������Ҫһλ���洢����
    s[i] = 0;                           //ĩβ�ǽ�����0
    while(1)
    {
        i--;
        if(buf[len-i-1]==0)
            break;
        
        s[i] = buf[len-i-1];            //��buf��������ַ������ַ���
    }
    if(i==0)
        s[i] = '-';                     //����Ǹ��������һ������
}


/*
    �ַ���תΪ����
    �������Ϊ�����ַ�
*/
int str2int(const char *s)
{
    int temp=0;
    const char *ptr=s;                    //ptr����str�ַ�����ͷ
    
    if (*s=='-' || *s=='+')             //�����һ���ַ������������Ƶ���һ���ַ�
        s++;
    
    while(*s!=0)
    {
        if((*s<'0') || (*s>'9'))       //�����ǰ�ַ���������
            break;
        
        temp = temp*10+(*s-'0');      //�����ǰ�ַ��������������ֵ
        s++;                                //�Ƶ���һ���ַ�
    }   
    if (*ptr=='-')                        //����ַ������ԡ�-����ͷ����ת�������෴��
        temp = -temp;
    
    return temp;
}


/*
    ����:���ָ�������ڵ�ֵ
    para: ����ָ��
    para: ���ֵ
    para: �����С��ʹ��sizeof()    
*/
void* memset(void* s, int c, size_t n)
{
    unsigned char* p = (unsigned char*)s;
    while (n > 0)
    {
        *p++ = (unsigned char)c;
        --n;
    }
    return s;
}

char tolower(char chr)
{	
    if ((chr >= 'A') && (chr <= 'Z'))		
        return chr + ('a' - 'A');	
    
    return chr;
} 

char toupper(char chr)
{	
    if ((chr >= 'a') && (chr <= 'z'))		
        return chr + ('A' - 'a');	
    
    return chr;
}


int strtohex(char *s)
{
    int intPara=0;
    while(*s!='\0')
    {
        *s = toupper(*s);
        intPara <<= 4;
        if(*s>='A' && *s<='F')
            intPara |= (10 + (*s-'A'));
        else
            intPara |= (*s-'0');
        
        s++;
    }
    return intPara;
}


char* myStrncpy(char *des, char *src, size_t n)
{
    strncpy(des, src, n-1);
    *(des+n-1) = '\0';
    return des;
}


/*
    @cmdlen     �����
    @paracnt    �������   ֵΪ0ʱ���Ը������м��
    @*para      ����
    @����ֵ     ���ش������
    1: ���������������쳣���
    2: ���������붨�岻��
    3: ������������
*/
unsigned char FetchInt(unsigned char cmdlen, unsigned char paracnt, char *srcStr, int *para)
{
    char chSep, *p, cpyBuf[W_LEN+1], rngBuf[P_LEN+1];
    unsigned short cnt=0, bfCnt=0, strCnt=0;
    strncpy(&chSep, (srcStr+cmdlen), 1);                      //ȡ����������ķָ���
    if(chSep==C_EQUAL)
    {
        strncpy(cpyBuf, (srcStr+cmdlen+1), W_LEN);            //ȡ�����ַ���
        *(cpyBuf+W_LEN) = C_TERM;                           //�ֶ���ӽ�����
        p = cpyBuf;
        for(cnt=0; cnt<=W_LEN; cnt++)
        {//��ȡ����
            if(*p==C_SEP_1 || *p==C_SEP_2)
            {//�жϷָ���
                if((cnt-bfCnt) && (cnt-bfCnt)<=P_LEN)         // paralen���ַ�
                {//�ж���������
                    strncpy(rngBuf, cpyBuf+bfCnt, cnt-bfCnt);  //ȡ�����ַ���
                    rngBuf[cnt-bfCnt] = C_TERM;
                    bfCnt = cnt+1;
                    if(paracnt)
                    {
                        if(strCnt<paracnt)
                            *(para+strCnt) = str2int(rngBuf);
                        else
                            return 3;
                    }
                    else
                    {
                        *(para+strCnt) = str2int(rngBuf);
                    }
                    ++strCnt;       //�ڼ�������
                }
                else//���������붨�岻��
                    return 2;
            }
            if(*p++==C_TERM)
            {//�жϽ�����
             //���������Ҫ������ȫ����Ȼ����ע�͵�����ֻ����ǰ�漸��
                if((cnt-bfCnt) && (cnt-bfCnt)<=P_LEN)    // paralen���ַ�
                {//�ж���������,�������
                    strncpy(rngBuf, cpyBuf+bfCnt, cnt-bfCnt);
                    rngBuf[cnt-bfCnt] = C_TERM;
                    if(paracnt)
                    {
                        if(strCnt<paracnt)
                        {
                            *(para+strCnt) = str2int(rngBuf);
                        }
        				++strCnt;       //�ڼ�������
        				if(strCnt!=paracnt)
        				{//���������붨�岻��
                            return 3;
        				}
                    }
                    else
                    {
                        *(para+strCnt) = str2int(rngBuf);
                    }
                    break;
                }
                else//���������붨�岻��
                    return 2;
            }
        }
    }
    else    //���������������쳣���
    {
        return 1;
    }
    return 0;
}


/*
    @cmdlen     �����
    @paracnt    �������   ֵΪ0ʱ���Ը������м��
    @*para      �������
    @����ֵ     ���ش������
    1: ���������������쳣���
    2: ���������붨�岻��
    3: ������������
*/
unsigned char FetchChar(unsigned char cmdlen, unsigned char paracnt, char *srcStr, char *para)
{//strncpy���������ַ�����ȡ��ʱ�򣬲��������������������Ҫ�ֶ���ӣ����������洢��ʱ����Ҫ��һ���ֽ�
    char chSep, *p, cpyBuf[W_LEN+1], rngBuf[P_LEN+1];
    unsigned short cnt=0, bfCnt=0, strCnt=0;
    strncpy(&chSep, (srcStr+cmdlen), 1);                //ȡ����������ķָ���
    if(chSep==C_EQUAL)
    {
        strncpy(cpyBuf, (srcStr+cmdlen+1), W_LEN);        //ȡ�����ַ���
        *(cpyBuf+W_LEN) = C_TERM;                           //�ֶ���ӽ�����
        p = cpyBuf;
        for(cnt=0; cnt<=W_LEN; cnt++)
        {//��ȡ����
            if(*p==C_SEP_1 || *p==C_SEP_2)
            {//�жϷָ���
                if((cnt-bfCnt) && (cnt-bfCnt)<=P_LEN)         // paralen���ַ�
                {//�ж���������
                    strncpy(rngBuf, cpyBuf+bfCnt, cnt-bfCnt);   //ȡ�����ַ���
                    rngBuf[cnt-bfCnt] = C_TERM;
                    bfCnt = cnt+1;
                    if(paracnt)
                    {
                        if(strCnt<paracnt)
                            strcpy((para+strCnt*(P_LEN+1)), rngBuf);
                        else
                            return 3;
                    }
                    else
                    {
                        strcpy((para+strCnt*(P_LEN+1)), rngBuf);
                    }
                    ++strCnt;                                   //�ڼ�������
                }
                else    //���������붨�岻��
                    return 2;
            }
            if(*p++==C_TERM)
            {//�жϽ�����
            //���������Ҫ������ȫ����Ȼ����ע�͵�����ֻ����ǰ�漸��
                if((cnt-bfCnt) && (cnt-bfCnt)<=P_LEN)      // paralen-1���ַ�
                {//�ж���������,�������
                    strncpy(rngBuf, cpyBuf+bfCnt, cnt-bfCnt);
                    rngBuf[cnt-bfCnt] = C_TERM;
                    if(paracnt)
                    {
                        if(strCnt<paracnt)
                        {
                            strcpy((para+strCnt*(P_LEN+1)), rngBuf);
                        }
        				++strCnt;       //�ڼ�������
        				if(strCnt!=paracnt)
        				{//���������붨�岻��
                            return 3;
        				}
                    }
                    else
                    {
                        strcpy((para+strCnt*(P_LEN+1)), rngBuf);
                    }
                    break;
                }
                else    //���������붨�岻��
                    return 2;
            }
        }
    }
    else    //���������������쳣���
    {
        return 1;
    }
    return 0;
}

/*
    @cmdlen     �����
    @paracnt    �������   ֵΪ0ʱ���Ը������м��
    @*para      ����
    @����ֵ     ���ش������
    1: ���������������쳣���
    2: ���������붨�岻��
    3: ������������
*/
unsigned char UnEqFetchInt(unsigned char cmdlen, unsigned char paracnt, char *srcStr, int *para)
{
    char *p, cpyBuf[W_LEN+1], rngBuf[P_LEN+1];
    unsigned short cnt=0, bfCnt=0, strCnt=0;
	strncpy(cpyBuf, (srcStr+cmdlen), W_LEN);        //ȡ�����ַ���
	*(cpyBuf+W_LEN) = C_TERM;                           //�ֶ���ӽ�����
	p = cpyBuf;
	for(cnt=0; cnt<=W_LEN; cnt++)
	{//��ȡ����
        if(*p==C_SEP_1 || *p==C_SEP_2)
		{//�жϷָ���
			if((cnt-bfCnt) && (cnt-bfCnt)<=P_LEN)         // paralen���ַ�
			{//�ж���������
				strncpy(rngBuf, cpyBuf+bfCnt, cnt-bfCnt);   //ȡ�����ַ���
				rngBuf[cnt-bfCnt] = C_TERM;
				bfCnt = cnt+1;
                if(paracnt)
                {
                    if(strCnt<paracnt)
                        *(para+strCnt) = str2int(rngBuf);
                    else
                        return 3;
                }
                else
                {
                    *(para+strCnt) = str2int(rngBuf);
                }
				++strCnt;       //�ڼ�������
			}
			else//���������붨�岻��
				return 2;
		}
		if(*p++==C_TERM)
		{//�жϽ�����
		//���������Ҫ������ȫ����Ȼ����ע�͵�����ֻ����ǰ�漸��
			if((cnt-bfCnt) && (cnt-bfCnt)<=P_LEN)    // paralen���ַ�
			{//�ж���������,�������
				strncpy(rngBuf, cpyBuf+bfCnt, cnt-bfCnt);
				rngBuf[cnt-bfCnt] = C_TERM;
                if(paracnt)
                {
                    if(strCnt<paracnt)
                    {
                        *(para+strCnt) = str2int(rngBuf);
                    }
    				++strCnt;       //�ڼ�������
    				if(strCnt!=paracnt)
    				{//���������붨�岻��
                        return 3;
    				}
                }
                else
                {
                    *(para+strCnt) = str2int(rngBuf);
                }
				break;
			}
			else//���������붨�岻��
				return 2;
		}
	}
    return 0;
}


/*
    @cmdlen     �����
    @paracnt    �������   ֵΪ0ʱ���Ը������м��
    @*para      �������
    @����ֵ     ���ش������
    1: ���������������쳣���
    2: ���������붨�岻��
    3: ������������
*/
unsigned char UnEqFetchChar(unsigned char cmdlen, unsigned char paracnt, char *srcStr, char *para)
{//strncpy���������ַ�����ȡ��ʱ�򣬲��������������������Ҫ�ֶ���ӣ����������洢��ʱ����Ҫ��һ���ֽ�
    char *p, cpyBuf[W_LEN+1], rngBuf[P_LEN+1];
    unsigned short cnt=0, bfCnt=0, strCnt=0;
	strncpy(cpyBuf, (srcStr+cmdlen), W_LEN);              //ȡ�����ַ���
	*(cpyBuf+W_LEN) = C_TERM;                           //�ֶ���ӽ�����
	p = cpyBuf;
	for(cnt=0; cnt<=W_LEN; cnt++)
	{//��ȡ����
        if(*p==C_SEP_1 || *p==C_SEP_2)
		{//�жϷָ���
			if((cnt-bfCnt) && (cnt-bfCnt)<=P_LEN)           // paralen���ַ�
			{//�ж���������
				strncpy(rngBuf, cpyBuf+bfCnt, cnt-bfCnt);   //ȡ�����ַ���
				rngBuf[cnt-bfCnt] = C_TERM;
				strcpy((para+strCnt*(P_LEN+1)), rngBuf);
				bfCnt = cnt+1;
                if(paracnt)
                {
                    if(strCnt<paracnt)
                        strcpy((para+strCnt*(P_LEN+1)), rngBuf);
                    else
                        return 3;
                }
                else
                {
                    strcpy((para+strCnt*(P_LEN+1)), rngBuf);
                }
				++strCnt;                                   //�ڼ�������
			}
			else    //���������붨�岻��
				return 2;
		}
		if(*p++==C_TERM)
		{//�жϽ�����
    		//���������Ҫ������ȫ����Ȼ����ע�͵�����ֻ����ǰ�漸��
			if((cnt-bfCnt) && (cnt-bfCnt)<=P_LEN)           // paralen-1���ַ�
			{//�ж���������,�������
				strncpy(rngBuf, cpyBuf+bfCnt, cnt-bfCnt);
				rngBuf[cnt-bfCnt] = C_TERM;
                if(paracnt)
                {
                    if(strCnt<paracnt)
                    {
                        strcpy((para+strCnt*(P_LEN+1)), rngBuf);
                    }
    				++strCnt;       //�ڼ�������
    				if(strCnt!=paracnt)
    				{//���������붨�岻��
                        return 3;
    				}
                }
                else
                {
                    strcpy((para+strCnt*(P_LEN+1)), rngBuf);
                }
				break;
			}
			else    //���������붨�岻��
				return 2;
		}
	}
    return 0;
}



