#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>
#include <sys/wait.h> 
#include <sys/ioctl.h> 
#include <net/if.h> 
#include <net/if_arp.h> 
#include <arpa/inet.h> 

#include "type.h"
#include "log.h"
#include "GE_SRTP.h"

#define     T1           (20)    /*Connect timeout*/
extern u32 TIMTICK;

char uart_rev[200][50] = {0};
char* uart_result[200] = {0};
char sendDataReault[MQTT_MSG_MAX_LEN-40] = {0};
int uart_resultCnt = 0;
int uart_isSendData = 0;

int current_status = _INIT;

char serverIP[20] = {"10.45.222.80"};
u32 serverPort = 18245; //   Port


char rev[1536];
int len;
int sock;
unsigned long sck_mode;

struct sockaddr_in addr;

static void print_rx_tx_data(char* data,int flag,int len)
{
	u16 i = 0; 
    char buff[500] = {0};
	for(i=0 ; i<len ; i++)
	{
		sprintf(buff+i*3,"%02X ",data[i]);
	}
	if(flag)
	{
		zlg_debug("SendData:%s\r\n",buff);
	}
	else
	{
		zlg_debug("RecvData:%s\r\n",buff);
	}
}

int connect_init(void)
{
	int i = 0;
	if(sock>0)
	{
		close(sock);
	}
	for(i=0 ; i<200 ; i++)
	{
		uart_result[i] = uart_rev[i];
	}
    current_status = _INIT;
	bzero(&addr,sizeof(addr));
	addr.sin_port = htons(serverPort);
	addr.sin_family = PF_INET;
	strcpy(serverIP,IOT_PBoxInfo.PBoxInfo_equipmentIP());
	addr.sin_addr.s_addr = inet_addr(serverIP);
	sock = socket (AF_INET, SOCK_STREAM , 0);
	if(sock<0)
	{
		zlg_debug("socket creat error\r\n");
		return sock;
	}

	return 0;
}

int socket_not_connect(void)
{
	int res;
	struct timeval timeout={10,0};//3s
	//res=setsockopt(sock,SOL_SOCKET,SO_SNDTIMEO, (char *)&timeout,sizeof(timeout));
	//if(res != 0) return res;
	//res=setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout,sizeof(timeout));
	//if(res != 0) return res;
	res = connect (sock, (struct sockaddr_in *)&addr, sizeof (addr));
	return res;
}

int keep_alive_srtp(void)
{
	int res = 0;
	char* sendData = NULL;
	char* datarev = NULL;
	unsigned short crc = 0;
	int i = 0;
	int cnt = get_u16_u32_float32_cnt() + get_bit_cnt() + get_str_cnt();
	zlg_debug("cmd cnt = %d\r\n",cnt);
	if(cnt > 0)
	{
		sendData = get_cmd_str(0);//数据打包
		print_rx_tx_data(sendData,1,56);
		res = send (sock, (char*)(sendData), 56, 0);
		if(res<0)
		{
			zlg_debug("Send Error : %d\r\n",res);
			return 2;
		}
		res = recv (sock, (char*)rev, sizeof(rev), 0);
		if(res>0)
		{
			datarev = rev;
			rev[res] = 0;
		}
		else
		{
			datarev = 0;
		}
		if(datarev!=0)
		{
			print_rx_tx_data(datarev,0,res);
		}
		else
		{
			zlg_debug("receive data error! index = %d\r\n",i);
			return 2;
		}
		return 0;
	}
	else
	{
		return -1;
	}
}

int srtp_connected(void)
{
	int res = 0;
    char* sendData = NULL;
    char* datarev = NULL;
    unsigned short crc = 0;
	int i = 0;
	int cnt = get_u16_u32_float32_cnt() + get_bit_cnt() + get_str_cnt();
	zlg_debug("cmd cnt = %d\r\n",cnt);
	for(i=0 ; i<cnt ; i++)
	{
		sendData = get_cmd_str(i);//数据打包
		print_rx_tx_data(sendData,1,56);
		res = send (sock, (char*)(sendData), 56, 0);
		if(res<0)
		{
			zlg_debug("Send Error : %d\r\n",res);
			return 2;
		}
		res = recv (sock, (char*)rev, sizeof(rev), 0);
		if(res>0)
		{
			datarev = rev;
			rev[res] = 0;
		}
		else
		{
			datarev = 0;
		}
		//datarev = IOT_Uart.uart_revDataTimeOut(baud,200);
		if(datarev!=0)
		{
			print_rx_tx_data(datarev,0,res);
			//datarev+=2;
			if(0==srtp_analyze_data(datarev,i))
			{}
			else
			{
				zlg_debug("analyze error! index = %d\r\n",i);
				return 1;
			}
		}
		else
		{
			zlg_debug("receive data error! index = %d\r\n",i);
			return 2;
		}
	}
	return 0;
}

