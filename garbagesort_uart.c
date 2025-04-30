#include <stdio.h>
#include <string.h>
#include <errno.h>

#include<stdlib.h>

#include <wiringPi.h>
#include <wiringSerial.h>
#include <time.h>


char a[10];
int i=0;
void *uart_getdata(int fd){
	while(1){
		while (serialDataAvail (fd))//检查是否有可用数据
		{
			if((char)serialGetchar (fd)=='N') {
			system("sudo fswebcam -d /dev/video1 \
			--no-banner -r 1280x720 -S 5 /home/orangepi/yuanxueshe/pic/image.jpg");
			fflush (stdout) ;
			}
		}
	} 
}//const int fd,const unsigned char *s, int len
void *uart_putsdata(unsigned char *a,int len, int fd){
	char b[6];
	while(1){
		if(strncmp(a,"quit",4)==0){
			printf("quit");
			exit(1);
		}
		strncpy(b,a,len);//将a复制到b中
		serialPuts (fd, b);			
		memset(b,'\0',sizeof(b)/sizeof(b[1]));
	}
}

void *uart_putdata(unsigned char *a,int fd){
	while(1){
		while(1){
			if(strncmp(a,"quit",4)==0){
				printf("quit");
				exit(1);
			}
			if(a[i]=='\0'){
				i=0;
				break;
			}
			serialPutchar (fd, a[i]) ;
			i++;
		}
		memset(a,'\0',sizeof(a)/sizeof(a[1]));
	}

}
int uart_open(char *file, int baud_rate){
	int uart_openfd;
	uart_openfd = serialOpen (file, baud_rate);	
	if (wiringPiSetup () == -1){
		perror("wiringpi");
	}
	return uart_openfd;	
}