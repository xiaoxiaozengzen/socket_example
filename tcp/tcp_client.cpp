#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
 
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
   * 
   * @note 当服务端关闭时，connect会返回-1，并且errno为ECONNREFUSED
   * @note 当服务端bind并listen时，connect会成功
   *       客户端会向服务端发送syn包，服务端会返回syn+ack包，客户端收到syn+ack包并发送ack，可以认为connect成功
   *       此时服务端并没有accept，这条连接是处于半连接状态  
   * @note 当服务端accept时，connect会成功。可以进行数据传输
   */
	int ret = connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
  if(ret == -1){
		printf("connect socket error: %s(error: %d)\n", strerror(errno), errno);
    close(sockfd);
		exit(0);
	}
 
	while(1){
		sendbuf[0] = 'h';
    sendbuf[1] = 'e';
    sendbuf[2] = 'l';
    sendbuf[3] = 'l';
    sendbuf[4] = 'o';
    sendbuf[7] = '\0';
		write(sockfd, sendbuf, strlen(sendbuf)+1);
    printf("sending: %s\n", sendbuf);
 
		int len = read(sockfd, recbuf, sizeof(recbuf));
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
 
		printf("receive %d bytes: %s\n", len, recbuf);
    sleep(1);
	}
 
	close(sockfd);
 
}