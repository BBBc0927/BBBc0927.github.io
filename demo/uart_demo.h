
#ifndef UART_DEMO_H_
#define UART_DEMO_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>		//标准函数
#include <string.h>
#include <fcntl.h>	//文件控制定义
#include <termios.h>	//终端控制
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FALSE	-1
#define TRUE	0

/******************************************************************* 
* 名称：                  UART_Open 
* 功能：                打开串口并返回串口设备文件描述 
* 入口参数：        fd    :文件描述符     port :串口号(ttyS0,ttyS1,ttyS2) 
* 出口参数：        正确返回为1，错误返回为0 
*******************************************************************/ 

int UART_Open(int fd, char *port)	// fopen 系列是标准的C库函数;open系列是 POSIX 定义的,是UNIX系统里的system call
{
	fd = open(port, O_RDWR|O_NOCTTY|O_NDELAY);	/*以不可阻断的方式打开文件,以可读写方式打开文件,
	打开终端设备，而不是控制终端*/
	if (FALSE == fd)
	{
		perror("open serial error");
			return(FALSE);
	}
	if(fcntl(fd, F_SETFL, 0) < 0)		//此处设置为阻塞,fcntl(socket_descriptor, F_SETFL, flags | O_NONBLOCK)设置非阻塞
	{
		printf("fcntl error");
		return(FALSE);
	}else
		{
		printf("fcntl success: %d", fcntl(fd, F_SETFL, 0));		//成功返回值一般为非0
	}
	if(isatty(STDIN_FILENO) == 0)		//STDIN_FILENO就是标准输入设备（一般是键盘）的文件描述符
		{
			printf("standard input is not a terminal device\n");  
			return(FALSE); 
	}else
		{
			printf("isatty success!\n");  
	}
	return fd;
}
/******************************************************************* 
* 名称：                UART_Close 
* 功能：                关闭串口并返回串口设备文件描述 
* 入口参数：        fd    :文件描述符     port :串口号(ttyS0,ttyS1,ttyS2) 
* 出口参数：        void 
*******************************************************************/  
   
