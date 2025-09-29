#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <unistd.h>

/**
 * struct iphdr {
 * #if __BYTE_ORDER == __LITTLE_ENDIAN
 *     unsigned int ihl:4; // IP头长度
 *     unsigned int version:4; // 版本号
 * #elif __BYTE_ORDER == __BIG_ENDIAN
 *     unsigned int version:4;
 *     unsigned int ihl:4;
 * #else
 * # error "Please fix <bits/endian.h>"
 * #endif
 *     uint8_t tos; // 服务类型
 *     uint16_t tot_len; // 总长度
 *     uint16_t id; // 标识
 *     uint16_t frag_off; // 标志和片偏移
 *     uint8_t ttl; // 生存时间
 *     uint8_t protocol; // 协议
 *     uint16_t check; // 头部校验和
 *     uint32_t saddr; // 源地址
 *     uint32_t daddr; // 目的地址
 *     // 选项字段（如果有）
 * };
 */

int main() {
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    /**
     * 不是必须进行bind操作，因为原始套接字会接收所有符合协议的报文
     * 但如果需要监听特定的接口或IP地址，可以使用bind进行绑定
     */
    sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sockfd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sockfd);
        return 1;
    }

    char buffer[2048];
    while (true) {
        ssize_t len = recv(sockfd, buffer, sizeof(buffer), 0);
        if (len < 0) {
            perror("recv");
            break;
        }
        // 解析IP头
        struct iphdr* iph = (struct iphdr*)buffer;
        int iphdr_len = iph->ihl * 4;
        std::cout << "IP Version: " << (int)iph->version
                  << ", Header Length: " << (int)iph->ihl * 4
                  << ", TTL: " << (int)iph->ttl
                  << ", tos: " << (int)iph->tos
                  << ", Total Length: " << ntohs(iph->tot_len)
                  << ", saddr: " << inet_ntoa(*(in_addr*)&iph->saddr)
                  << ", daddr: " << inet_ntoa(*(in_addr*)&iph->daddr)
                  << ", Protocol: " << (int)iph->protocol
                  << std::endl;
        // 只处理ICMP
        if (iph->protocol != IPPROTO_ICMP) continue;

        // 解析ICMP头
        /**
         * struct icmphdr {
         *     uint8_t type;      // ICMP类型
         *     uint8_t code;     // ICMP代码
         *     uint16_t checksum; // 校验和
         *     union {
         *         struct {
         *             uint16_t id;
         *             uint16_t sequence;
         *         } echo;
         *         uint32_t gateway;
         *         struct {
         *             uint16_t __unused;
         *             uint16_t mtu;
         *         } frag;
         *     } un;
         * };
         */

        struct icmphdr* icmph = (struct icmphdr*)(buffer + iphdr_len);

        // 打印ICMP类型
        std::cout << "ICMP type: " << (int)icmph->type
                  << ", code: " << (int)icmph->code << std::endl;

        // 打印源IP和目标IP
        struct in_addr src, dst;
        src.s_addr = iph->saddr;
        dst.s_addr = iph->daddr;
        std::cout << "Src IP: " << inet_ntoa(src)
                  << ", Dst IP: " << inet_ntoa(dst) << std::endl;

        // 只处理Echo请求和应答
        if (icmph->type == ICMP_ECHO)
            std::cout << "ICMP Echo Request" << std::endl;
        else if (icmph->type == ICMP_ECHOREPLY)
            std::cout << "ICMP Echo Reply" << std::endl;
        else
            std::cout << "Other ICMP type" << std::endl;

        std::cout << "------------------------" << std::endl;
    }
    close(sockfd);
    return 0;
}