unsigned int notConnectTimeOut = 0;
int SECS_TICK = 1;
void app_logic(void)
{
	u8 t = 0;
	int res = 0;
	char tmp[30] = {0};
	switch(current_status)
	{
		case _INIT:
			if(connect_init()!=0)
			{
				current_status = _INIT;
			}
			else
			{
				zlg_debug("init ok\r\n");
				current_status = _NotConnect;
			}
		break;
		case _NotConnect:
			if(socket_not_connect()!=0)
			{
				current_status = _NotConnect;
				notConnectTimeOut++;
                sleep(2);
				if(notConnectTimeOut > T1)
				{
					notConnectTimeOut = 0;
					current_status = _INIT;
				}
			}
			else
			{
				zlg_debug("connect ok\r\n");
				notConnectTimeOut = 0;
				current_status = _Connect_srtp;
			}
		break;
		case _Connect_srtp:
			if(keep_alive_srtp()!=0)
			{
				current_status = _INIT;
			}
			else
			{
				current_status = _Connected;
			}
		case _Connected:
            SECS_TICK++;
			if(!is_empty_MQCmd())
			{
				if(front_MQCmd().flag==1)
				{
					pop_MQCmd();
					if(0==srtp_connected())
					{
						//发送数据 
						//RSTCP_SendData(TC_GetResult(0));
						strncpy(sendDataReault,get_result(0),MQTT_MSG_MAX_LEN-40);
						//zlg_debug("analyze success:%s\r\n",sendDataReault);
						clear_result();
					}
					else
					{
						close(sock);
						clear_result();
						current_status = _INIT;
					}
				}
			}
		break;
	}
}

unsigned char bsp_GetOnLine(void)
{
	return (current_status == _Connected);
}

void app_TestMQ(void)
{
	int i=0;
	if(current_status==_Connected)
	{
		MQCmd temp = {0};
		temp.flag = 1;
		push_MQCmd(temp);
	}
	else
	{
		//memset(sendDataReault,0,MQTT_MSG_MAX_LEN-40);
		memset(uart_rev,0,200*30);
		strncpy(sendDataReault,"-1;-1;-1;",MQTT_MSG_MAX_LEN-40);
		
	}
}


static int split(char *src,const char *separator,char **dest,int *num) {
     char *pNext;
     int count = 0;
     if (src == NULL || strlen(src) == 0) //?????????????0,???? 
        return 1;
     if (separator == NULL || strlen(separator) == 0) //??????????,???? 
        return 2;
     pNext = (char *)strtok(src,separator); //????(char *)????????(??????????????????)
     while(pNext != NULL) {
          strcpy(*dest++ , pNext);
          ++count;

         pNext = (char *)strtok(NULL,separator);  //????(char *)????????
    }  
    *num = count;
	return 0;
} 


typedef enum {BYTE=1,SHORT=2,INT=3,FLOAT=4,STRING=5} DataTypeAPP;
extern void DataSerialization(u8 dataType,int* pos,void* data,char* dataDiscripton);
void app_interface_binary_data(int* pos,MQData* data)
{
	int i = 0;
	int uart_resultCnt = 0;
	char buf[20] = {0};
	const char sep = ';';
	simcom_lbs_receive_info_t* templbs = IOT_GPS.IOT_GetAGPSInfoVal();
	int dataCnt = Get_SendDataCNT();
	split(data->data,&sep,uart_result,&uart_resultCnt);

    for(i=0 ; i< dataCnt; i++)
   	{
		sprintf(buf,"%d->",i);
		//printf("uart_result[%d] : %s\r\n",i,uart_result[i]);
		DataSerialization((u8)STRING,pos,(void*)(uart_result[i]),buf);
	}
	//获取经纬度值
	sprintf(buf,"%f",(float)(templbs->u32Lng)/1000000);
	DataSerialization((u8)STRING,pos,(void*)buf,"longitude");

	sprintf(buf,"%f",(float)(templbs->u32Lat)/1000000);
	DataSerialization((u8)STRING,pos,(void*)buf,"latitude");
	memset(uart_rev,0,200*30);
}

void app_ConvertToQueue(void)
{
	MQData temp = {0};
	strncpy(temp.data,sendDataReault,MQTT_MSG_MAX_LEN-40);
	//zlg_debug("sendDataReault=%s\r\n",sendDataReault);
	//zlg_debug("temp.data=%s\r\n",temp.data);
	IOT_NTP.get_date_time(temp.collTime);
	if(!is_full_MQData())
	{
		push_MQData(temp);
		//zlg_debug("push_MQData ok\r\n size_MQData:%d\r\n",size_MQData());
	}
	else
	{
		zlg_debug("is_full_MQData is true\r\n");
	}
}