void UART_Close(int fd)  
{  
    close(fd);  
} 
/******************************************************************* 
* 名称：                UART_Set 
* 功能：                设置串口数据位，停止位和效验位 
* 入口参数：        fd        串口文件描述符 
*                              speed     串口速度 
*                              flow_ctrl   数据流控制 
*                           databits   数据位   取值为 7 或者8 
*                           stopbits   停止位   取值为 1 或者2 
*                           parity     效验类型 取值为N,E,O,,S 
*出口参数：          正确返回为1，错误返回为0 
*struct termios {  
    tcflag_t c_iflag;       input mode flags   
    tcflag_t c_oflag;       output mode flags   
    tcflag_t c_cflag;       control mode flags   
    tcflag_t c_lflag;       local mode flags  
    cc_t c_line;            line discipline  
    cc_t c_cc[NCCS];        control characters 
}; 
*******************************************************************/  
int UART_Set(int fd,int speed,int flow_ctrl,int databits,int stopbits,int parity)  //115200 0 8 1 N
{  
     
	int   i;  
	int   status;  
	int   speed_arr[] = { B115200, B19200, B9600, B4800, B2400, B1200, B300};  
	int   name_arr[] = {115200, 19200, 9600, 4800, 2400, 1200, 300};  
           
    struct termios options;  
     
    /*tcgetattr(fd,&options)得到与fd指向对象的相关参数，并将它们保存于options,该函数还可以测试配置是否正确，该串口是否可用等。若调用成功，函数返回值为0，若调用失败，函数返回值为1. 
    */  
    if  ( tcgetattr( fd,&options)  !=  0)  
       {  
          perror("SetupSerial 1");      
          return(FALSE);   
       }     
    //设置串口输入波特率和输出波特率  
    for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)  
                {  
                     if  (speed == name_arr[i])  
                            {               
                                 cfsetispeed(&options, speed_arr[i]);   
                                 cfsetospeed(&options, speed_arr[i]);    
                            }  
              }       
     
    //修改控制模式，保证程序不会占用串口  
    options.c_cflag |= CLOCAL;  
    //修改控制模式，使得能够从串口中读取输入数据  
    options.c_cflag |= CREAD;  
    
    //设置数据流控制  
    switch(flow_ctrl)  
    {  
        
       case 0 ://不使用流控制  
              options.c_cflag &= ~CRTSCTS;  
              break;     
        
       case 1 ://使用硬件流控制  
              options.c_cflag |= CRTSCTS;  
              break;  
       case 2 ://使用软件流控制  
              options.c_cflag |= IXON | IXOFF | IXANY;  
              break;  
    }  
    //设置数据位  
    //屏蔽其他标志位  
    options.c_cflag &= ~CSIZE;  
    switch (databits)  
    {    
       case 5:  
                 options.c_cflag |= CS5;  
                 break;  
       case 6:  
                 options.c_cflag |= CS6;  
                 break;  
       case 7:      
                 options.c_cflag |= CS7;  
                 break;  
       case 8:      
                 options.c_cflag |= CS8;  
                 break;    
       default:     
                 fprintf(stderr,"Unsupported data size\n");  
                 return (FALSE);   
    }  
        // 设置停止位   
    switch (stopbits)  
    {    
       case 1:     
                 options.c_cflag &= ~CSTOPB; break;   
       case 2:     
                 options.c_cflag |= CSTOPB; break;  
       default:     
                       fprintf(stderr,"Unsupported stop bits\n");   
                       return (FALSE);  
    }  
	//设置校验位  
    switch (parity)  
    {    
       case 'n':  
       case 'N': //无奇偶校验位。  
                 options.c_cflag &= ~PARENB;   
                 options.c_iflag &= ~(BRKINT | INPCK | ICRNL | ISTRIP | IXON);      
                 break;   
       case 'o':    
       case 'O'://设置为奇校验      
                 options.c_cflag |= (PARODD | PARENB);   
                 options.c_iflag |= INPCK;               
                 break;   
       case 'e':   
       case 'E'://设置为偶校验    
                 options.c_cflag |= PARENB;         
                 options.c_cflag &= ~PARODD;         
                 options.c_iflag |= INPCK;        
                 break;  
       case 's':  
       case 'S': //设置为空格   
                 options.c_cflag &= ~PARENB;  
                 options.c_cflag &= ~CSTOPB;  
                 break;   
        default:    
                 fprintf(stderr,"Unsupported parity\n");      
                 return (FALSE);   
    }   

     
  //修改输出模式，原始数据输出  
  options.c_oflag &= ~OPOST;  
    
  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  //只用串口传输数据
//options.c_lflag &= ~(ISIG | ICANON);  
     
    //设置等待时间和最小接收字符  
    options.c_cc[VTIME] = 1; /* 读取一个字符等待1*(1/10)s */    
    options.c_cc[VMIN] = 1; /* 读取字符的最少个数为1 */  
     
    /*如果发生数据溢出，接收数据，但是不再读取 刷新收到的数据但是不读  
    tcflush(fd,TCIFLUSH);  */
     
    //激活配置 (将修改后的termios数据设置到串口中）  
    if (tcsetattr(fd,TCSANOW,&options) != 0)    
           {  
               perror("com set error!\n");    
              return (FALSE);   
           }  
    return (TRUE);   
}  

/******************************************************************* 
* 名称：                UART_Init() 
* 功能：                串口初始化 
* 入口参数：    fd       :  文件描述符    
*               speed  :  串口速度 
*               flow_ctrl  数据流控制 
*               databits   数据位   取值为 7 或者8 
*               stopbits   停止位   取值为 1 或者2 
*               parity     效验类型 取值为N,E,O,,S 
*                       
* 出口参数：        正确返回为1，错误返回为0 
*******************************************************************/  
int UART_Init(int fd, int speed,int flow_ctrl,int databits,int stopbits,int parity)  
{  
    int err;  
    //设置串口数据帧格式  
    if (UART_Set(fd,115200,0,8,1,'N') == FALSE)  
       {                                                           
        return FALSE;  
       }  
    else  
       {  
               return  TRUE;  
        }  
} 

