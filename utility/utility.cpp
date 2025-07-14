#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <stddef.h>

/**
 * @brief arpa/inet.h主要是提供了网络地址转换，以及字节序转换的函数。 
 */

void ipaddr_example() {
    /**
     * in_addr_t inet_addr(const char *cp);
     * @brief 将点分十进制的IPv4地址转换为网络字节序
     * @param cp: 点分十进制的IP地址字符串
     * @return: 返回一个32位的整数，表示网络字节序的IP地址
     * @note: 如果转换失败，则返回INADDR_NONE(0xFFFFFFFF)
     */
    const char* vaild_ip = "10.11.12.13";
    const char* invalid_ip = "10.11.12";
    in_addr_t valid_addr = inet_addr(vaild_ip);
    if(valid_addr == INADDR_NONE) {
        printf("Invalid IP address: %s\n", vaild_ip);
    } else {
        printf("Valid IP address: %s, in network byte order: %x\n", vaild_ip, valid_addr);
        uint8_t* addr_bytes_1 = (uint8_t*)&valid_addr;
        uint8_t* addr_bytes_2 = (uint8_t*)&valid_addr + 1;
        uint8_t* addr_bytes_3 = (uint8_t*)&valid_addr + 2;
        uint8_t* addr_bytes_4 = (uint8_t*)&valid_addr + 3;
        printf("IP address bytes: %u.%u.%u.%u\n", 
               *addr_bytes_1, *addr_bytes_2, *addr_bytes_3, *addr_bytes_4);
    }
    in_addr_t invalid_addr = inet_addr(invalid_ip);
    if(invalid_addr == INADDR_NONE) {
        printf("Invalid IP address: %s\n", invalid_ip);
    } else {
        printf("Valid IP address: %s, in network byte order: %x\n", invalid_ip, invalid_addr);
    }

    /**
     * int inet_aton(const char *cp, struct in_addr *inp);
     * @brief 将点分十进制的IPv4地址转换为网络字节序
     * @param cp: 点分十进制的IP地址字符串
     * @param inp: 指向in_addr结构体的指针，用于存放转换后的IP地址
     * @return: 成功返回1，失败返回0
     */
    in_addr addr;
    if(inet_aton(vaild_ip, &addr)) {
        printf("inet_aton success: %s, in network byte order: %x\n", vaild_ip, addr.s_addr);
    } else {
        printf("inet_aton failed for IP address: %s\n", vaild_ip);
    }

    /**
     * char* inet_ntoa(struct in_addr in);
     * @brief 将网络字节序的IP地址转换为点分十进制
     * @param in: in_addr结构体，包含网络字节序的IP地址
     * @return: 返回一个指向字符串的指针，表示点分十进制的IP地址
     * @note: 返回的字符串是静态分配的，每次调用都会覆盖之前的结果
     */
    char* ip_str = nullptr;
    ip_str = inet_ntoa(addr);
    if(ip_str) {
        printf("inet_ntoa result: %p %s\n", ip_str, ip_str);
    } else {
        printf("inet_ntoa failed for IP address: %x\n", addr.s_addr);
    }
    char* ip_str2 = inet_ntoa(addr);
    if(ip_str2) {
        printf("inet_ntoa result again: %p %s\n", ip_str2, ip_str2);
    } else {
        printf("inet_ntoa failed for IP address: %x\n", addr.s_addr);
    }

    /**
     * const char* inet_ntop(int af, const void *src, char *dst, socklen_t size);
     * @brief 将网络字节序的IP地址转换为点分十进制
     * @param af: 地址族，AF_INET表示IPv4，AF_INET6表示IPv6
     * @param src: 指向网络字节序IP地址的指针
     * @param dst: 指向存放转换后字符串的缓冲区
     * @param size: 缓冲区大小
     * @return: 成功返回dst指针，失败返回NULL
     * @note: 如果af为AF_INET6，则src指向的应该是一个16字节的IPv6地址
     * @note: dst缓冲区必须足够大，至少要能存放INET_ADDRSTRLEN(16)个字符
     */
    char ip_buffer[INET_ADDRSTRLEN];
    const char* result = inet_ntop(AF_INET, &addr, ip_buffer, sizeof(ip_buffer));
    if(result) {
        printf("inet_ntop result: %s\n", ip_buffer);
    } else {
        printf("inet_ntop failed for IP address: %x\n", addr.s_addr);
    }

    /**
     * int inet_pton(int af, const char *src, void *dst);
     * @brief 将点分十进制的IP地址转换为网络字节序
     * @param af: 地址族，AF_INET表示IPv4，AF_INET6表示IPv6
     * @param src: 点分十进制的IP地址字符串
     * @param dst: 指向存放转换后IP地址的缓冲区
     * @return: 成功返回1，失败返回0或-1
     * @note: 如果af为AF_INET6，则src应该是一个IPv6地址
     * @note: dst缓冲区必须足够大，至少要能存放sizeof(struct in_addr)个字节
     */
    in_addr addr2;
    int ret = inet_pton(AF_INET, vaild_ip, &addr2);
    if(ret == 1) {
        printf("inet_pton success: %s, in network byte order: %x\n", vaild_ip, addr2.s_addr);
    } else if(ret == 0) {
        printf("inet_pton failed: %s is not a valid IP address\n", vaild_ip);
    } else {
        printf("inet_pton error: %s\n", strerror(errno));
    }
    ret = inet_pton(AF_INET, invalid_ip, &addr2);
    if(ret == 1) {
        printf("inet_pton success: %s, in network byte order: %x\n", invalid_ip, addr2.s_addr);
    } else if(ret == 0) {
        printf("inet_pton failed: %s is not a valid IP address\n", invalid_ip);
    } else {
        printf("inet_pton error: %s\n", strerror(errno));
    }
}

