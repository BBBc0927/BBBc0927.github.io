/*************************************************************************
    > File Name: thread.c
    > Author: bbc
    > Mail: ma6174@163.com 
    > Created Time: 2016年05月11日 星期三 12时44分56秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "uart_demo.h"
#include "txmsg_demo.h"

//char send_buf[1024];	//传输数据
int flag = 1;		//w写指针
//int state = 1;
void uartfunction();
void txmsgfunction();
pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty;
//pthread_cond_t full;
//定义数据结构体
struct buffer
{
	int state;
	char send_buf[1024];
	char recv_buf[1024];
}data_buf;

int main(){
	data_buf.state = 1;
	pthread_t mythread[2];
//	char msg[] = "hello";	
	void *retval;
	
	int j=1;
	int res;
	pthread_mutex_init(&mymutex,NULL);
	pthread_cond_init(&empty,NULL);
/*	for(j = 1 ; j < 3; j++)
	{
		res = j % 2;
		if(res == 0){*/
	
		pthread_create(&mythread[0],NULL,(void *)&uartfunction,NULL);
		
	//	}else{	
		pthread_create(&mythread[1],NULL,(void *)&txmsgfunction,NULL);
		
	//	}
	/*if(i == 0)
	{
		printf("creat successed! \n");
	}*/
		pthread_join(mythread[0], &retval);
		pthread_join(mythread[1], &retval);
	//	printf("get result:%s\n",(char *)retval);
//	}
	return 0;
}

void uartfunction(void *arg)
{
	while(1)	//w=true串口可以写入传感器数据
	{
		while(flag)
		{

			pthread_mutex_lock(&mymutex);
			data_buf.state = uart_demo();	//调用串口函数 与节点通信
			printf("uart working... state: %d\n",data_buf.state);
			sprintf(data_buf.send_buf,"%d",data_buf.state);		//将state写入缓冲区
	//	sleep(1);
			if(data_buf.state == 0)
			{
				flag = 0;
				pthread_cond_signal(&empty);
			}
			pthread_mutex_unlock(&mymutex);
		}
	}
	pthread_exit(NULL) ;
	
//	sleep(1);
}

void txmsgfunction(void *arg)
{
	while(1)		//可以将缓冲区发送至服务器
	{
		while(flag == 0)
		{
			pthread_mutex_lock(&mymutex);
		
			if(data_buf.state == 1)
			{
				flag = 1;
				pthread_cond_wait(&empty,&mymutex);
			}
			data_buf.state = txmsg_demo(data_buf.send_buf);	//调用客户端函数，\与云服务器通信,发送countchar
			
			printf("txmsg working... state: %d\n",data_buf.state);
		//	sprintf()
			pthread_mutex_unlock(&mymutex);
		}
	}

	pthread_exit(NULL) ;
//	sleep(1);
}
