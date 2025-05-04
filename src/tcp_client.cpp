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
	char sendbuf[MAXLINE];
  char recbuf[MAXLINE];
  const char* ip_addr_ctr = "10.236.130.22";
 
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = inet_addr(ip_addr_ctr);
 
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		printf("create socket error: %s(error: %d)\n", strerror(errno), errno);
		exit(0);
	}
 
  /**
   * int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
   * @brief 连接服务器
   * @param sockfd: 套接字描述符
   * @param addr: 服务器地址
   * @param addrlen: 地址长度
   * @return: 成功返回0，失败返回-1
   */
	int ret = connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
  if(ret == -1){
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
 
		ssize_t len = read(sockfd, recbuf, sizeof(recbuf));
		if(len < 0){
			if(errno == EINTR){
				continue;
			}
      printf("recv error: %s(error: %d)\n", strerror(errno), errno);
			break;
		}
    if(len == 0){
      printf("server close connect\n");
      break;
    }
 
		printf("receive: %s\n", recbuf);
    sleep(1);
	}
 
	close(sockfd);
 
}