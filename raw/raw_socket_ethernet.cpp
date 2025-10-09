#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <netinet/ip_icmp.h>
#include <linux/if_ether.h>

int main() {
    int sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    unsigned char buffer[2048];
    while (true) {
        ssize_t len = recvfrom(sockfd, buffer, sizeof(buffer), 0, nullptr, nullptr);
        if (len < 0) {
            perror("recvfrom");
            break;
        }

        /**
         * struct ethhdr {
         *     unsigned char h_dest[ETH_ALEN];   // 目的MAC地址（6字节）
         *     unsigned char h_source[ETH_ALEN]; // 源MAC地址（6字节）
         *     __be16 h_proto;                   // 上层协议类型（2字节，网络字节序）
         * };
         * 
         */
        struct ethhdr* eth = (struct ethhdr*)buffer;

        // 只处理IP包
        if (ntohs(eth->h_proto) == ETH_P_IP) {
            struct iphdr* iph = (struct iphdr*)(buffer + sizeof(struct ethhdr));
            struct in_addr src, dst;
            src.s_addr = iph->saddr;
            dst.s_addr = iph->daddr;

            // 只处理ICMP
            if (iph->protocol != IPPROTO_ICMP) continue;

            printf("Src MAC: %02x:%02x:%02x:%02x:%02x:%02x, ",
                eth->h_source[0], eth->h_source[1], eth->h_source[2],
                eth->h_source[3], eth->h_source[4], eth->h_source[5]);
            printf("Dst MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
                eth->h_dest[0], eth->h_dest[1], eth->h_dest[2],
                eth->h_dest[3], eth->h_dest[4], eth->h_dest[5]);
            printf("EtherType: 0x%04x\n", ntohs(eth->h_proto));

            // 解析ICMP头
            int iphdr_len = iph->ihl * 4;
            struct icmphdr* icmph = (struct icmphdr*)(buffer + sizeof(struct ethhdr) + iphdr_len);

            // 打印ICMP类型
            std::cout << "ICMP type: " << (int)icmph->type
                    << ", code: " << (int)icmph->code << std::endl;

            // 只处理Echo请求和应答
            if (icmph->type == ICMP_ECHO)
                std::cout << "ICMP Echo Request" << std::endl;
            else if (icmph->type == ICMP_ECHOREPLY)
                std::cout << "ICMP Echo Reply" << std::endl;
            else
                std::cout << "Other ICMP type" << std::endl;
            
            printf("Src IP: %s, Dst IP: %s\n", inet_ntoa(src), inet_ntoa(dst));
            printf("Protocol: %d\n", iph->protocol);
        }
        printf("------------------------\n");
    }
    close(sockfd);
    return 0;
}