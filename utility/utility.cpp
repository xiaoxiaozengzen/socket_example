#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <stddef.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdint.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/file.h>

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

void fstat_example() {
  const char* filename = "/mnt/workspace/cgz_workspace/Exercise/socket_example/utility/CMakeLists.txt";

  int fd = open(filename, O_RDONLY);
  if(fd == -1) {
    perror("open failed");
    return;
  }

  /**
   * struct stat {
   *     dev_t     st_dev;     // 设备ID
   *     ino_t     st_ino;     // 索引节点号
   *     mode_t    st_mode;    // 文件类型和权限
   *     nlink_t   st_nlink;   // 硬链接数
   *     uid_t     st_uid;     // 所有者用户ID
   *     gid_t     st_gid;     // 所有者组ID
   *     dev_t     st_rdev;    // 设备类型（如果是特殊文件）
   *     off_t     st_size;    // 文件大小（字节数）
   *     blksize_t st_blksize; // 文件系统块大小
   *     blkcnt_t  st_blocks;  // 分配的块数
   *     time_t    st_atime;   // 最后访问时间
   *     time_t    st_mtime;   // 最后修改时间
   *     time_t    st_ctime;   // 最后状态改变时间
   * };
   * @brief 获取文件状态信息
   */
  struct stat sb;

  /**
   * int fstat(int fd, struct stat *buf);
   * @brief 获取文件描述符fd对应的文件状态信息
   * @param fd: 文件描述符
   * @param buf: 指向stat结构体的指针，用于存储文件
   * 
   * @note fstat函数不会改变文件的访问时间
   * @return 成功返回0，失败返回-1并设置errno
   * 
   * @note stat函数是获取文件路径对应的文件状态信息，而fstat函数是获取文件描述符对应的文件状态信息。
   */
  int ret = fstat(fd, &sb);
  if(ret == -1) {
    perror("fstat failed");
    close(fd); // 关闭文件描述符
    return; 
  }

  printf("stat st_dev: %ld\n", sb.st_dev);
  printf("stat st_ino: %ld\n", sb.st_ino);

  /**
   * int st_mode: 文件类型和权限
   * @note st_mode的高位表示文件类型，低位表示文件权限
   * @note 文件类型包括：S_IFREG(普通文件)、S_IFDIR(目录)、S_IFCHR(字符设备)、S_IFBLK(块设备)、S_IFIFO(管道)、S_IFLNK(符号链接)、S_IFSOCK(套接字)等
   * @note 文件权限包括：S_IRUSR(用户可读)、 S_IWUSR(用户可写)、S_IXUSR(用户可执行)、S_IRGRP(组可读)、S_IWGRP(组可写)、S_IXGRP(组可执行)、S_IROTH(其他用户可读)、S_IWOTH(其他用户可写)、S_IXOTH(其他用户可执行)
   * @note st_mode的值可以通过按位与操作来检查文件类型和权限
   */
  printf("stat st_mode: %o\n", sb.st_mode);
  if(S_IFREG & sb.st_mode) {
    printf("File type: Regular file\n");
  }

  printf("stat st_nlink: %ld\n", sb.st_nlink);
  printf("stat st_uid: %d\n", sb.st_uid);
  printf("stat st_gid: %d\n", sb.st_gid);
  printf("stat st_rdev: %ld\n", sb.st_rdev);
  printf("stat st_size: %ld bytes\n", sb.st_size);
  printf("stat st_blksize: %ld bytes\n", sb.st_blksize);
  printf("stat st_blocks: %ld\n", sb.st_blocks);
  printf("stat st_atime: %ld\n", sb.st_atime);
  printf("stat st_mtime: %ld\n", sb.st_mtime);
  printf("stat st_ctime: %ld\n", sb.st_ctime);

  close(fd); // 关闭文件描述符
}

