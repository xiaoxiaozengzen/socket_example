#include <ctype.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define MAXLINE 80

const char *socket_path = "server.socket";

int main(void) {
  /**
   * struct sockaddr_un {
   *  sa_family_t sun_family; // Address family, AF_UNIX
   *  char sun_path[108]; // Path name for the UNIX domain socket
   *  };
   *
   * typedef unsigned short int sa_family_t;
   */
  struct sockaddr_un ser_un;
  struct sockaddr_un cli_un;
  socklen_t cli_un_len;
  int listen_fd = -1;
  int conn_fd = -1;
  int size = 0;
  char buf[MAXLINE];

  listen_fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (listen_fd < 0) {
    perror("socket error");
    exit(1);
  }

  memset(&ser_un, 0, sizeof(ser_un));
  ser_un.sun_family = AF_UNIX;
  strncpy(ser_un.sun_path, socket_path, sizeof(ser_un.sun_path) - 1);
  size = offsetof(struct sockaddr_un, sun_path) + strlen(ser_un.sun_path);
  unlink(socket_path);

  /**
   * @brief 当服务端调用bind函数，并且地址是AF_UNIX类型时，
   *        系统会自动创建一个名为sun_path的文件。
   *        这个文件的作用是作为UNIX域套接字的地址。
   *        当服务端调用listen函数时，系统会将这个文件设置为监听状态。
   *        当客户端连接到这个套接字时，系统会创建一个新的
   *        文件描述符，并将其与这个套接字关联。
   *        这个文件描述符可以用于与客户端进行通信。
   *        注意：如果之前已经存在同名的文件，调用bind函数会失败
   *        因此在调用bind之前，通常会先调用unlink函数删除同
   */
  int ret = bind(listen_fd, (struct sockaddr *)&ser_un, size);
  if (ret < 0) {
    perror("bind error");
    exit(1);
  }
  printf("UNIX domain socket bound\n");

  ret = listen(listen_fd, 20);
  if (ret < 0) {
    perror("listen error");
    exit(1);
  }
  printf("Accepting connections ...\n");

  while (1) {
    cli_un_len = sizeof(cli_un);
    conn_fd = accept(listen_fd, (struct sockaddr *)&cli_un, &cli_un_len);
    if (conn_fd < 0) {
      perror("accept error");
      continue;
    }
    printf("Accepted connection from %s\n", cli_un.sun_path);

    /**
     * 获取对端进程的凭据，只能用于UNIX域套接字
     * struct ucred {
     *  pid_t pid; // 进程ID
     *  uid_t uid; // 用户ID
     *  gid_t gid; // 组ID
     * };
     */
    struct ucred cred;
    socklen_t cred_len = sizeof(cred);
    if (getsockopt(conn_fd, SOL_SOCKET, SO_PEERCRED, &cred, &cred_len) == 0) {
      printf("Peer PID: %d, UID: %d, GID: %d\n", cred.pid, cred.uid, cred.gid);
    } else {
      perror("getsockopt SO_PEERCRED");
    }

    while (1) {
      int n = read(conn_fd, buf, sizeof(buf));
      if (n < 0) {
        perror("read error");
        break;
      } else if (n == 0) {
        printf("EOF\n");
        break;
      }
      buf[n] = '\0'; // 确保字符串以null结尾

      printf("received: %s\n", buf);

      for (int i = 0; i < n; i++) {
        buf[i] = toupper(buf[i]);
      }
      write(conn_fd, buf, n);
      printf("sent: %s\n", buf);
    }
    close(conn_fd);
  }
  close(listen_fd);
  return 0;
}