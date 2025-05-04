#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
 
#define MAXLINE 4096
#define PORT 8000
 
int main(void){
	int sockfd = -1;
	struct sockaddr_in servaddr;
 
	char sendbuf[MAXLINE], recbuf[MAXLINE];
 
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
 
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		printf("create socket error: %s(error: %d)\n", strerror(errno), errno);
		exit(0);
	}
 
	if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
		printf("connect socket error: %s(error: %d)\n", strerror(errno), errno);
		exit(0);
	}
 
	while(1){
		sendbuf[0] = 'h';
    sendbuf[1] = 'e';
    sendbuf[2] = 'l';
    sendbuf[3] = 'l';
    sendbuf[4] = 'o';
    sendbuf[7] = '\0';
		write(sockfd, sendbuf, sizeof(sendbuf));
    printf("sending: %s\n", sendbuf);
 
		//从服务器接收信息
		ssize_t len = read(sockfd, recbuf, sizeof(recbuf));
		if(len < 0){
			if(errno == EINTR){
				continue;
			}
			exit(0);
		}
 
		printf("receive: %s\n", recbuf);
    sleep(1);
	}
 
	//关闭套接字
	close(sockfd);
 
}