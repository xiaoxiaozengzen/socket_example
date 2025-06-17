#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


/**
 * tcp协议：
 * 1.每一个socket都会在内核中分配一个接收缓冲区和一个发送缓冲区
 * 2.接收缓冲区：
 *  1)接收数据时，内核会将数据放入接收缓冲区中
 *  2)read等接口，会从接收缓冲区中拷贝数据到用户空间
 * 3.发送缓冲区：
 *  1)用户调用send接口，会将数据放入发送缓冲区中，然后send接口便会返回
 *  2)内核会将发送缓冲区中的数据发送到网络中
 * 4.实际使用中：
 *  1）如果接收缓冲区满了，接收端会通知发送端，并且接受窗口关闭，发送端便不会再继续发送消息。这便是滑动窗口的原理
 *  2）如果接收缓冲区满了，并且发送端继续发送消息，则接收端会丢弃数据
 * 
 */

/**
 * udp协议：
 * 1.每一个socket都会在内核中分配一个接收缓冲区，一般没有发送缓冲区。因为udp是无连接的协议，不用保证发送数据的可靠性
 * 2.接收缓冲区：
 *  1)接收数据时，内核会将数据放入接收缓冲区中
 *  2)read等接口，会从接收缓冲区中拷贝数据到用户空间
 *  3)当接收缓冲区满了，内核会丢弃数据
 */

/**
 * struct sockaddr {
 *     unsigned short sa_family; // 地址族
 *     char sa_data[14]; // 包含套接字中的目标地址和端口地址 
 * };
 * 
 * @note sockaddr的缺陷是：sa_data把目标地址和端口信息混在一起了
 * @note sockaddr_in把目标地址和端口信息分开了
 * @note 二者长度一样，都是16个字节，即占用的内存大小是一致的，因此可以互相转化。
 */
 
#define MAXLINE 4096
#define PORT 8000
 
int main(void){
	int listen_fd = -1;
  int connect_fd = -1;
  char sendbuf[MAXLINE];
  char recbuf[MAXLINE];
  const char* ip_addr_ctr = "10.236.130.22";

  /**
   * struct sockaddr_in{
   *     short sin_family; // 地址族
   *     unsigned short sin_port; // 端口号
   *     struct in_addr sin_addr; // IP地址
   *     char sin_zero[8]; // 填充字节，为了字节对齐
   * };
   * 
   * typedef struct in_addr {
   *    union {
   *        struct {
   *            unsigned char s_b1, s_b2, s_b3, s_b4;
   *        } S_un_b;
   *        struct {
   *            unsigned short s_w1, s_w2;
   *        } S_un_w;
   *        unsigned long S_addr; // IP地址
   *    } S_un;
   * #define s_addr S_un.S_addr       // IP地址
   * #define s_host S_un.S_un_w.s_w2  // 主机字节序
   * #define s_net S_un.S_un_w.s_w1   // 网络字节序
   * #define s_b1 S_un.S_un_b.s_b1    // 字节1
   * #define s_b2 S_un.S_un_b.s_b2
   * #define s_b3 S_un.S_un_b.s_b3
   * #define s_b4 S_un.S_un_b.s_b4
   * }IN_ADDR, *PIN_ADDR, FAR *LPIN_ADDR;
   */
	struct sockaddr_in servaddr;

 
  /**
   * int socket(int domain, int type, int protocol);
   * @brief 创建一个套接字
   * 
   * @param domain: 地址族
   * @param type: 套接字类型
   * @param protocol: 协议类型 
   * @return: 返回一个套接字描述符
   * 
   * @note socket函数用于创建一个套接字，并返回一个套接字描述符
   * @note socket函数创建的套接字是一个无连接的套接字，不能直接用于通信
   * @note 此时需要调用bind明确端口，或者connect/listen自动分配端口
   * 
   * domain: 常用得协议族有AF_INET(IPv4)和AF_INET6(IPv6)、AF_LOCAL(或称AF_UNIX)等
   *         协议族决定了socket的地址类型，在通信中必须采用相同的协议族。
   *         例如：AF_INET表示ipv4地址跟端口的组合
   * 
   * type: 套接字类型有：
   *       SOCK_STREAM：面向连接的字节流套接字，使用TCP协议
   *       SOCK_DGRAM：无连接的报文套接字，使用UDP协议
   *       SOCK_RAW：原始套接字，使用IP协议
   * 
   * protocol: 协议类型，type和protocol不能随意组合。当为0时，表示使用type类型对应的默认协议。
   *           IPPROTO_TCP：TCP协议
   *           IPPROTO_UDP：UDP协议
   *           IPPROTO_ICMP：ICMP协议
   *           IPPROTO_IP：IP协议
   */
	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  if(listen_fd == -1){
		printf("create socket error: %s(error: %d)\n", strerror(errno), errno);
		exit(0);
	}

  /**
   * <arpa/inet.h>中定义了一些函数和宏，用于将主机字节序转换为网络字节序
   */
  uint32_t value = 0x11223344;
  uint8_t* value1_ptr = reinterpret_cast<uint8_t*>(&value);
  uint8_t* value2_ptr = reinterpret_cast<uint8_t*>(&value) + 1;
  uint8_t* value3_ptr = reinterpret_cast<uint8_t*>(&value) + 2;
  uint8_t* value4_ptr = reinterpret_cast<uint8_t*>(&value) + 3;
  printf("value: %x.%x.%x.%x\n", *value1_ptr, *value2_ptr, *value3_ptr, *value4_ptr);
  in_addr_t ip_addr = inet_addr("192.168.2.2");
  uint8_t ip_addr1 = ip_addr & 0xFF;
  uint8_t ip_addr2 = (ip_addr >> 8) & 0xFF;
  uint8_t ip_addr3 = (ip_addr >> 16) & 0xFF;
  uint8_t ip_addr4 = (ip_addr >> 24) & 0xFF;
  printf("ip_addr: %u.%u.%u.%u\n", ip_addr1, ip_addr2, ip_addr3, ip_addr4);
  uint16_t port = htons(0x8000);
  uint16_t port1 = port & 0xFF;
  uint16_t port2 = (port >> 8) & 0xFF;
  printf("port: %x.%x\n", port1, port2);
  uint32_t addr = htonl(0x80706050);
  uint32_t addr1 = addr & 0xFF;
  uint32_t addr2 = (addr >> 8) & 0xFF;
  uint32_t addr3 = (addr >> 16) & 0xFF;
  uint32_t addr4 = (addr >> 24) & 0xFF;
  printf("addr: %x.%x.%x.%x\n", addr1, addr2, addr3, addr4);

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
     * @note 即使socket被设置为非阻塞，此时发送缓冲区满了，write也会返回-1，并设置errno为EAGAIN或EWOULDBLOCK
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