

#ifndef TXMSG_DEMO_H_
#define TXMSG_DEMO_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <limits.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <time.h>
#include <assert.h>

#define weixin_host		"yang583570219.applinzi.com"
#define weixin_port		80

static char connect_host(const char *host_addr,const int portno,const char *request)
{
	int sockfd = 0;
	struct sockaddr_in server_addr;
	int i, send, totalsend;
	int Htemp = 0;
	char buffer[1024] = "";
	char pc_tmp[1024] = "";
//	char request[1024];
	int nbytes = 0;
	int mark_num;
	char recv[1024];
	int j;
//	FILE *fp;
	struct hostent *host;
//	double data[3];
	assert((host_addr != NULL) && (request != NULL));		//ASSERT是一个宏，只判断宏参数是否为假或者空,一般用来判断一个等式是否成立。如果成立就不会影响。
	//获取主机IP地址
	if((host = gethostbyname(weixin_host))==NULL)
	{
		fprintf(stderr,"Gethostname error:%s \a\n",strerror(errno));
		exit(1);
	}
	//建立socket连接
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		fprintf(stderr, "Socket Error:%s\a\n", strerror(errno));
		exit(1);
	}
	//http请求包资料补全
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(portno);
	server_addr.sin_addr=*((struct in_addr *)host->h_addr);
	printf("serverip is:%s\n", inet_ntoa(*((struct in_addr *)host->h_addr)));
	//server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	//客户端发起连接请求，连接服务器
	if(connect(sockfd, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr))==-1) {
		fprintf(stderr, "Connect Error:%s\a\n", strerror(errno));
		exit(1);
	}
	send = 0;
	totalsend = 0;
	nbytes = strlen(request); //字符串strlen,整型sizeof
	//发送HTTP请求request
	while(totalsend < nbytes) 
	{
		send = write(sockfd, request+totalsend, nbytes-totalsend);
		if(send==-1) 
		{
			printf("send error!%s\n", strerror(errno));
			strerror(errno);
			exit(0);
		}
		totalsend+=send;
		printf("%d bytes send OK!\n", totalsend);
	}
		//接受HTTP响应
	i = 0;j=0;
	mark_num = 4;        //正常=4,
	while((nbytes=read(sockfd, buffer, 1)) == 1)
		{
			if(i<4) //连续两个\r\n则header结束
			{
				if(buffer[0]=='\r'||buffer[0]=='\n') 
				{
					i++;
					if(Htemp == 0)
					{
						pc_tmp[j] = '\0';
						if (!strcmp(pc_tmp, "HTTP/1.1 200 OK")) //相等返回0 
						{
							printf("HTTP:200 send ok \n");
							Htemp = 1;
						}
					}
				}//直到HTTP200成功
				else 
				{
					i=0;
					pc_tmp[j++] = buffer[0];//新行计数
				}
			}else 
			{
				recv[j++]=buffer[0];
				//fwrite(pc_tmp, 1, 1, fp);
				
				printf("%c", buffer[0]);
			}
		}
	//fclose(fp);
	close(sockfd);
	recv[j] = '\0';
	printf("data: %s\n",recv);
	return Htemp;
}

int weixin_creatdata(const int sensorid, const int state, const int value)
{
	char sendata[1024];
	char request[1024], ret;
    //int len;
	sprintf(sendata, "sensorid%dstate%dvalue%d", sensorid, state,value);//sensorid*state*value*
    sprintf(request, "GET /transData.php?token=doubleq&data=%s HTTP/1.1\r\nAccept: text/html\r\nAccept-Language: zh-cn\r\nUser-Agent: smart210\r\nHost: %s\r\nConnection: Close\r\n\r\n",sendata, weixin_host);
	
	printf(">>send http prototype\n");
	printf("%s", request);
	

    
    //len = strlen(pc_json);

    //sprintf(request, "POST /%s HTTP/1.1\r\nHost: %s\r\nAccept: */*\r\n%s\r\nContent-Length: %d\r\nContent-Type: application/x-www-form-urlencoded\r\nConnection: Close\r\n\r\n%s\r\n",
        //pc_host_file, YEELINK_HOST, pc_header, len, pc_json);
    ret = connect_host(weixin_host, weixin_port, request);

    return(ret);

}
int txmsg_demo(char *countchar)
{
	int ret,i;
	/*int sockfd;
	struct sockaddr_in server_addr;
	int i, send, totalsend;
	char buffer[1024];
	char request[1024];
	char pc_tmp[1024];
	int nbytes;
	char recv[1024];
	int j;
	FILE *fp;
	struct hostent *host;

	//char  *tdata=null;
	/*enum body
	{ id,state,para} machine[8];*/
/*	int data[3];
	
	if(argc != 4)
	{
		printf("id,state,param.\n");
		return -1;
	}

	//printf("please enter:id,state,para:");
		for(i = 0; i < 3; i++)
	{
		data[i] = atoi(argv[i]);
	}

	ret = weixin_creatdata(data[0], data[1], data[2]);
	*/
	//i = 0;
	ret = weixin_creatdata(atoi(countchar),atoi(countchar),
			atoi(countchar));
	printf("transdata state: %d\n",ret);
	return ret;
}

#endif
