#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
 
#define MAXLINE 4096
#define PORT 9000
 
int main(void){
	int client_fd = -1;
	struct sockaddr_in servaddr; 
	char sendbuf[MAXLINE];
	char recbuf[MAXLINE];
	const char* ip_addr_ctr = "10.236.130.22";
 
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = inet_addr(ip_addr_ctr);
 
	if((client_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
		printf("create socket error: %s(error: %d)\n", strerror(errno), errno);
		exit(0);
	}
 
	while(1){
		sleep(1); // Sleep for 1 second before sending the next message
		sendbuf[0] = 'h';
		sendbuf[1] = 'e';
		sendbuf[2] = 'l';
		sendbuf[3] = 'l';
		sendbuf[4] = 'o';
		sendbuf[7] = '\0';
		int send_size = sendto(client_fd, sendbuf, strlen(sendbuf), 0, (struct sockaddr*)&servaddr, sizeof(servaddr));
		if(send_size < 0){
			if(errno == EINTR){
				printf("send interrupted, retrying...\n");
				continue;
			}
			printf("send error: %s(error: %d)\n", strerror(errno), errno);
			break;
		}
    	printf("client sending: %s\n", sendbuf);
 
		struct sockaddr_in server_addr;
		socklen_t addr_len = sizeof(server_addr);
		int rec_size = recvfrom(client_fd, recbuf, sizeof(recbuf), 0, (struct sockaddr*)&server_addr, &addr_len);
		if(rec_size < 0){
			if(errno == EINTR){
				printf("recv interrupted, retrying...\n");
				continue;
			}
			printf("recv error: %s(error: %d)\n", strerror(errno), errno);
			break;
		}
		if(rec_size == 0){
			printf("recv timed out or connection closed\n");
			break;
		}
		printf("client received from %s:%u, content: %s\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port), recbuf);

	}
 
	printf("client exit\n");
	close(client_fd);
}