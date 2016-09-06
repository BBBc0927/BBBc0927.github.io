
#ifndef UART_DEMO_H_
#define UART_DEMO_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>		//��׼����
#include <string.h>
#include <fcntl.h>	//�ļ����ƶ���
#include <termios.h>	//�ն˿���
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FALSE	-1
#define TRUE	0

/******************************************************************* 
* ���ƣ�                  UART_Open 
* ���ܣ�                �򿪴��ڲ����ش����豸�ļ����� 
* ��ڲ�����        fd    :�ļ�������     port :���ں�(ttyS0,ttyS1,ttyS2) 
* ���ڲ�����        ��ȷ����Ϊ1�����󷵻�Ϊ0 
*******************************************************************/ 

int UART_Open(int fd, char *port)	// fopen ϵ���Ǳ�׼��C�⺯��;openϵ���� POSIX �����,��UNIXϵͳ���system call
{
	fd = open(port, O_RDWR|O_NOCTTY|O_NDELAY);	/*�Բ�����ϵķ�ʽ���ļ�,�Կɶ�д��ʽ���ļ�,
	���ն��豸�������ǿ����ն�*/
	if (FALSE == fd)
	{
		perror("open serial error");
			return(FALSE);
	}
	if(fcntl(fd, F_SETFL, 0) < 0)		//�˴�����Ϊ����,fcntl(socket_descriptor, F_SETFL, flags | O_NONBLOCK)���÷�����
	{
		printf("fcntl error");
		return(FALSE);
	}else
		{
		printf("fcntl success: %d", fcntl(fd, F_SETFL, 0));		//�ɹ�����ֵһ��Ϊ��0
	}
	if(isatty(STDIN_FILENO) == 0)		//STDIN_FILENO���Ǳ�׼�����豸��һ���Ǽ��̣����ļ�������
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
* ���ƣ�                UART_Close 
* ���ܣ�                �رմ��ڲ����ش����豸�ļ����� 
* ��ڲ�����        fd    :�ļ�������     port :���ں�(ttyS0,ttyS1,ttyS2) 
* ���ڲ�����        void 
*******************************************************************/  
   
void UART_Close(int fd)  
{  
    close(fd);  
} 
/******************************************************************* 
* ���ƣ�                UART_Set 
* ���ܣ�                ���ô�������λ��ֹͣλ��Ч��λ 
* ��ڲ�����        fd        �����ļ������� 
*                              speed     �����ٶ� 
*                              flow_ctrl   ���������� 
*                           databits   ����λ   ȡֵΪ 7 ����8 
*                           stopbits   ֹͣλ   ȡֵΪ 1 ����2 
*                           parity     Ч������ ȡֵΪN,E,O,,S 
*���ڲ�����          ��ȷ����Ϊ1�����󷵻�Ϊ0 
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
     
    /*tcgetattr(fd,&options)�õ���fdָ��������ز������������Ǳ�����options,�ú��������Բ��������Ƿ���ȷ���ô����Ƿ���õȡ������óɹ�����������ֵΪ0��������ʧ�ܣ���������ֵΪ1. 
    */  
    if  ( tcgetattr( fd,&options)  !=  0)  
       {  
          perror("SetupSerial 1");      
          return(FALSE);   
       }     
    //���ô������벨���ʺ����������  
    for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)  
                {  
                     if  (speed == name_arr[i])  
                            {               
                                 cfsetispeed(&options, speed_arr[i]);   
                                 cfsetospeed(&options, speed_arr[i]);    
                            }  
              }       
     
    //�޸Ŀ���ģʽ����֤���򲻻�ռ�ô���  
    options.c_cflag |= CLOCAL;  
    //�޸Ŀ���ģʽ��ʹ���ܹ��Ӵ����ж�ȡ��������  
    options.c_cflag |= CREAD;  
    
    //��������������  
    switch(flow_ctrl)  
    {  
        
       case 0 ://��ʹ��������  
              options.c_cflag &= ~CRTSCTS;  
              break;     
        
       case 1 ://ʹ��Ӳ��������  
              options.c_cflag |= CRTSCTS;  
              break;  
       case 2 ://ʹ�����������  
              options.c_cflag |= IXON | IXOFF | IXANY;  
              break;  
    }  
    //��������λ  
    //����������־λ  
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
        // ����ֹͣλ   
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
	//����У��λ  
    switch (parity)  
    {    
       case 'n':  
       case 'N': //����żУ��λ��  
                 options.c_cflag &= ~PARENB;   
                 options.c_iflag &= ~(BRKINT | INPCK | ICRNL | ISTRIP | IXON);      
                 break;   
       case 'o':    
       case 'O'://����Ϊ��У��      
                 options.c_cflag |= (PARODD | PARENB);   
                 options.c_iflag |= INPCK;               
                 break;   
       case 'e':   
       case 'E'://����ΪżУ��    
                 options.c_cflag |= PARENB;         
                 options.c_cflag &= ~PARODD;         
                 options.c_iflag |= INPCK;        
                 break;  
       case 's':  
       case 'S': //����Ϊ�ո�   
                 options.c_cflag &= ~PARENB;  
                 options.c_cflag &= ~CSTOPB;  
                 break;   
        default:    
                 fprintf(stderr,"Unsupported parity\n");      
                 return (FALSE);   
    }   

     
  //�޸����ģʽ��ԭʼ�������  
  options.c_oflag &= ~OPOST;  
    
  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  //ֻ�ô��ڴ�������
