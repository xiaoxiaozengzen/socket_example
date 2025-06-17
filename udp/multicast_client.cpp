#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

/**
 * 组播组有临时和永久的区别：
 * 1. 永久组播组：官方分配的组播地址，通常用于长期存在的服务。
 * 2. 临时组播组：用户自定义的组播地址，通常用于临时的通信需求。
 * 
 * 永久组播组的地址范围是：
 * 224.0.0.0～224.0.0.255 为预留的组播地址（永久组地址），地址224.0.0.0保留不做分配，其它地址供路由协议使用；
 * 224.0.1.0～224.0.1.255 是公用组播地址，可以用于Internet；欲使用需申请。
 * 224.0.2.0～238.255.255.255 为用户可用的组播地址（临时组地址），全网范围内有效；
 * 239.0.0.0～239.255.255.255 为本地管理组播地址，仅在特定的本地范围内有效。
 */
 
#define MAXLINE 4096
#define PORT 9010
 
int main(void){
	int client_fd = -1;
	char sendbuf[MAXLINE];
	char recbuf[MAXLINE];
	const char* multicast_addr_ctr = "224.0.1.135";
	const char* local_addr_ctr = "10.236.130.22";

	struct sockaddr_in multicast_address;
	memset(&multicast_address, 0, sizeof(multicast_address));
	struct sockaddr_in local_address;
	memset(&local_address, 0, sizeof(local_address));
	socklen_t addr_len = sizeof(multicast_address);

	/**
	 * struct ip_mreq {
	 *   struct in_addr imr_multiaddr; // Multicast group address
	 *   struct in_addr imr_interface; // Local interface address
	 * };
	 * 
	 * @brief 用于IPV4多播组的加入和离开
	 */
	struct ip_mreq mreq;
 
	if((client_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
		printf("create socket error: %s(error: %d)\n", strerror(errno), errno);
		exit(0);
	}

	local_address.sin_family = AF_INET;
	// 本地地址必须设置为INADDR_ANY，并且绑定到多播的端口
	local_address.sin_port = htons(PORT);
	local_address.sin_addr.s_addr = htonl(INADDR_ANY);

	// 设置端口复用
	int opt = 1;
	int ret = setsockopt(client_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if(ret == -1){
		printf("setsockopt error: %s(error: %d)\n", strerror(errno), errno);
		close(client_fd);
		exit(0);
	}

	ret = bind(client_fd, (struct sockaddr*)&local_address, sizeof(local_address));
	if(ret == -1){
		printf("bind socket error: %s(error: %d)\n", strerror(errno), errno);
		close(client_fd);
		exit(0);
	}

	// 加入多播组
	mreq.imr_multiaddr.s_addr = inet_addr(multicast_addr_ctr);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	ret = setsockopt(client_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
	if(ret == -1){
		printf("setsockopt IP_ADD_MEMBERSHIP error: %s(error: %d)\n", strerror(errno), errno);
		close(client_fd);
		exit(0);
	}
 
	while(1){
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

	ret = setsockopt(client_fd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));
	if(ret == -1){
		printf("setsockopt IP_DROP_MEMBERSHIP error: %s(error: %d)\n", strerror(errno), errno);
		close(client_fd);
		exit(0);
	}
 
	printf("client exit\n");
	close(client_fd);
}