void byte_order_example() {
    const char* ip_addr_ctr = "10.11.12.13";
    in_addr addr;
    int ret = inet_aton(ip_addr_ctr, &addr);
    if(ret == 0) {
        printf("inet_aton failed for IP address: %s\n", ip_addr_ctr);
        return;
    }

    /**
     * uint32_t ntohl(uint32_t netlong);
     * @brief 将网络字节序的32位整数转换为主机字节序
     * @param netlong: 网络字节序的32位整数
     * @return: 返回主机字节序的32位整数
     * @note: 如果主机是小端字节序，则返回的整数字符的字节顺序会被反转
     * @note: 如果主机是大端字节序，则返回的整数字符的字节顺序不变
     */
    uint32_t netlong = addr.s_addr;
    uint32_t hostlong = ntohl(netlong);
    printf("Network byte order: %x, Host byte order: %x\n", netlong, hostlong);

    /**
     * uint32_t htonl(uint32_t hostlong);
     * @brief 将主机字节序的32位整数转换为网络字节序
     * @param hostlong: 主机字节序的32位整数
     * @return: 返回网络字节序的32位整数
     */
    uint32_t converted_netlong = htonl(hostlong);
    printf("Converted back to network byte order: %x\n", converted_netlong);

    /**
     * uint16_t ntohs(uint16_t netshort);
     * uint16_t htons(uint16_t hostshort);
     */
}

struct A {
    int a;
    char b;
    short c;
};

void offest_example() {
    /**
     * offsetof(type, member)宏用于获取结构体成员的偏移量
     * @param type: 结构体类型
     * @param member: 结构体成员名
     * @return: 返回成员相对于结构体起始地址的偏移量
     */
    int size_of_a = sizeof(A);
    printf("Size of struct A: %d\n", size_of_a);
    size_t offset_a = offsetof(A, a);
    size_t offset_b = offsetof(A, b);
    size_t offset_c = offsetof(A, c);
    printf("Offset of member 'a': %zu\n", offset_a);
    printf("Offset of member 'b': %zu\n", offset_b);
    printf("Offset of member 'c': %zu\n", offset_c);
}

void unlink_example() {
    /**
     * int unlink(const char *pathname);
     * @brief 删除一个文件或符号链接
     * @param pathname: 要删除的文件或符号链接的路径
     * @return: 成功返回0，失败返回-1并设置errno
     * EROFS: 文件存在于只读文件系统中
     * EFAULT: 文件系统不支持删除操作
     * ENOENT: 文件不存在
     * ENOTDIR: 路径中的某个组件不是目录
     * ENAMETOOLONG: 路径名过长
     * 
     * @note 使用unlink函数删除文件的时候，只会删除目录项 ，并且将inode节点的硬链接数目减一而已，并不一定会释放inode节点。
     *       
     */
    const char* file_path = "/mnt/workspace/cgz_workspace/Exercise/socket_example/build/test.txt";
    int ret = unlink(file_path);
    if(ret == 0) {
        printf("File %s deleted successfully.\n", file_path);
    } else {
        printf("Failed to delete file %s: %s\n", file_path, strerror(errno));
    }
}

void fgets_example() {
    /**
     * char* fgets(char *str, int n, FILE *stream);
     * @brief 从指定的文件流中读取一行字符
     * @param str: 指向存储读取内容的缓冲区
     * @param n: 缓冲区大小
     * @param stream: 指向文件流的指针
     * @return: 成功返回str指针，失败返回NULL
     * 
     * @note fgets会读取直到遇到换行符、文件结束符或读取n-1个字符为止，并在末尾添加null字符'\0'。
     */
    char buf[5];
    if(fgets(buf, sizeof(buf), stdin) != NULL) {
        printf("Read from stdin with len %ld: %s\n", strlen(buf), buf);
        if(buf[strlen(buf) - 1] == '\n') {
            printf("the %ld character is '\\n'\n", strlen(buf));
        }
    } else {
        printf("Failed to read from stdin: %s\n", strerror(errno));
    }
}

int main(void) {
    printf("======================= IP Address Example =======================\n");
    ipaddr_example();
    printf("======================= Byte Order Example =======================\n");
    byte_order_example();
    printf("======================= Offset Example =======================\n");
    offest_example();
    printf("======================= Unlink Example =======================\n");
    unlink_example();
    printf("======================= fgets Example =======================\n");
    fgets_example();
    return 0;
}