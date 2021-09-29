#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "type.h"
#include "log.h"
#include "GE_SRTP.h"
extern ADDR_U16_U32 addr_u16_u32[];
extern ADDR_BIT addr_bit[];
extern ADDR_STRING addr_string[];
static char result[MQTT_MSG_MAX_LEN-40] = {0};

char ABC_Digit[62]={"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"};

char* Lx_itoaEx(int value, char* str, int radix) {
    static char dig[] ="0123456789abcdefghijklmnopqrstuvwxyz";
    int n = 0, neg = 0;
    unsigned int v;
    char* p, *q;
    char c;
	zlg_debug("value is %d\r\n",value);
    if (radix == 10 && value < 0) {
        value = -value;
        neg = 1;
    }
    v = value;
    do {
        str[n++] = dig[v%radix];
        v /= radix;
    } while (v);
    if (neg)
        str[n++] = '-';
    str[n] = '\0';
    for (p = str, q = p + (n-1); p < q; ++p, --q)
        c = *p, *p = *q, *q = c;
    return str;
}

unsigned char Inquire_letter(unsigned char letter)
{
	if(letter>=48&&letter<=57)
		return 1;
	else if(letter>=65&&letter<=90)
		return 2;
	else if(letter>=97&&letter<=120)
		return 3;
	else 
		return 0;
}
char ASCII(unsigned char letter)
{
	char ABC;
	switch (Inquire_letter(letter))
	{
		case 1:
			ABC = ABC_Digit[letter-0x30];
			break;
		case 2:
			ABC = ABC_Digit[letter-0x37];
			break;
		case 3:
			ABC = ABC_Digit[letter-0x3D];
		default:
			return 0;
	}
	return ABC;
}

void resolve_lotname(unsigned char * HEXLotName,char * Lotname)
{
		char * p = Lotname;
        unsigned char ii = 0;
		for(ii = 0;ii<8;ii++)
		{
			if(ASCII(HEXLotName[ii])!=0)
			{
				*p=ASCII(HEXLotName[ii]);
				p++;
			}		
		}
}

char *lotnameRevData(char *hexlotname,unsigned char * data,unsigned char len)
{
	unsigned char hexname[8];
	memcpy(hexname,data,len);
	resolve_lotname(hexname,hexlotname);
	return hexlotname;
}

//将Hex字符转换成无符号整型
unsigned int hex2int(char c)
{
    if( c >= '0' && c <= '9')
    {
        return (unsigned int) (c -48);
    }
    else if( c >= 'A' && c <= 'F')
    {
        return (unsigned int) (c -55);
    }
    else if( c >= 'a' && c <= 'f')
    {
        return (unsigned int) (c - 87);
    }
    else
    {
        return 0;
    }
}
unsigned char* getRevDataCnt(unsigned char* data,unsigned int cnt)
{
	unsigned short ucount;
	ucount = (unsigned short)((hex2int(data[0])<<12) | (hex2int(data[1])<<8) | (hex2int(data[2])<<4) | (hex2int(data[3])<<0));
	data++;
	data++;
	data++;
	data++;
	printf("%s\r\n",data);
	if(ucount+18 == cnt)
	{
		return data;
	}
	else
	{
		printf("ucount:%d  cnt:%d \r\n",ucount,cnt);
		return 0;
	}
}

unsigned char* isOverRevData(unsigned char* data)
{
	unsigned char ret;
	if(*data++ == '0' && *data++ == '0' && *data++ == '0' && *data++ == '0')
		ret = 1;
	if (ret == 1) 
	return data;
	else
	return 0;
}


unsigned char * AnalyzeHalfwordtatusData(unsigned char* data,unsigned short *data16)
{
	
	*data16=(unsigned short)((hex2int(data[0])<<12) | (hex2int(data[1])<<8) | (hex2int(data[2])<<4) | (hex2int(data[3])<<0));
	data=data+4;
	return data;
}
unsigned char * AnalyzeRevData(unsigned char* data,unsigned int *data32)
{
	unsigned short tp1 = 0;
	unsigned short tp2 = 0;
	data = AnalyzeHalfwordtatusData(data,&tp1);
	data = AnalyzeHalfwordtatusData(data,&tp2);
	*data32=((unsigned int)tp1<<16 | (unsigned int)tp2);
	return data;
}

