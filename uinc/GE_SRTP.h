#ifndef _GE_SRTP_H
#define _GE_SRTP_H

//SRTP_R sUpport  u16 16 u32 32 float float
//SRTP_M sUpport  u16 16 u32 32 float float bool
//SRTP_Q sUpport  bool
//SRTP_I sUpport  u16 16 u32 32 float float bool
//SRTP_AI sUpport  u16 16 u32 32 float float
typedef enum{SRTP_I=0x10,SRTP_Q=0x48,SRTP_M=0x16,SRTP_R=0x08,SRTP_AI=0x0A}SRTP_TYPE;
typedef enum{_U16=1,_U32=2,_FLOAT32=3,_INT16=4,_INT32=5}DATA_TYPE;

typedef struct
{
	SRTP_TYPE       type;                     //func code
	u16             start_addr;				  // start addr
	DATA_TYPE       data_type;                //data type u16/u32/float32
	float           ax;                      //scale
	u32           	b;                       //read length
}ADDR_U16_U32;

typedef struct
{
	SRTP_TYPE      type;                     //func code
	u16            start_addr;				 // start addr
	int            bits;                     //与操作，取其中哪几位
}ADDR_BIT;

typedef struct
{
	SRTP_TYPE      type;                    //func code
	u16            start_addr;				 // start addr
	u32            length;//¶ÁÈ¡×ÖÊý         //read length
	int            highLowExchange;          //0:不交换，1交换   每2Byte为单位是否交换位置
}ADDR_STRING;

void app_logic(void);
int connect_init(void);
void app_TestMQ(void);
void app_ConvertToQueue(void);
unsigned char bsp_GetOnLine(void);
char* bsp_GetDistMac(void);
unsigned char app_GetDistMac(void);

char* get_result(unsigned char i);

void clear_result(void);
int Get_SendDataCNT(void);
unsigned char srtp_analyze_data(unsigned char* data,unsigned char index);



double get_ax(int i);
float get_b(int i);//采集个数
unsigned int get_addr_bit_bits(int i);
int Get_HighLow_Exchange(int i);
void cmd_init(void);

unsigned char get_u16_cnt(void);
unsigned char get_u32_cnt(void);
unsigned char get_float32_cnt(void);
unsigned char get_bit_cnt(void);
unsigned char get_str_cnt(void);
unsigned int string_read_len(int j);
char get_bits_funcode(int i);
int get_string_length(int i);

char* get_cmd_str(int i);
char get_cmd_funcode(int i);

int TC_GetResultCnt(void);


#endif