void ftruncate_example() {
  /**
   * int truncate(const char *path, off_t length);
   * @brief 截断文件到指定长度
   * @param path: 要截断的文件路径
   * @param length: 截断后的文件长度
   * @return 成功返回0，失败返回-1并设置errno
   * 
   * @note 如果length小于当前文件大小，则会丢弃超出部分的数据
   * @note 如果length大于当前文件大小，则会在文件末尾填充空字节
   * 
   * @note ftruncate函数参数是文件描述符，而truncate函数参数是文件路径
   */
  const char* file_path = "/mnt/workspace/cgz_workspace/Exercise/socket_example/utility/output/test.txt";
  int ret = truncate(file_path, 5);
  if(ret == -1) {
    perror("truncate failed");
  } else {
    printf("File %s truncated to 5 bytes successfully.\n", file_path);
  }
}

void fopen_example() {
    /**
     * FILE* fopen(const char *filename, const char *mode);
     * @brief 打开一个文件
     * @param filename: 要打开的文件名
     * @param mode: 打开模式，如"r"（只读）、"w"（写入）、"a"（追加）等
     * @return 成功返回指向FILE对象的指针，失败返回NULL并设置errno
     * 
     * @note 打开模式包括：
     * "r": 只读模式，文件必须存在，不会清空文件内容
     * "w": 写入模式，如果文件存在则清空内容，如果不存在则创建新文件
     * "a": 追加模式，如果文件存在则在末尾追加内容，如果不存在则创建新文件
     * "r+": 读写模式，文件必须存在，不会清空文件内容
     * "w+": 读写模式，如果文件存在则清空内容，如果不存在则创建新文件
     * "a+": 读写追加模式，如果文件存在则在末尾追加内容，如果不存在则创建新文件
     * "b": 二进制模式，可以与其他模式组合使用，如"rb"（只读二进制）、 "wb"（写入二进制）、"ab"（追加二进制）等
     * "t": 文本模式，可以与其他模式组合使用，如"rt"（只读文本）、"wt"（写入文本）、"at"（追加文本）等
     */
    FILE* fp = fopen("/mnt/workspace/cgz_workspace/Exercise/socket_example/utility/output/test.txt", "r+");
    if(fp == NULL) {
        perror("fopen failed");
        return;
    }

    /**
     * typedef struct _IO_FILE FILE;
     * struct _IO_FILE {
     *     int _flags; // 文件状态标志
     *     char *_IO_read_ptr; // 读取指针
     *     char *_IO_read_end; // 读取结束指针
     *     char *_IO_read_base; // 读取缓冲区起始地址
     *     char *_IO_write_base; // 写入缓冲区起始地址
     *     char *_IO_write_ptr; // 写入指针
     *     char *_IO_write_end; // 写入结束指针
     *     char *_IO_buf_base; // 缓冲区起始地址
     *     char *_IO_buf_end; // 缓冲区结束地址
     *     char *_IO_save_base; // 保存的缓冲区起始地址
     *     char *_IO_backup_base; // 备份的缓冲区起始地址
     *     char *_IO_save_end; // 保存的缓冲区结束地址
     *     struct _IO_marker *_markers; // 标记列表
     *     struct _IO_FILE *_chain; // 链接到下一个文件对象
     *     int _fileno; // 文件描述符
     *     int _flags2; // 额外的文件状态标志
     *     __off_t _old_offset; // 旧的偏移量
     *     unsigned short _cur_column; // 当前列号
     *     signed char _vtable_offset; // 虚函数表偏移量
     *     char _shortbuf[1]; // 短缓冲区
     *     _IO_lock_t *_lock; // 文件锁
     *     __off64_t _offset; // 文件偏移量
     *     struct _IO_codecvt *_codecvt; // 编码转换结构体
     *     struct _IO_wide_data *_wide_data; // 宽字符数据结构
     *     struct _IO_FILE *_freeres_list; // 释放资源列表
     *     void *_freeres_buf; // 释放资源缓冲区
     *     size_t __pad5; // 填充字节
     *     int _mode; // 文件模式
     *     char _unused2[15 * sizeof(int) - 4 * sizeof(void *) - sizeof(size_t)]; // 未使用的填充字节
     * };
     * @note FILE结构体用于表示打开的文件，包含了文件状态、缓冲区、文件描述符等信息。
     */

    printf("FILE pointer: %p\n", fp);
    printf("FILE _flags: %x\n", fp->_flags);
    
    // 读取文件内容
    char buf[100];
    /**
     * int fread(void *ptr, size_t size, size_t count, FILE *stream);
     * @brief 从文件流中读取数据
     * @param ptr: 指向存储读取数据的缓冲区
     * @param size: 每个元素的大小（字节数）
     * @param count: 要读取的元素数量
     * @param stream: 指向FILE对象的指针
     * @return: 成功返回实际读取的元素数量，失败返回0并设置errno
     */
    int bytes_read = fread(buf, 1, sizeof(buf) - 1, fp);
    if(bytes_read < 0) {
        perror("fread failed");
    } else {
        buf[bytes_read] = '\0'; // 确保字符串以null结尾
        printf("Read %d bytes: %s\n", bytes_read, buf);
    }

    // 写入文件内容
    const char* write_data = "Hello, World!";
    /**
     * int fwrite(const void *ptr, size_t size, size_t count, FILE *stream);
     * @brief 向文件流中写入数据
     * @param ptr: 指向要写入的数据的缓冲区
     * @param size: 每个元素的大小（字节数）
     * @param count: 要写入的元素数量
     * @param stream: 指向FILE对象的指针
     * @return: 成功返回实际写入的元素数量，失败返回0并设置errno
     */
    int bytes_written = fwrite(write_data, 1, strlen(write_data), fp);
    if(bytes_written < 0) {
        perror("fwrite failed");
    } else {
        printf("Wrote %d bytes: %s\n", bytes_written, write_data);
    }

    fclose(fp);
}