/******************************************************************* 
* 名称：                  UART_Recv 
* 功能：                接收串口数据 
* 入口参数：        fd          :文件描述符     
*                   rcv_buf     :接收串口中数据存入rcv_buf缓冲区中 
*                   data_len    :一帧数据的长度 
* 出口参数：        正确返回为1，错误返回为0 
*******************************************************************/  
int UART_Recv(int fd, char *rcv_buf,int data_len)   
{  
    int len,fs_sel;  
    fd_set fs_read;  
     
    struct timeval time;	//有两个成员，，秒数和毫秒数
     
    FD_ZERO(&fs_read);		//fd_set存放文件描述符的集合，zero清空集合
    FD_SET(fd,&fs_read);	/*将一个给定的描述符加入集合中， 从集合中删除FD_CLR(int,fd_set*) 
	检查集合中指定的文件描述符是否可以读写FD_ISSET(int ,fd_set* )*/
     
    time.tv_sec = 10;  
    time.tv_usec = 0;  
     
    //使用select实现串口的多路通信  
    fs_sel = select(fd+1,&fs_read,NULL,NULL,&time);  /*第一个参数maxfdp是一个整数值，
	是指集合中所有文件描述符的范围，即所有文件描述符的最大值加1,因为文件描述符是从0开始计数的，
	+1告诉了select一共有多少描述符，&fs_read即指向fd_set的指针*/
    if(fs_sel)  //如果集合中的描述符允许读取一个文件，那么select返回一个正值
       {  
              len = read(fd,rcv_buf,data_len);  
          printf("I am right!(version1.2) len = %d fs_sel = %d\n",len,fs_sel);  //串口数据可读。读进buf
              return len;  
       }  
    else  
       {  
          printf("Sorry,I am wrong!");  
              return FALSE;  
       }       
}  
/******************************************************************** 
* 名称：                  UART_Send 
* 功能：                发送数据 
* 入口参数：        fd			:文件描述符     
*                   send_buf    :存放串口发送数据 
*                   data_len    :一帧数据的个数 
* 出口参数：        正确返回为1，错误返回为0 
*******************************************************************/  
int UART_Send(int fd, char *send_buf,int data_len)  
{  
    int len = 0;  
     
    len = write(fd,send_buf,data_len);  
    if (len == data_len )  
              {  
                     return len;  
              }       
    else     
        {  
                 
                tcflush(fd,TCOFLUSH);  
                return FALSE;  
        }  
     
}  
   
   
int uart_demo()  //main函数的三个参数，第一个参数使用者，第二个port串口端口号，第三个参数0发送数据，1接受数据
{  
    int fd;                            //文件描述符  
    int err;                           //返回调用函数的状态  
    int len;                          
    int i;  
    char rcv_buf[100];         
    char send_buf[20]="";
	int flag;							//判断输入还是输出
    /*if(argc != 3)  
       {  
              printf("Usage: %s /dev/ttySn 0(send data)/1 (receive data) \n",argv[0]);  
              return FALSE;  
       }  */
    fd = UART_Open(fd,"/dev/ttySAC0"); //打开串口，返回文件描述符  
    do{  
                  err = UART_Init(fd,115200,0,8,1,'N');  
                  printf("Set Port Exactly!\n");  
       }while(FALSE == err || FALSE == fd);  
    
	scanf("please enter 0(send data) or 1(receive data):\n %d",&flag);

   // if(0 == strcmp(argv[2],"0"))  
	  if(flag == 0)
           {  
                  for(i = 0;i < 10;i++)  
                         {  
                                len = UART_Send(fd,send_buf,sizeof(send_buf));  
                                if(len > 0)  
                                       printf(" %d send data successful\n",i);  
                                else  
                                       printf("send data failed!\n");  
                            
                                sleep(2);  
                         }  
                  UART_Close(fd);               
           }  
    else  
           {  
                                        
           while (1) //轮询  
                  {    
                     len = UART_Recv(fd, rcv_buf,sizeof(rcv_buf));  
                     if(len > 0)  
                            {  
                       rcv_buf[len] = '\0';  
                                   printf("receive data is %s\n",rcv_buf);  
                       printf("len = %d\n",len);  
                            }  
                     else  
                            {  
                                   printf("cannot receive data\n");  
                            }  
                     sleep(2);  
              }    
		   //进行异或操作

       UART_Close(fd);   
           }  
 
}
#endif
