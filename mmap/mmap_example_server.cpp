/**
 * 内存映射：
 * 将内核空间的一段内存映射到用户空间，映射成功后，
 * 用户空间对内存的修改就会反映到内核空间，同样的，内核空间对内存的修改也会反映到用户空间
 * 也可以将内核空间的一段内存映射到多个进程的用户空间，这样还可以实现进程间的通信
 * 
 * 使用场景：
 * 1. 共享内存：多个进程可以通过映射同一块内存区域来实现数据共享
 * 2. 文件映射：将文件内容映射到内存中，可以像操作内存一样操作文件内容
 * 3. 大数据处理：对于大文件或大数据集，使用内存映射可以提高访问速度
 * 4. IPC（进程间通信）：通过映射同一块内存区域，多个进程可以高效地交换数据
 * 
 * mmap:
 * 1. `mmap` 函数用于实现上述所说的内存映射功能
 * 2. 最常见的操作是将某个文件映射到内存中，这样就可以像操作内存一样操作文件内容，
 *    以此避免频繁的磁盘 I/O 操作，提高性能
 */

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <semaphore.h>

#include <iostream>
#include <thread>
#include <chrono>

/**
 * void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
 * @brief 映射文件到内存
 * @param addr 用户进程要映射的用户空间的起始地址，通常为 NULL，表示由内核指定
 * @param length 映射的长度
 * @param prot 映射区域的保护标志，通常为 PROT_READ | PROT_WRITE，表示可读可写
 * @param flags 映射的标志，通常为 MAP_SHARED 或 MAP_PRIVATE
 *              MAP_SHARED 表示映射区域的修改会反映到原文件中，
 *              MAP_PRIVATE 表示映射区域的修改不会反映到原文件中
 * @param fd 要映射的文件描述符，通常通过 open 函数获取
 * @param offset 文件的偏移量，通常为 0，表示从文件的开头开始映射
 * @return 成功时返回映射区域的起始地址
 *         失败时返回 MAP_FAILED，通常是 -1
 * @note 映射成功后，可以通过返回的地址直接访问映射的内存区域，
 *       也可以使用指针操作来读写映射的内容
 * @note 映射区域的大小必须是系统页面大小的整数倍，可以通过 `sysconf(_SC_PAGE_SIZE)` 获取页面大小
 * @note 映射区域的生命周期与文件描述符 fd 相关联，
 *       当文件描述符关闭时，映射区域会被自动解除映射
 * @note 映射区域的大小必须小于或等于文件的大小，否则映射会失败
 */

void regular_file_example() {
  const char* filename = "/mnt/workspace/cgz_workspace/Exercise/socket_example/mmap/output/shared";

  // 打开文件
  int fd = open(filename, O_RDWR);
  if (fd == -1) {
    perror("open failed");
    return;
  }

  // 获取当前的页面大小
  unsigned long page_size = sysconf(_SC_PAGE_SIZE);
  std::cout << "page size: " << page_size << " bytes" << std::endl;

  struct stat sb;
  int ret = fstat(fd, &sb);
  if (ret == -1) {
    perror("fstat failed");
    close(fd); // 关闭文件描述符
    return;
  }

  size_t file_size = sb.st_size;
  std::cout << "file size: " << file_size << " bytes" << std::endl;
  if(file_size < page_size) {
    // 确保文件存在并且有足够的空间
    if (ftruncate(fd, page_size) == -1) {
      perror("ftruncate failed");
      close(fd);
      return;
    }
  }

  char* addr = (char*)mmap(
      nullptr, // 让内核选择映射地址
      page_size, // 映射的长度，至少为页面大小
      PROT_READ | PROT_WRITE, // 可读可写
      MAP_SHARED, // 映射区域的修改会反映到原文件中
      fd, // 文件描述符
      0 // 从文件的开头开始映射
  );

  if (addr == MAP_FAILED) {
    perror("mmap failed");
    close(fd); // 关闭文件描述符
    return;
  }
  printf("mmap success, addr: %p\n", addr);

  // 可以在映射区域写入数据
  char message[13] = "Hello, mmap!";
  memcpy(addr, message, strlen(message)); // 将数据写入映射区域
  const char* message_2 = "world";
  memcpy(addr + 12, message_2, strlen(message_2)); // 将数据写入映射区域

  // 读取映射区域的数据
  char buffer[256];
  memcpy(buffer, addr, 5); // 从映射区域读取数据
  buffer[5] = '\0'; // 确保字符串以 null 结尾
  printf("Read from mmap: %s\n", buffer);

  /**
   * int munmap(void *addr, size_t length);
   * @brief 解除映射
   * @param addr 映射区域的起始地址
   * @param length 映射区域的长度
   * @return 成功返回0，失败返回-1并设置errno
   * @note 解除映射后，映射区域的内容不再可用
   * @note 如果映射区域是共享的，解除映射后其他进程
   *       仍然可以访问该区域的内容
   * @note 解除映射后，文件描述符 fd 仍然有效，
   *       但映射区域的内容不再可用
   */
  munmap(addr, page_size);
  close(fd); // 关闭文件描述符
}

/**
 * @brief 使用特别的文件，例如共享内存文件
 * 
 */
void special_file_example() {
  struct SharedData {
    sem_t sem;
    char buf[1024];
  };

  /**
   * int shm_open(const char *name, int oflag, mode_t mode);
   * @brief 打开或创建一个共享内存对象
   * @param name 共享内存对象的名称
   * @param oflag 打开标志，通常为 O_CREAT | O_RDWR
   * @param mode 共享内存对象的权限，通常为 0666
   * @return 成功返回文件描述符，失败返回 -1 并设置 errno
   * @note shm_open打开的文件都在tempfs文件系统中，例如/dev/shm/，因此name不能带"/"路径
   * 
   */
  int shm_fd = shm_open("my_shm", O_CREAT | O_RDWR, 0666);
  if (shm_fd == -1) {
    perror("shm_open failed");
    return;
  }
  ftruncate(shm_fd, sizeof(SharedData)); // 设置共享内存对象的大小
  SharedData* data = (SharedData*)mmap(
      nullptr,
      sizeof(SharedData),
      PROT_READ | PROT_WRITE,
      MAP_SHARED,
      shm_fd,
      0
  );
  if (data == MAP_FAILED) {
    perror("mmap failed");
    close(shm_fd);

    return;
  }

  sem_init(&data->sem, 1, 0); // 初始化信号量，初始值为0
  strcpy(data->buf, "Hello from server via shared memory!");
  std::cout << "Server wrote to shared memory: " << data->buf << std::endl;
  sem_post(&data->sem); // 释放信号量，通知客户端数据已写入

  while(true) {
    if(strcmp(data->buf, "Hello from server via shared memory!") != 0) {
      break;
    }
  }

  sem_wait(&data->sem); // 等待客户端处理完数据
  std::cout << "Server read from shared memory: " << data->buf << std::endl;

  int ret = shm_unlink("my_shm"); // 删除共享内存对象
  if (ret == -1) {
    perror("shm_unlink failed");
    close(shm_fd);
    return;
  }
  
}

int main() {
  std::cout << "============= regular file_example =============" << std::endl;
  regular_file_example();
  std::cout << "============= special file_example =============" << std::endl;
  special_file_example();

  return 0;
}