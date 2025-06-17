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

/**
 * 端口：
 * 
 * 1.端口号的作用：
 *   - 端口号用于区分同一主机上的不同应用程序或服务。例如ftp服务通常使用端口21，http服务通常使用端口80。
 *   - 每个应用程序或服务在网络通信中都需要一个唯一的端口号，以便接收和发送数据。
 * 
 * 2.端口按照端口号分类：
 *  - 知名端口（Well-Known Ports）：范围从0到1023，这些端口通常被系统保留给特定的服务和协议，如HTTP（80）、HTTPS（443）、FTP（21）等。
 *  - 注册端口（Registered Ports）：范围从1024到49151，这些端口可以被用户或应用程序注册使用，但不如知名端口那么严格。
 *  - 动态或者私有端口（Dynamic or Private Ports）：范围从49152到65535，这些端口通常用于临时的或私有的应用程序通信。
 * 
 * 3.端口按照协议分类：
 *   - TCP端口：用于TCP协议的通信，提供可靠的、面向连接的服务。
 *   - UDP端口：用于UDP协议的通信，提供无连接
 *   - IP端口：用于IP协议的通信，通常用于多播和广播通信。
 * 
 * 4.端口占用：
 *   - 某个端口被某个进程绑定(监听)，导致其他进程无法再以相同的协议(如UDP)和相同的IP地址绑定该端口。
 *   - 端口相同，ip地址不同，可以绑定成功。
 *   - 端口相同，协议不同，可以绑定成功。
 *   - 端口相同，ip地址相同，协议相同，则无法绑定成功。
 * 
 * 5.端口复用：
 *  - 端口复用是指允许多个套接字绑定到同一个端口上，以便在同一主机上运行多个服务或应用程序。
 *  - 端口复用通常通过设置套接字选项：
 *   - SO_REUSEADDR：允许多个套接字绑定到同一个端口上，但必须满足特定条件，如所有套接字都必须设置为SO_REUSEADDR选项或者绑定ip地址不同。
 *   - SO_REUSEPORT：允许多个套接字绑定到相同ip和相同端口
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
	multicast_address.sin_family = AF_INET;
	multicast_address.sin_port = htons(PORT);
	multicast_address.sin_addr.s_addr = inet_addr(multicast_addr_ctr); 

	/**
	 * @note socket需要绑定到多播端口，通常是INADDR_ANY跟多播端口。
	 * @note 这样内核才会把发往改端口的数据包发送给你的socket
	 * 
	 * @note 尝试将ip地址修改成固定ip，发现收不到数据。
	 * @note 这是绑定固定地址A，则只有发送给地址A的多播数据才会发送给你的socket。而多播包的目的地址是多播地址，不是你的IP地址。
	 * @note 只有绑定到INADDR_ANY(0.0.0.0)，内核会将所有本机收到的，端口匹配的多播数据包发送给你的socket。
	 */
	struct sockaddr_in local_address;
	memset(&local_address, 0, sizeof(local_address));
	local_address.sin_family = AF_INET;
	local_address.sin_port = htons(PORT);
	local_address.sin_addr.s_addr = htonl(INADDR_ANY);

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

	/**
	 * @note 设置SO_REUSEADDR选项，允许端口复用，避免多个客户端从多播接受数据的时候发生端口占用
	 * @note 如果只有一个客户端，改步骤就不需要了
	 */
	int opt = 1;
	int ret = -1;
	ret = setsockopt(client_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
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

	/**
	 * @note 需要加入多播组，这样内核才会将发往指定多播组的数据转发给你的socket
	 */
	mreq.imr_multiaddr = multicast_address.sin_addr; // 多播组地址
	mreq.imr_interface = local_address.sin_addr; // 本地地址
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