static void analyze_u16_data(char* data,unsigned char index,unsigned char func_code)
{
	char strval[50] = {0};
	int j = 0;
	short tmp = 0;
	//if((int)get_b(index)*2*8 == (u16)((u16)data[23]<<8|data[24]))//判断长度是否一致
	//{
		for(j=0 ; j<(int)get_b(index) ; j++)
		{								
			if(data[4]==0)
			{
				tmp = data[45+j*2];
				tmp = tmp<<8;
				tmp |= data[44+j*2];      
			}
			else
			{
				tmp = data[57+j*2];
				tmp = tmp<<8;
				tmp |= data[56+j*2];      
			}
			if(addr_u16_u32[index].data_type==_U16)
			{
				sprintf(strval,"%.12g;",(float)(get_ax(index)*((unsigned short)tmp)));
			}
			else if(addr_u16_u32[index].data_type==_INT16)
			{
				sprintf(strval,"%.12g;",(float)(get_ax(index)*(tmp)));
			}
			
			strval[strlen(strval)] = 0;
			strcat(result,strval);
		}
		zlg_debug("---%s\r\n",result);
	//}
	//else
	//{
	//	zlg_debug("analyze_u16_data error\r\n,(int)get_b(index)*2=%d;data[8]=%d\r\n",(int)get_b(index)*2,data[8]);
	//}
}
typedef union _int {
  int i;
  char  bufI[4];
} IntUnion;
static void analyze_u32_data(char* data,unsigned char index,unsigned char func_code)
{
	char strval[50] = {0};
	IntUnion iu;
	int j = 0;
	//if((int)get_b(index)*4*8 == (u16)((u16)data[23]<<8|data[24]))
	//{
		for(j=0 ; j<(int)get_b(index) ; j++)
		{
			if(data[4]==0)
			{
				iu.bufI[0] = data[44+j*4];
				iu.bufI[1] = data[44+j*4+1];
				iu.bufI[2] = data[44+j*4+2];
				iu.bufI[3] = data[44+j*4+3];
			}
			else
			{
				iu.bufI[0] = data[56+j*4];
				iu.bufI[1] = data[56+j*4+1];
				iu.bufI[2] = data[56+j*4+2];
				iu.bufI[3] = data[56+j*4+3];
			}	
			if(addr_u16_u32[index].data_type==_U32)
			{
				sprintf(strval,"%.12g;",(double)((double)((unsigned int)iu.i*get_ax(index))));
			}
			else if(addr_u16_u32[index].data_type==_INT32)
			{
				sprintf(strval,"%.12g;",(float)(iu.i*get_ax(index)));
			}
			strval[strlen(strval)] = 0;
			strcat(result,strval);
		}
		zlg_debug("---%s\r\n",result);
	//}
	//else
	//{
	//	zlg_debug("analyze_u32_data error\r\n,(int)get_b(index)*2=%d;data[8]=%d\r\n",(int)get_b(index)*2,data[8]);
	//}
}
typedef union _float {
  float f;
  char  bufF[4];
} FloatUnion;
static void analyze_float32_data(char* data,unsigned char index,unsigned char func_code)
{
	char strval[50] = {0};
	float tmp = 0;
	int j = 0;
	FloatUnion fu;
	//if((int)get_b(index)*4*8 == (u16)((u16)data[23]<<8|data[24]))
	//{
		for(j=0 ; j<get_b(index) ; j++)
		{
			if(data[4]==00)
			{
				fu.bufF[0] = data[44+j*4];
				fu.bufF[1] = data[44+j*4+1];
				fu.bufF[2] = data[44+j*4+2];
				fu.bufF[3] = data[44+j*4+3];	
			}
			else
			{
				fu.bufF[0] = data[56+j*4];
				fu.bufF[1] = data[56+j*4+1];
				fu.bufF[2] = data[56+j*4+2];
				fu.bufF[3] = data[56+j*4+3];
			}			
			zlg_debug("fu.f=%.3f\r\n",fu.f);
			sprintf(strval,"%.12g;",(double)(fu.f*get_ax(index)));
			strval[strlen(strval)] = 0;
			strcat(result,strval);
		}
		zlg_debug("---%s\r\n",result);
	//}
}