void opendir_example() {
    /**
     * DIR* opendir(const char *name);
     * @brief 打开一个目录
     * @param name: 要打开的目录路径
     * @return 成功返回指向DIR对象的指针，失败返回NULL并设置errno
     * 
     * @note DIR结构体用于表示打开的目录，包含了目录状态、缓冲区等信息。
     */
    const char* dir_path = "/mnt/workspace/cgz_workspace/Exercise/socket_example/utility/output";
    DIR* dir = opendir(dir_path);
    if(dir == NULL) {
        perror("opendir failed");
        return;
    }

    /**
     * struct dirent {
     *     ino_t          d_ino;       // inode号
     *     off_t          d_off;       // 目录项在该目录流中的偏移
     *     unsigned short d_reclen;    // 目录项长度
     *     unsigned char  d_type;      // 类型（文件/目录等）
     *     char           d_name[256]; // 文件名
     * };
     * @brief dirent结构体用于表示目录项，包含了inode号、目录项偏移、目录项长度、类型和文件名等信息。
     */
    struct dirent* entry;
    printf("dirent d_type: %d is reg\n", DT_REG);
    printf("dirent d_type: %d is dir\n", DT_DIR);
    printf("dirent d_type: %d is link\n", DT_LNK);
    printf("dirent d_type: %d is sock\n", DT_SOCK);
    printf("dirent d_type: %d is fifo\n", DT_FIFO);
    printf("dirent d_type: %d is char\n", DT_CHR);
    printf("dirent d_type: %d is blk\n", DT_BLK);
    printf("dirent d_type: %d is unknown\n", DT_UNKNOWN);
    while((entry = readdir(dir)) != NULL) {
        printf("d_ino: %lu, d_off: %ld, d_reclen: %hu, d_type: %u, d_name: %s\n",
               (unsigned long)entry->d_ino, entry->d_off, entry->d_reclen,
               entry->d_type, entry->d_name);
    }

    closedir(dir);
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
    printf("======================= fstat Example =======================\n");
    fstat_example();
    printf("======================= ftruncate Example =======================\n");
    ftruncate_example();
    printf("======================= fopen Example =======================\n");
    fopen_example();
    printf("======================= opendir Example =======================\n");
    opendir_example();
    return 0;
}