#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "type.h"
#include "GE_SRTP.h"
#include "log.h"
int U16_CNT = 0 ;
int U32_CNT = 0;
int FLOAT_CNT = 0;
int Data_CNT = 0;
u16 invokeid = 0;
//typedef enum{R=0x9c,B=0xa0,MR=0x90,LR=0x92,CR=0x91,CM=0xa9,DM=0xa8,FM=0xaf,ZF=0xb0,W=0xb4,TN=0xc2,TS=0xc1,CN=0xc5,CS=0xc4}MC_TYPE;
extern unsigned char DeviceID ;
extern ADDR_U16_U32 addr_u16_u32[];
extern ADDR_BIT addr_bit[];
extern ADDR_STRING addr_string[];
extern int get_u16_u32_float32_cnt(void);
void Init_U16_U32_FLOATCNT(void)
{
	int i = 0;
	for(i=0 ; i<get_u16_u32_float32_cnt();i++)
	{
		if(addr_u16_u32[i].data_type==_U16 || addr_u16_u32[i].data_type==_INT16)
		{
			U16_CNT++;
			Data_CNT += addr_u16_u32[i].b*1;
		}
		else if(addr_u16_u32[i].data_type==_U32 || addr_u16_u32[i].data_type==_INT32)
		{
			U32_CNT++;
			Data_CNT += addr_u16_u32[i].b*1;
		}
		else if(addr_u16_u32[i].data_type==_FLOAT32)
		{
			FLOAT_CNT++;
			Data_CNT += addr_u16_u32[i].b*1;
		}
	}
	printf("U16_CNT=%d;U32_CNT=%d;FLOAT_CNT=%d\r\n",U16_CNT,U32_CNT,FLOAT_CNT);
}
double get_ax(int i)
{
	return addr_u16_u32[i].ax;
}
float get_b(int i)
{
	return addr_u16_u32[i].b;
}

static unsigned int bit_read_len(int j)
{
	//int i=0;
	//for(i=0 ; i<16 ; i++)
	//{
	//	if((addr_bit[j].bits&(0x8000>>i))!=0)
	//	{
	//		//zlg_debug("num=%d\r\n",(15-i)/8+1);
	//		return ((15-i)/8+1);
	//	}
	//}
	//return 0;
	return 2;
}
unsigned int string_read_len(int j)
{
	if(addr_string[j].length%2 != 0)
		return (addr_string[j].length/2+1);
	else
		return (addr_string[j].length/2);
}

unsigned int get_addr_bit_bits(int i)
{
	return addr_bit[i].bits;
}


int Get_HighLow_Exchange(int i)
{
	return addr_string[i].highLowExchange;
}
int get_string_length(int i)
{
	return addr_string[i].length;
}

char changeToChar(char data)
{
    switch(data)
    {
        case 0:case 1:case 2:case 3:case 4:case 5:case 6:case 7:case 8:case 9:
            return (data + '0');
        case 10:
            return 'a';
        case 11:
            return 'b';
        case 12:
            return 'c';
        case 13:
            return 'd';
        case 14:
            return 'e';
        case 15:
            return 'f';
        default:
            return 0xff;
    }
}

