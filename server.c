#include<stdio.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<unistd.h>

int garbage_socket(){
	int s_fd;
	
	struct sockaddr_in myaddr={
		.sin_family=AF_INET,
		.sin_port=htons(8601),
	};
	
	inet_aton("192.168.31.111",&(myaddr.sin_addr));
	s_fd=socket(AF_INET,SOCK_STREAM,0);
	bind(s_fd,(struct sockaddr *)&myaddr,sizeof(myaddr));
	listen(s_fd,1);
	return s_fd;
}
