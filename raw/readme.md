# Overview

AF_INET 和 PF_PACKET 的区别如下：

* AF_INET（Address Family INET）
  * 1. 代表 IPv4 网络协议族。
  * 2. 用于操作系统的“网络层”套接字，主要用于 TCP、UDP、原始IP包等。
  * 3. 只能收发 IP 层及以上的数据包（如 IP、TCP、UDP），不能直接操作以太网帧头。
  * 4. 典型用法：socket(AF_INET, SOCK_STREAM, 0)（TCP），socket(AF_INET, SOCK_DGRAM, 0)（UDP），socket(AF_INET, SOCK_RAW, ...)（原始IP包）。
* PF_PACKET（Packet Family）
  * 1. 代表“数据链路层”协议族。
  * 2. 允许直接收发以太网帧（包括以太网头部），可用于抓包、构造和发送自定义以太网帧。
  * 3. 可以捕获所有经过网卡的原始数据包（包括非IP协议），常用于抓包工具（如 tcpdump、Wireshark）。
  * 4. 典型用法：socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))

# IP层协议格式

IP的头格式如下：

```bash
0                   1                   2                   3
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|Version|  IHL  |Type of Service|          Total Length         |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|         Identification        |Flags|      Fragment Offset    |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  Time to Live |    Protocol   |         Header Checksum       |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                       Source Address                          |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                    Destination Address                        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                    Options                    |    Padding    |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

字段含义：
* version：4表示ipv4，6表示ipv6
* IHL：首部长度，如果不带options，则为20，最长为60。以4字节为一个单位
* ToS：服务类型，只有在QoS差分服务要求时这个字段才起作用
* ToL：总长度，整个IP报文的长度，包括首部和数据之和，单位为字节，最长为65535。不能超过MTU
* identification：标识，主机每发送一个报文，加1。分片重组的时候需要
* flags：标志位，跟上边的分片结合使用
* Fragment offest：分片偏移
* ttl：生存时间，可经过的最多路由数量
* protocol：协议：下一层协议
  * 1. 0: 保留
  * 2. 1：ICMP
  * 3. 6：TCP
* crc：
* source address：原ip地址
* destination address：目的ip地址
* options：选项字段，用来支持排错、测量以及安全等措施
* padding：填充字段，全填0

## ICMP协议格式

ICMP报文的通用格式如下：

```bash
+0------7-------15---------------31
|  Type | Code  |    Checksum    |
+--------------------------------+
|          Message Body          |
|        (Variable length)       |
+--------------------------------+
```

其中：
* type：报文类型
* code：代码，提供报文类型的进一步信息
  * type-0，code-0：回显应答(ping应答)
  * type-3，code-0：网络不可达
  * type-3，code-1：主机不可达
* crc
* message body：字段的长度和内容，取决于消息的类型跟代码