u8 buf[100] = {
	0x02,0x00,
	0x00,0x00,//index
	0x00,0x00,0x00,0x00,0x00,0x01, 
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00, 
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
	0x06,0xC0,0x00,0x00,0x00,0x00,0x10,0x0E,0x00,0x00, 
	0x01,0x01,0x04,
	0x00,//function code
	0x00,0x00,//address
	0x00,0x00,//length
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
u8* create_data_type(u8 FuncCode,u16 startAddr,u32 readLth)
{
	invokeid++;
	buf[2] = (u8)invokeid;
	buf[3] = (u8)(invokeid>>8);
	buf[43] = FuncCode;
	buf[44] = (u8)(startAddr-1);
	buf[45] = (u8)((startAddr-1)>>8);
	buf[46] = (u8)readLth; 
	buf[47] = (u8)(readLth>>8);
	return buf;
}

void cmd_init(void)
{
	Init_U16_U32_FLOATCNT();
}

unsigned char get_u16_cnt(void)
{
	return U16_CNT;
}
unsigned char get_u32_cnt(void)
{
	return U32_CNT;
}
unsigned char get_float32_cnt(void)
{
	return FLOAT_CNT;
}
extern unsigned char get_bit_cnt(void);

extern unsigned char get_str_cnt(void);


static char* get_u16_u32_float32_cmd_str(int i)
{
	if((addr_u16_u32[i].data_type == _U16) || (addr_u16_u32[i].data_type == _INT16))
	{
		if(addr_u16_u32[i].type == SRTP_R ||addr_u16_u32[i].type == SRTP_AI)
			return create_data_type(addr_u16_u32[i].type,addr_u16_u32[i].start_addr,(u16)(addr_u16_u32[i].b));
		else
			return create_data_type(addr_u16_u32[i].type,addr_u16_u32[i].start_addr,(u16)(2*addr_u16_u32[i].b));
	}
	else if((addr_u16_u32[i].data_type == _U32) || (addr_u16_u32[i].data_type == _INT32) ||  (addr_u16_u32[i].data_type == _FLOAT32))
	{
		if(addr_u16_u32[i].type == SRTP_R||addr_u16_u32[i].type == SRTP_AI)
			return create_data_type(addr_u16_u32[i].type,addr_u16_u32[i].start_addr,(u16)(2*addr_u16_u32[i].b));
		else
			return create_data_type(addr_u16_u32[i].type,addr_u16_u32[i].start_addr,(u16)(4*addr_u16_u32[i].b));
	}
	return "ERROR";
}
static char* get_bITCMDSTR(int i)
{
	if(i<get_bit_cnt())
	{
		if(addr_bit[i].type == SRTP_Q ||addr_bit[i].type == SRTP_R||addr_bit[i].type == SRTP_AI)
			return create_data_type(addr_bit[i].type,addr_bit[i].start_addr,1);
		else
			return create_data_type(addr_bit[i].type,addr_bit[i].start_addr,bit_read_len(i));
	}
	return "ERROR";
}
static char* get_str_cmd_str(int i)
{
	if(i<get_str_cnt())
	{
		if((addr_string[i].type == SRTP_R)||(addr_string[i].type == SRTP_AI))
			return create_data_type(addr_string[i].type,addr_string[i].start_addr,string_read_len(i));
		else
			return create_data_type(addr_string[i].type,addr_string[i].start_addr,addr_string[i].length);
	}
	return "ERROR";
}
char* get_cmd_str(int i)
{
	if(i<get_u16_u32_float32_cnt()+get_bit_cnt()+get_str_cnt())
	{
		if(i<get_u16_u32_float32_cnt())
		{
			return get_u16_u32_float32_cmd_str(i);
		}
		else if(i<get_u16_u32_float32_cnt()+get_bit_cnt())
		{
			return get_bITCMDSTR(i-get_u16_u32_float32_cnt());
		} 
		else if(i<get_u16_u32_float32_cnt()+get_bit_cnt()+get_str_cnt())
		{
			return get_str_cmd_str(i-(get_u16_u32_float32_cnt()+get_bit_cnt()));
		}
	}
	return "ERROR";
}
char get_cmd_funcode(int i)
{
	if(i<get_u16_u32_float32_cnt()+get_bit_cnt()+get_str_cnt())
	{
		if(i<get_u16_u32_float32_cnt())
		{
			return addr_u16_u32[i].type;
		}
		else if(i<get_u16_u32_float32_cnt()+get_bit_cnt())
		{
			return addr_bit[i-(get_u16_u32_float32_cnt())].type;
		}
		else if(i<get_u16_u32_float32_cnt()+get_bit_cnt()+get_str_cnt())
		{
			return addr_string[i-(get_u16_u32_float32_cnt()+get_bit_cnt())].type;
		}
	}
	return 0;
}
char get_bits_funcode(int i)
{	
	return addr_bit[i].type;
}
int Get_SendDataCNT(void)
{
	return (Data_CNT + get_bit_cnt() + get_str_cnt());
}