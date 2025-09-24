#include <iostream>

#include <sys/sendfile.h>
#include <fcntl.h>
#include <unistd.h>

/**
 * ssize_t sendfile(int out_fd,int in_fd,off_t* offset,size_t count);
 * @brief sendfile 直接在内核空间完成文件到 socket 的数据传输，避免了用户态和内核态之间的多次切换
 * @param out_fd 目标文件描述符，通常是 socket
 * @param in_fd 源文件描述符，通常是普通文件
 * @param offset 指向文件偏移量的指针，如果为 NULL，则从读入文件默认偏移量处读数据，
 * @param count 要发送的字节数
 * @return int 
 */

int main() {
  std::string input_file = "/mnt/workspace/cgz_workspace/Exercise/socket_example/mmap/output/shared";
  std::string output_file = "/mnt/workspace/cgz_workspace/Exercise/socket_example/mmap/output/socket";

  int in_fd = open(input_file.c_str(), O_RDONLY);
  int out_fd = open(output_file.c_str(), O_WRONLY | O_CREAT, 0666);

  if (in_fd == -1 || out_fd == -1) {
    perror("open failed");
    return 1;
  }

  off_t offset = 0;
  ssize_t bytes_sent = sendfile(out_fd, in_fd, &offset, 1024);

  if (bytes_sent == -1) {
    perror("sendfile failed");
    close(in_fd);
    close(out_fd);
    return 1;
  }

  std::cout << "Sent " << bytes_sent << " bytes from " << input_file << " to " << output_file << std::endl;

  close(in_fd);
  close(out_fd);

  return 0;
}