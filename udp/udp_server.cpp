#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXLINE 4096
#define PORT 9000
 
int main(void){
	int server_fd = -1;
  char sendbuf[MAXLINE];
  char recbuf[MAXLINE];
  const char* ip_addr_ctr = "10.236.130.22";

	server_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if(server_fd == -1){
		printf("create socket error: %s(error: %d)\n", strerror(errno), errno);
		exit(0);
	}

  struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = inet_addr(ip_addr_ctr);
 
	int ret = bind(server_fd, (struct sockaddr*)&servaddr, sizeof(servaddr));
  if(ret == -1){
		printf("bind socket error: %s(error: %d)\n", strerror(errno), errno);
    close(server_fd);
		exit(0);
	}

  while(1){
    struct sockaddr_in clientaddr;
    socklen_t addrlen = sizeof(clientaddr);
    ssize_t len = recvfrom(server_fd, recbuf, sizeof(recbuf), 0, reinterpret_cast<sockaddr*>(&clientaddr), &addrlen);
    if(len < 0){
      if(errno == EINTR){
        printf("recv interrupted, retrying...\n");
        continue;
      }
      printf("recv error: %s(error: %d)\n", strerror(errno), errno);
      break;
    }
    if(len == 0){
      printf("recv timed out or connection closed\n");
      break;
    }

    printf("server receive from %s:%u, content: %s  \n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), recbuf);

    sendbuf[0] = 'w';
    sendbuf[1] = 'o';
    sendbuf[2] = 'r';
    sendbuf[3] = 'l';
    sendbuf[4] = 'd';
    sendbuf[5] = '\0';
    ssize_t send_len = sendto(server_fd, sendbuf, sizeof(sendbuf), 0, reinterpret_cast<sockaddr*>(&clientaddr), addrlen);
    if(send_len < 0){
      if(errno == EINTR){
        printf("send interrupted, retrying...\n");
        continue;
      }
      printf("send error: %s(error: %d)\n", strerror(errno), errno);
      break;
    }
    printf("send back: %s\n", sendbuf);
  }
	

  printf("close server_fd\n");
	close(server_fd);
}