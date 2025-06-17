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

	struct sockaddr_in servaddr;

 
	server_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if(server_fd == -1){
		printf("create socket error: %s(error: %d)\n", strerror(errno), errno);
		exit(0);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = inet_addr(ip_addr_ctr);
 
  /**
   * int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
   * @brief 绑定套接字到指定的地址和端口
   * @param sockfd: 套接字描述符
   * @param addr: 套接字地址
   * @param addrlen: 套接字地址长度
   * @return: 成功返回0，失败返回-1
   */
	int ret = bind(listen_fd, (struct sockaddr*)&servaddr, sizeof(servaddr));
  if(ret == -1){
		printf("bind socket error: %s(error: %d)\n", strerror(errno), errno);
    close(listen_fd);
		exit(0);
	}
 
  /**
   * int listen(int sockfd, int backlog);
   * @brief 套接字进入监听状态，可以接受连接请求
   * @param sockfd: 套接字描述符
   * @param backlog: 指定等待连接的队列长度。如果队列已满，则会拒绝新的连接请求
   * @return: 成功返回0，失败返回-1
   * 
   * @note 链接队列可以分为两种：
   * 1. 等待连接队列：存放已经连接的套接字描述符
   * 2. 等待接收队列：存放等待连接的套接字描述符
   * 
   */
	ret = listen(listen_fd, 10);
  if(ret == -1){
		printf("listen socket error: %s(error: %d)\n", strerror(errno), errno);
    close(listen_fd);
		exit(0);
	}
 
	while(1){
    printf("waiting for client connect...\n");
    struct sockaddr_in clientaddr;
    socklen_t addrlen = sizeof(clientaddr);

    /**
     * int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
     * @brief 接受连接请求
     * @param sockfd: 套接字描述符
     * @param addr: 套接字地址
     * @param addrlen: 套接字地址长度
     * @return: 成功返回一个新的套接字描述符，失败返回-1
     */
		connect_fd = accept(listen_fd, reinterpret_cast<sockaddr*>(&clientaddr), &addrlen);
    if(connect_fd == -1){
			printf("accept socket error: %s(error: %d)\n", strerror(errno), errno);
			continue;
		}
    printf("accept client connect: %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
    uint16_t client_port = ntohs(clientaddr.sin_port);
    uint32_t client_addr = ntohl(clientaddr.sin_addr.s_addr);
    printf("client port: %u\n", client_port);
    printf("client addr: %s\n", inet_ntoa(clientaddr.sin_addr));

    /**
     * 数据的读写有以下几组函数：
     * 1. read/write：读写数据，通用的读写函数
     * 2. recv/send：从套接字中读取和写入数据，支持TCP和UDP
     * 3. recvfrom/sendto：接收和发送数据，支持多播和广播
     * 4. recvmsg/sendmsg：接收和发送数据，支持多播和广播
     * 5. readv/writev：读写数据，支持分散读和聚集写
     */

    /**
     * ssize_t read(int fd, void *buf, size_t count);
     * @brief 从文件描述符fd中读取数据到buf中，最多读取count个字节
     * @param fd: 文件描述符
     * @param buf: 缓冲区
     * @param count: 读取的字节数
     * @return: 大于0表示实际读取的字节数；等于0表示文件结束；小于0表示出错
     * @note read函数会阻塞，直到读取到数据或者文件结束
     * 
     * ssize_t write(int fd, const void *buf, size_t count);
     * @brief 向文件描述符fd中写入数据，从buf中读取数据，最多写入count个字节
     * @param fd: 文件描述符
     * @param buf: 缓冲区
     * @param count: 写入的字节数
     * @return: 大于等于0表示实际写入的字节数；小于0表示出错
     * 
     * ssize_t recv(int sockfd, void *buf, size_t len, int flags);
     * @brief 从套接字sockfd中读取数据到buf中，最多读取len个字节
     * @param sockfd: 套接字描述符
     * @param buf: 缓冲区
     * @param len: 读取的字节数
     * @param flags: 标志位，一般为0
     * @return: 大于0表示实际读取的字节数；等于0表示连接关闭或超时；小于0表示出错
     * @note recv函数会阻塞，直到读取到数据或者连接关闭
     * @note flag：
     *       MSG_PEEk：查看接收缓冲区中的数据，但不删除数据
     *       MSG_DONTWAIT：非阻塞模式
     *       MSG_WAITALL：阻塞接受，直到接收到指定长度的数据或者连接关闭
     *       MSG_OOB：带外数据OOB(out of band)，也被称为TCP紧急数据 
     * 
     * ssize_t send(int sockfd, const void *buf, size_t len, int flags);
     * 
     * ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen);
     * @brief 从指定地址中读取数据到buf中，最多读取len个字节
     * @param sockfd: 套接字描述符
     * @param buf: 缓冲区
     * @param len: 读取的字节数
     * @param flags: 标志位，一般为0
     * @param from: 指向sockaddr结构体的指针，用于存放发送方的地址
     * @param fromlen: 指向sockaddr结构体的指针，用于存放发送方的地址长度
     * @return: 大于0表示实际读取的字节数；等于0表示连接关闭或超时；小于0表示出错
     * @note recvfrom函数一般用于UDP协议
     * 
     * ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *to, socklen_t tolen);
     * 
     * ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);
     * @brief 从套接字sockfd中读取数据到msg中，最多读取len个字节。支持分散读和额外的控制信息
     * @param sockfd: 套接字描述符
     * @param msg: 缓冲区
     * @param flags: 标志位，一般为0
     * @return: 大于0表示实际读取的字节数；等于0表示连接关闭或超时；小于0表示出错
     * 
     * struct msghdr {
     *   void *msg_name; // 发送方的地址
     *   socklen_t msg_namelen; // 发送方的地址长度
     *   struct iovec *msg_iov; // 缓冲区数组
     *   size_t msg_iovlen; // 缓冲区个数
     *   void *msg_control; // 附加控制信息
     *   size_t msg_controllen; // 附加控制信息长度
     *   int msg_flags; // 标志位
     * };
     * 
     * ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);
     * 
     * ssize_t readv(int fd, const struct iovec *iov, int iovcnt);
     * @brief 从文件描述符fd中读取数据，并将数据分散到多个缓冲区中
     * @param fd: 文件描述符
     * @param iov: 缓冲区数组
     * @param iovcnt: 缓冲区个数
     * @return: 大于0表示实际读取的字节数；等于0表示文件结束；小于0表示出错
     * struct iovec {
     *    void *iov_base; // 缓冲区指针
     *    size_t iov_len; // 缓冲区长度
     * };
     * 
     * ssize_t writev(int fd, const struct iovec *iov, int iovcnt);
     */

		while(1){
			ssize_t len = recv(connect_fd, recbuf, sizeof(recbuf), 0);
			if(len < 0){
				if(errno == EINTR){
					continue;
				}
        printf("recv error: %s(error: %d)\n", strerror(errno), errno);
				break;
			}
      if(len == 0){
        printf("client close connect\n");
        break;
      }
 
			printf("receive: %s\n", recbuf);

      sendbuf[0] = 'w';
      sendbuf[1] = 'o';
      sendbuf[2] = 'r';
      sendbuf[3] = 'l';
      sendbuf[4] = 'd';
      sendbuf[5] = '\0';
      ssize_t send_len = send(connect_fd, sendbuf, sizeof(sendbuf), 0);
      if(send_len < 0){
        if(errno == EINTR){
          continue;
        }
        printf("send error: %s(error: %d)\n", strerror(errno), errno);
        break;
      }
      printf("send back: %s\n", sendbuf);
		}

    printf("close connect_fd\n");
		/**
     * int close(int fd);
     * @brief 关闭文件描述符fd
     * @param fd: 文件描述符
     * @return: 成功返回0，失败返回-1
     */
		close(connect_fd);
	}

  printf("close listen_fd\n");
	close(listen_fd);
}