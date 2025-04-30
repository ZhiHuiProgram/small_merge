#include "garbagesort_uart.h"
#include "garbagesort.h"
#include<stdlib.h>
#include<stdio.h>
#include <wiringPi.h>
#include<time.h>
#include <unistd.h>
#include <wiringSerial.h>
#include<string.h>
#include<pthread.h>
#include"pwm.h"
#include "Python.h"
#include"server.h"
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include <netinet/tcp.h>
#include "myoled.h"

int fd;
char *ret;
int mark;
unsigned char buffer[6] = {0xAA, 0X55, 0x00, 0x00, 0x55, 0xAA};
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static int detect_process(const char *process_name);
void *uart_recv();
void *pwm_func();
void *uart_send();
void *myoled(void *buffer);
void *garbagesort_socket();
int main(){
	char *ret;
	int ret1=-1;
	int ret2;
	ret2 = detect_process("mjpg_streamer");//用于判断mjpg_streamer服务是否已经启动

	if ( -1 == ret2)
	{
		printf("detect process failed\n");
		return -1;
	}
	fd = uart_open(SERIAL_DEV, BAUD_RATE);
	pythoninit();
	myoled_init();
	pthread_t thread1;
	pthread_t thread2;
	ret1 = detect_process("mjpg_streamer");
	if ( ret1 == -1)
	{
		printf("photo detect failed\n");
		goto END;
	}
	printf("detect process success\n");
	pthread_create(&thread1, NULL, uart_send, 0);
	pthread_create(&thread2, NULL, uart_recv, 0);
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
END:
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
	pythonfinal();
	return 0;

}
static int detect_process(const char *process_name)
{ 
	int n = -1; 
	FILE *strm; 
	char buf[128]={0}; 
	sprintf(buf,"ps -ax | grep %s|grep -v grep", process_name); 

	if((strm = popen(buf, "r")) != NULL) 
	{ 
		if(fgets(buf, sizeof(buf), strm) != NULL) 
		{
			printf("buf=%s\n",buf);
			n = atoi(buf); 
			printf("n=%d\n",n);
		}
	}
	else
	{
		return -1;
	} 

	pclose(strm); 
	return n; 
}
void *uart_recv(){
	while(1){
		pthread_t thread3;
		pthread_t thread_oled,thread_socket;
		pthread_create(&thread_socket, NULL, garbagesort_socket, 0);	
		pthread_mutex_lock(&mutex);
		pthread_cond_wait(&cond, &mutex);
		pthread_mutex_unlock(&mutex);
		if(strstr(ret,"干垃圾")){
			buffer[2]=0x42;
		}else if(strstr(ret,"湿垃圾")){
			buffer[2]=0x41;
		}else if(strstr(ret,"可回收垃圾")){
			buffer[2]=0x40;
		}else if(strstr(ret,"有害垃圾")){
			buffer[2]=0x3F;
		}else if(strstr(ret,"其他垃圾")){
			buffer[2]=0x3E;
		}else{
			buffer[2]=0x3E;
		}
		pthread_create(&thread_oled, NULL, myoled, buffer);
		write (fd, buffer, 6);
		pthread_create(&thread3, NULL, pwm_func, 0);
		pthread_mutex_lock(&mutex);
		pthread_cond_wait(&cond, &mutex);
		pthread_mutex_unlock(&mutex);
		buffer[2]=0x00;
	}
}
void *garbagesort_socket(){
	char buf[100];
	pthread_detach(pthread_self());
	int c_fd;
	struct sockaddr_in c_addr;
	int len=sizeof(c_addr);
	memset(&c_addr,0,sizeof(c_addr));
	c_fd = garbage_socket();
	while(1){
		c_fd=accept(c_fd,(struct sockaddr *)&c_addr , &len);
		int keepalive = 1; // 开启TCP KeepAlive功能
		int keepidle = 5; // tcp_keepalive_time 3s内没收到数据开始发送心跳包
		int keepcnt = 3; // tcp_keepalive_probes 每次发送心跳包的时间间隔,
		int keepintvl = 3;  // tcp_keepalive_intvl 每3s发送一次心跳包
		setsockopt(c_fd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepalive, sizeof(keepalive));
		setsockopt(c_fd, SOL_TCP, TCP_KEEPIDLE, (void *) &keepidle, sizeof (keepidle));
		setsockopt(c_fd, SOL_TCP, TCP_KEEPCNT, (void *)&keepcnt, sizeof (keepcnt));
		setsockopt(c_fd, SOL_TCP, TCP_KEEPINTVL, (void *)&keepintvl, sizeof (keepintvl));
		printf("%s|%s|%d: Accept a connection from %s:%d\n", __FILE__, __func__, 
				__LINE__, inet_ntoa(c_addr.sin_addr), ntohs(c_addr.sin_port));
		if(c_fd == -1)
		{
			perror("accept");
			continue;
		}
		while(1){
			memset(buf,0,sizeof(buf));
			if((read(c_fd, buf, sizeof(buf)))<=0){
				break;
			}else if (strstr(buf, "open"))
                 {
                    mark =1;
                 }
			printf("recv buf is %s\n",buf);
			if(strcmp(buf,"exit")==0){
				break;
			}
		}

	}
	close(c_fd);
	printf("c_addr net is %s\n",inet_ntoa(c_addr.sin_addr));
	pthread_exit(0);
}
void *myoled(void *buffer){
	pthread_detach(pthread_self());
	oled_demo((unsigned char *)buffer);
	pthread_exit(0);
}
void *pwm_func(){
	pthread_detach(pthread_self());
	pthread_cond_signal(&cond);
	if(buffer[2]==0x42){
		pwm_write(PWM_RECOVERABLE_GARBAGE);
		sleep(2);
		pwm_stop(PWM_RECOVERABLE_GARBAGE);
	}else if(buffer[2]==0x41){
		pwm_write(PWM_RECOVERABLE_GARBAGE);
		sleep(2);
		pwm_stop(PWM_RECOVERABLE_GARBAGE);
	}else if(buffer[2]==0x41){
		pwm_write(PWM_RECOVERABLE_GARBAGE);
		sleep(2);
		pwm_stop(PWM_RECOVERABLE_GARBAGE);
	}else if(buffer[2]==0x40){
		pwm_write(PWM_RECOVERABLE_GARBAGE);
		sleep(2);
		pwm_stop(PWM_RECOVERABLE_GARBAGE);
	}else if(buffer[2]==0x3f){
		pwm_write(PWM_RECOVERABLE_GARBAGE);
		sleep(2);
		pwm_stop(PWM_RECOVERABLE_GARBAGE);
	}else if(buffer[2]==0x3e){
		pwm_write(PWM_RECOVERABLE_GARBAGE);
		sleep(2);
		pwm_stop(PWM_RECOVERABLE_GARBAGE);
	}
	pthread_exit(0);
}
void *uart_send(){	
	ret = (char *)malloc(1);
	while(1){
		while (serialDataAvail (fd)|| mark == 1)//检查是否有可用数据
		{
			if((char)serialGetchar (fd)=='N'|| mark == 1) {
				mark = 0;
				printf("indification received\n");
				system("wget http://127.0.0.1:8080/?action=snapshot -O \
						/home/orangepi/yuanxueshe/pic/image.jpg");
				if (0 == access(GARBAGE_FILE, F_OK)){
					ret=garbagesort("garbage", "gar_rec");
					printf("ret=%s\n",ret);
				}else{
					printf("no file\n");
				}
				sleep(1);
				pthread_mutex_lock(&mutex);
				pthread_cond_signal(&cond);
				pthread_mutex_unlock(&mutex);
				system("sudo rm -f /home/orangepi/yuanxueshe/pic/image.jpg");
				fflush (stdout) ;
			}
		}
	}
	free(ret);

}