//options.c_lflag &= ~(ISIG | ICANON);  
     
    //���õȴ�ʱ�����С�����ַ�  
    options.c_cc[VTIME] = 1; /* ��ȡһ���ַ��ȴ�1*(1/10)s */    
    options.c_cc[VMIN] = 1; /* ��ȡ�ַ������ٸ���Ϊ1 */  
     
    /*�����������������������ݣ����ǲ��ٶ�ȡ ˢ���յ������ݵ��ǲ���  
    tcflush(fd,TCIFLUSH);  */
     
    //�������� (���޸ĺ��termios�������õ������У�  
    if (tcsetattr(fd,TCSANOW,&options) != 0)    
           {  
               perror("com set error!\n");    
              return (FALSE);   
           }  
    return (TRUE);   
}  

/******************************************************************* 
* ���ƣ�                UART_Init() 
* ���ܣ�                ���ڳ�ʼ�� 
* ��ڲ�����    fd       :  �ļ�������    
*               speed  :  �����ٶ� 
*               flow_ctrl  ���������� 
*               databits   ����λ   ȡֵΪ 7 ����8 
*               stopbits   ֹͣλ   ȡֵΪ 1 ����2 
*               parity     Ч������ ȡֵΪN,E,O,,S 
*                       
* ���ڲ�����        ��ȷ����Ϊ1�����󷵻�Ϊ0 
*******************************************************************/  
int UART_Init(int fd, int speed,int flow_ctrl,int databits,int stopbits,int parity)  
{  
    int err;  
    //���ô�������֡��ʽ  
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
* ���ƣ�                  UART_Recv 
* ���ܣ�                ���մ������� 
* ��ڲ�����        fd          :�ļ�������     
*                   rcv_buf     :���մ��������ݴ���rcv_buf�������� 
*                   data_len    :һ֡���ݵĳ��� 
* ���ڲ�����        ��ȷ����Ϊ1�����󷵻�Ϊ0 
*******************************************************************/  
int UART_Recv(int fd, char *rcv_buf,int data_len)   
{  
    int len,fs_sel;  
    fd_set fs_read;  
     
    struct timeval time;	//��������Ա���������ͺ�����
     
    FD_ZERO(&fs_read);		//fd_set����ļ��������ļ��ϣ�zero��ռ���
    FD_SET(fd,&fs_read);	/*��һ�����������������뼯���У� �Ӽ�����ɾ��FD_CLR(int,fd_set*) 
	��鼯����ָ�����ļ��������Ƿ���Զ�дFD_ISSET(int ,fd_set* )*/
     
    time.tv_sec = 10;  
    time.tv_usec = 0;  
     
    //ʹ��selectʵ�ִ��ڵĶ�·ͨ��  
    fs_sel = select(fd+1,&fs_read,NULL,NULL,&time);  /*��һ������maxfdp��һ������ֵ��
	��ָ�����������ļ��������ķ�Χ���������ļ������������ֵ��1,��Ϊ�ļ��������Ǵ�0��ʼ�����ģ�
	+1������selectһ���ж�����������&fs_read��ָ��fd_set��ָ��*/
    if(fs_sel)  //��������е������������ȡһ���ļ�����ôselect����һ����ֵ
       {  
              len = read(fd,rcv_buf,data_len);  
          printf("I am right!(version1.2) len = %d fs_sel = %d\n",len,fs_sel);  //�������ݿɶ�������buf
              return len;  
       }  
    else  
       {  
          printf("Sorry,I am wrong!");  
              return FALSE;  
       }       
}  
/******************************************************************** 
* ���ƣ�                  UART_Send 
* ���ܣ�                �������� 
* ��ڲ�����        fd			:�ļ�������     
*                   send_buf    :��Ŵ��ڷ������� 
*                   data_len    :һ֡���ݵĸ��� 
* ���ڲ�����        ��ȷ����Ϊ1�����󷵻�Ϊ0 
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
   
   
int uart_demo()  //main������������������һ������ʹ���ߣ��ڶ���port���ڶ˿ںţ�����������0�������ݣ�1��������
{  
    int fd;                            //�ļ�������  
    int err;                           //���ص��ú�����״̬  
    int len;                          
    int i;  
    char rcv_buf[100];         
    char send_buf[20]="";
	int flag;							//�ж����뻹�����
    /*if(argc != 3)  
       {  
              printf("Usage: %s /dev/ttySn 0(send data)/1 (receive data) \n",argv[0]);  
              return FALSE;  
       }  */
    fd = UART_Open(fd,"/dev/ttySAC0"); //�򿪴��ڣ������ļ�������  
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
                                        
           while (1) //��ѯ  
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
		   //����������

       UART_Close(fd);   
           }  
 
}
#endif
