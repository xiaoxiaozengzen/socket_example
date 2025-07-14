#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <signal.h>

#define MAXLINE 80

const char *client_path = "client.socket";
const char *server_path = "server.socket";

void signal_handler(int sig) {
  if (sig == SIGINT || sig == SIGTERM || sig == SIGPIPE) {
    printf("Received signal %d, exiting...\n", sig);
    unlink(client_path); // 删除客户端的socket文件
    unlink(server_path); // 删除服务端的socket文件
    exit(0);
  }
}

int main() {
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);

  /**
   * @brief 当服务端关闭连接时，客户端继续write操作会触发SIGPIPE信号，
   *        导致程序异常终止。为了避免这种情况，可以在客户端设置SIGPIPE信号的处理函数，
   */
  signal(SIGPIPE, signal_handler);

  struct sockaddr_un cliun;
  struct sockaddr_un serun;
  int len;
  char buf[100];
  int sockfd;

  if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
    perror("client socket error");
    exit(1);
  }

  // 一般显式调用bind函数，以便服务器区分不同客户端
  memset(&cliun, 0, sizeof(cliun));
  cliun.sun_family = AF_UNIX;
  strcpy(cliun.sun_path, client_path);
  len = offsetof(struct sockaddr_un, sun_path) + strlen(cliun.sun_path);
  unlink(cliun.sun_path);

  if (bind(sockfd, (struct sockaddr *)&cliun, len) < 0) {
    perror("bind error");
    exit(1);
  }

  memset(&serun, 0, sizeof(serun));
  serun.sun_family = AF_UNIX;
  strcpy(serun.sun_path, server_path);
  len = offsetof(struct sockaddr_un, sun_path) + strlen(serun.sun_path);
  if (connect(sockfd, (struct sockaddr *)&serun, len) < 0) {
    perror("connect error");
    exit(1);
  }

  while (fgets(buf, MAXLINE, stdin) != NULL) {
    if (buf[strlen(buf) - 1] == '\n') {
      buf[strlen(buf) - 1] = '\0'; // 去掉换行符
    }
    if (buf[0] == 'q') {
      printf("Client exit.\n");
      break;
    }

    int ret = write(sockfd, buf, strlen(buf));
    if (ret < 0) {
      perror("write error");
      break;
    } else if (ret == 0) {
      printf("the other side has been closed.\n");
      break;
    }
    printf("sent: %s\n", buf);

    int n = read(sockfd, buf, MAXLINE);
    if (n < 0) {
      printf("the other side has been closed.\n");
    } else if (n == 0) {
      printf("the other side has been closed.\n");
      break;
    } else {
      buf[n] = '\0'; // 确保字符串以null结尾
      printf("received: %s\n", buf);
    }
  }
  close(sockfd);
  return 0;
}