static void analyze_bit_data(char* data,unsigned char index,unsigned char func_code)
{
	char strval[50] = {0};
	u8 a = 0;
	if(get_bits_funcode(index) == SRTP_Q)
	{
		if(data[44]&0x08) a = 1;
		else a=0;
		Lx_itoaEx((a&get_addr_bit_bits(index)), strval , 2);
	}
	else
	{
		//if((data[24]==8)||(data[24]==1))
		//	Lx_itoaEx((data[25]&get_addr_bit_bits(index)), strval , 2);
		//else if((data[24]==16)||(data[24]==2))
			Lx_itoaEx(((data[45]<<8 | data[44])&get_addr_bit_bits(index)), strval , 2);
		//else if((data[24]==24)||(data[24]==3))
		//	Lx_itoaEx(((data[25]<<16 | data[26]<<8 | data[27])&get_addr_bit_bits(index)), strval , 2);
		//else if((data[24]==32)||(data[24]==4))
		//	Lx_itoaEx(((data[25]<<24 | data[26]<<16 | data[27]<<8 | data[28])&get_addr_bit_bits(index)), strval , 2);
	}
	strval[strlen(strval)] = ';';
	strval[strlen(strval)+1] = 0;
	strcat(result,strval);
	zlg_debug("---%s\r\n",result);
}

static void analyze_string_data(char* data,unsigned char index,unsigned char func_code)
{
	char strval[50] = {0};
	char strbuff[50] = {0};
	int i = 0;
	int read_length = 0;
	if(Get_HighLow_Exchange(index)==0)
	{
		if(data[4]==0)
		{
			for(i=0; i<get_string_length(index); i=i+2)
			{
				strval[i] = data[45+i];
				strval[i+1] = data[44+i];
			}
		}
		else
		{
			for(i=0; i<data[4]; i=i+2)
			{
				strval[i] = data[57+i];
				strval[i+1] = data[56+i];
			}	
		}
	}
	else if(Get_HighLow_Exchange(index)==1)
	{
		if(data[4]==0)
		{
			for(i=0; i<get_string_length(index); i++)
			{
				strval[i] = data[44+i];
			}
		}
		else
		{
			for(i=0; i<data[4]; i++)
			{
				strval[i] = data[56+i];
			}
		}
	}
	strval[strlen(strval)] = ';';
	strval[strlen(strval)+1] = 0;
	strcat(result,strval);
	zlg_debug("---%s\r\n",result);
}

unsigned char srtp_analyze_data(unsigned char* data,unsigned char index)
{
	if(data[0]==0x03 && data[33]==0x0E)
	{
		if(index<get_u16_u32_float32_cnt())
		{
			if(addr_u16_u32[index].data_type==_U16 || addr_u16_u32[index].data_type==_INT16)
			{
				analyze_u16_data(data,index,addr_u16_u32[index].type);
			}
			else if(addr_u16_u32[index].data_type==_U32 || addr_u16_u32[index].data_type==_INT32)
			{
				analyze_u32_data(data,index,addr_u16_u32[index].type);
			}
			else if(addr_u16_u32[index].data_type==_FLOAT32)
			{
				analyze_float32_data(data,index,addr_u16_u32[index].type);
			}
			return 0;
		}
		else if(index<get_u16_u32_float32_cnt()+get_bit_cnt())
		{
			analyze_bit_data(data,index-get_u16_u32_float32_cnt(),addr_bit[index-get_u16_u32_float32_cnt()].type);
			return 0;
		}           
		else if(index<get_u16_u32_float32_cnt()+get_bit_cnt()+get_str_cnt())
		{
			analyze_string_data(data,index-(get_u16_u32_float32_cnt()+get_bit_cnt()),addr_string[index-(get_u16_u32_float32_cnt()+get_bit_cnt())].type);
			return 0;
		}
		return 1;
	}
	return 3;
}




char* get_result(unsigned char i)
{
	return result;
}
void clear_result(void)
{
	memset(result,0,sizeof(result));
}
