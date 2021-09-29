#include "type.h"
#include "GE_SRTP.h"
//typedef enum{RCS=0x01,RIS=2,RHR=3,RIR=4,FSC=5,PSR=6,FMC=15,PMR=16}MODBUS_TCP_TYPE; func code
//_U16=1,_U32=2,_FLOAT32=3,_INT16=4,_INT32=5
//SRTP_R sUpport  u16 16 u32 32 float 
//SRTP_M sUpport  u16 16 u32 32 float  bool
//SRTP_Q sUpport  bit(bool)
//SRTP_I sUpport  u16 16 u32 32 float  bool
//SRTP_AI sUpport  u16 16 u32 32 float 
ADDR_U16_U32 addr_u16_u32[]= {   //16位，32位数据定义
//func code,  start addr,  analyze data type,  scale, read length
/************************ U16 *********************************/
	//{SRTP_AI,  	100,          1,                  1,     3},
	//{SRTP_AI,  	100,          1,                  1,     5},
	//{SRTP_I,  	100,          1,                  1,     3},
	//{SRTP_I,  	100,          1,                  1,     5},
	//{SRTP_M,  	100,          1,                  1,     3},
	//{SRTP_M,  	100,          1,                  1,     5},
	//{SRTP_R,  	100,          1,                  1,     3},
	//{SRTP_R,  	100,          1,                  1,     5},
/************************ I16 *********************************/
	//{SRTP_AI,  	1000,          4,                  1,     3},
	//{SRTP_AI,  	1000,          4,                  1,     5},
	//{SRTP_I,  	1000,          4,                  1,     3},
	//{SRTP_I,  	1000,          4,                  1,     5},
	//{SRTP_M,  	1000,          4,                  1,     3},
	//{SRTP_M,  	1000,          4,                  1,     5},
	//{SRTP_R,  	1000,          4,                  1,     3},
	//{SRTP_R,  	1000,          4,                  1,     5},	
/************************ U32 *********************************/
	//{SRTP_AI,  	10000,          2,                  1,     3},
	//{SRTP_AI,  	10000,          2,                  1,     5},
	//{SRTP_I,  	10000,          2,                  1,     3},
	//{SRTP_I,  	10000,          2,                  1,     5},
	//{SRTP_M,  	10000,          2,                  1,     3},
	//{SRTP_M,  	10000,          2,                  1,     5},
	//{SRTP_R,  	10000,          2,                  1,     3},
	//{SRTP_R,  	10000,          2,                  1,     5},	
/************************ I32 *********************************/
	//{SRTP_AI,  	10000,          5,                  1,     3},
	//{SRTP_AI,  	10000,          5,                  1,     5},
	//{SRTP_I,  	10000,          5,                  1,     3},
	//{SRTP_I,  	10000,          5,                  1,     5},
	//{SRTP_M,  	10000,          5,                  1,     3},
	//{SRTP_M,  	10000,          5,                  1,     5},
	//{SRTP_R,  	10000,          5,                  1,     3},
	//{SRTP_R,  	10000,          5,                  1,     5},	
/************************ float32 *********************************/
	//{SRTP_AI,  	20000,          3,                  1,     3},
	//{SRTP_AI,  	20000,          3,                  1,     5},
	//{SRTP_I,  	20000,          3,                  1,     3},
	//{SRTP_I,  	20000,          3,                  1,     5},
	//{SRTP_M,  	20000,          3,                  1,     3},
	//{SRTP_M,  	20000,          3,                  1,     5},
	//{SRTP_R,  	20000,          3,                  1,     3},
	//{SRTP_R,  	20000,          3,                  1,     5},	
};
ADDR_BIT addr_bit[] = 
{
//func code,  start addr,  bits(取其中某几位)读取长度由bits中第一位不为0的位置数决定
	{SRTP_I,  100,          0xffff},
	{SRTP_I,  100,          0x0004},
	{SRTP_AI,  100,          0x0001},
	{SRTP_AI,  100,          0xffff},
	{SRTP_AI,  100,          0x0004},
	{SRTP_I,  100,           0xffff},
	{SRTP_I,  100,           0x0004},
	{SRTP_M,  100,           0xffff},
	{SRTP_M,  100,           0x0004},
	{SRTP_R,  100,           0xffff},
	{SRTP_R,  100,           0x0004},
	{SRTP_Q,  100,           0x0001},
};

ADDR_STRING addr_string[] = 
{
//func code,   start addr,  read length, 是否交换顺序
//	{SRTP_AI,  200,          6,                 0},
//	{SRTP_AI,  200,          8,                 1},
//	{SRTP_I,  200,          6,                 0},
//	{SRTP_I,  200,          8,                 1},
//	{SRTP_M,  200,          6,                 0},
//	{SRTP_M,  200,          8,                 1},
//	{SRTP_R,  200,          6,                 0},
//	{SRTP_R,  200,          8,                 1},
};

//一下函数禁止修改
int get_u16_u32_float32_cnt(void)
{
	return sizeof(addr_u16_u32)/sizeof(ADDR_U16_U32);
}
unsigned char get_bit_cnt(void)
{
	return sizeof(addr_bit)/sizeof(ADDR_BIT);
}

unsigned char get_str_cnt(void)
{
	return sizeof(addr_string)/sizeof(ADDR_STRING);
}