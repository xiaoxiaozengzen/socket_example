#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/ioctl.h>
 
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

  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 1000;

  /**
   * int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
   * @brief 设置套接字选项
   * @param sockfd: 套接字描述符
   * @param level: 套接字选项级别
   * @param optname: 套接字选项名称
   * @param optval: 套接字选项值
   * @param optlen: 套接字选项值长度
   * @return: 成功返回0，失败返回-1
   * 
   * @note level:
   * 1. SOL_SOCKET: 通用套接字选项
   * 2. IPPROTO_TCP: TCP协议选项
   * 3. IPPROTO_IP: IP协议选项
   * 
   * @note optname:
   * 1.SO_RCVBUF: 接收缓冲区大小
   * 2.SO_SNDBUF: 发送缓冲区大小
   * 3.SO_REUSEADDR: 重用地址：允许相同的端口绑定绑定不同的地址
   * 4.SO_REUSEPORT: 重用端口：允许多个套接字绑定到同一地址和端口，一般用于多线程服务器
   * 5.SO_KEEPALIVE: TCP保活机制：定期发送心跳包，保持连接
   * 6.SO_RCVTIMEO: 接收超时：设置接收数据的超时时间
   * 7.SO_SNDTIMEO: 发送超时：设置发送数据的超时时间
   * 8.TCP_KEEPIDLE: 设置TCP保活机制的空闲时间
   * 9.IP_TTL: 设置IP数据包的生存时间
   * 10.IP_ADD_MEMBERSHIP: 加入多播组
   * 11.IP_DROP_MEMBERSHIP: 离开多播组
   */
  int set_ret = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
  if(set_ret == -1){
    printf("setsockopt error: %s(error: %d)\n", strerror(errno), errno);
    close(sockfd);
    exit(0);
  }

  int ret = connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
  if(ret == -1){
    printf("connect socket error: %s(error: %d)\n", strerror(errno), errno);
    close(sockfd);
    exit(0);
  }
  printf("connect server: %s:%d\n", inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port));

  /**
   * int fcntl(int fd, int cmd, ... arg );
   * @brief 设置文件描述符的属性
   * @param fd: 文件描述符
   * @param cmd: 命令
   * @param arg: 命令参数，取决于cmd的值
   * @return: 成功返回与cmd对应的值，失败返回-1
   * 
   * @note cmd:
   * 1.文件状态标志：
   *  F_GETFL: 获取文件状态标志
   *  F_SETFL: 设置文件状态标志
   * 
   * @note arg:
   * 对应于F_GETFL和F_SETFL的值：
   *  O_RDONLY: 只读
   *  O_WRONLY: 只写
   *  O_UNBLOCK: 非阻塞
   */
  int flags = fcntl(sockfd, F_GETFL, 0);
  if(flags == -1){
    printf("fcntl get flags error: %s(error: %d)\n", strerror(errno), errno);
    close(sockfd);
    exit(0);
  }
  ret = fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
  if(ret == -1){
    printf("fcntl set flags error: %s(error: %d)\n", strerror(errno), errno);
    close(sockfd);
    exit(0);
  }
  printf("set socket non-blocking\n");

#if 0
  int io_flags = 1;

  /**
   * int ioctl(int fd, unsigned long request, ...args);
   * @brief 设置文件描述符的属性
   * @param fd: 文件描述符
   * @param request: 请求命令
   * @param args: 请求参数，对应于request的值
   * @return: 成功返回0，失败返回-1
   * 
   * @note request:
   * 1. FIONBIO: 设置非阻塞模式
   * 
   * @note args:
   * 对应于FIONBIO的值：
   *     1: 启用非阻塞模式
   *     0: 禁用非阻塞模式
   */
  ret = ioctl(sockfd, FIONBIO, &io_flags);
  if(ret == -1){
    printf("ioctl set non-blocking error: %s(error: %d)\n", strerror(errno), errno);
    close(sockfd);
    exit(0);
  }
  printf("set socket non-blocking\n");

#endif
  
	while(1){
		sendbuf[0] = 'h';
    sendbuf[1] = 'e';
    sendbuf[2] = 'l';
    sendbuf[3] = 'l';
    sendbuf[4] = 'o';
    sendbuf[7] = '\0';
		write(sockfd, sendbuf, sizeof(sendbuf));
    printf("sending: %s\n", sendbuf);

    int len = 0;
    while((len = read(sockfd, recbuf, sizeof(recbuf))) < 0) {
      if(len < 0){
        if(errno == EINTR){
          continue;
        }
        if(errno == EAGAIN || errno == EWOULDBLOCK){
          printf("recv timeout\n");
          continue;
        }
      }
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