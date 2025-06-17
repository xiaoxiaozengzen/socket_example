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
#define PORT 9010
 
int main(void){
	int server_fd = -1;
  char sendbuf[MAXLINE];
  char recbuf[MAXLINE];
  const char* multicast_addr_ctr = "224.0.1.135";
  int ret = -1;

	server_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if(server_fd == -1){
		printf("create socket error: %s(error: %d)\n", strerror(errno), errno);
		exit(0);
	}

  struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = inet_addr(multicast_addr_ctr);

  // 设置端口复用
	int opt = 1;
	ret = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if(ret == -1){
		printf("setsockopt error: %s(error: %d)\n", strerror(errno), errno);
		close(server_fd);
		exit(0);
	}
 
	ret = bind(server_fd, (struct sockaddr*)&servaddr, sizeof(servaddr));
  if(ret == -1){
		printf("bind socket error: %s(error: %d)\n", strerror(errno), errno);
    close(server_fd);
		exit(0);
	}

  while(1){
    sleep(1); // Sleep for 1 second before sending the next message
    sendbuf[0] = 'h';
    sendbuf[1] = 'e';
    sendbuf[2] = 'l';
    sendbuf[3] = 'l';
    sendbuf[4] = 'o';
    sendbuf[5] = '\0';
    int len = sendto(server_fd, sendbuf, sizeof(sendbuf), 0, (struct sockaddr*)&servaddr, sizeof(servaddr));
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
    printf("server sending %d bytes: %s\n", len, sendbuf);
  }
	

  printf("close server_fd\n");
	close(server_fd);
}