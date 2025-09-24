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
  int shm_fd = shm_open("my_shm", O_RDWR, 0666);
  if (shm_fd == -1) {
    perror("shm_open failed");
    return;
  }

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

  sem_wait(&data->sem); // 等待客户端处理完数据
  std::cout << "Client read from shared memory: " << data->buf << std::endl;

  strcpy(data->buf, "World from client via shared memory!");
  std::cout << "Client wrote to shared memory: " << data->buf << std::endl;
  sem_post(&data->sem); // 释放信号量，通知客户端数据已写入
}

int main() {
  special_file_example();

  return 0;
}