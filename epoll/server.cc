
#include <arpa/inet.h>
#include <chrono>
#include <errno.h>
#include <iomanip>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <thread>
#include <sys/epoll.h>
#include <sys/ioctl.h>

const short kPort = 5000;

int main(int argc, char *argv[]) {
  int listenfd = 0, connfd = 0;
  struct sockaddr_in serv_addr;

  char buffer[1024 * 10];
  uint64_t timestamp;

  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  std::cout << "socket: " << listenfd << std::endl;
  memset(&serv_addr, 0, sizeof(serv_addr));
  memset(buffer, 0, sizeof(buffer));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(kPort);

  int ret = 0;
  ret = bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  std::cout << "bind: " << ret << std::endl;
  ret = listen(listenfd, 10);
  std::cout << "listen: " << ret << std::endl;

  while (true) {
    connfd = accept(listenfd, (struct sockaddr *)NULL, NULL);
    std::cout << "accept: " << connfd << std::endl;

    int poll_fd = epoll_create(1);
    std::cout << "epoll_create: " << poll_fd << std::endl;
    struct epoll_event reg_event;
    reg_event.events = EPOLLIN;
    reg_event.data.fd = connfd;
    ret = epoll_ctl(poll_fd, EPOLL_CTL_ADD, connfd, &reg_event);
    std::cout << "epoll_ctl: " << ret << std::endl;

    uint64_t lastnow = 0;
    while (1) {
      struct epoll_event event;
      ret = epoll_wait(poll_fd, &event, 1, -1);
      std::cout << "epoll_wait: " << ret << std::endl;
      std::cout << "event: " << event.events << " " << event.data.fd << std::endl;
      int recv_len = 0;
      ret = ioctl(connfd, FIONREAD, &recv_len);
      std::cout << "ioctl: " << ret << " " << recv_len << std::endl;
      ret = recv(connfd, &timestamp, sizeof(timestamp), 0);
      std::cout << "recv: " << ret << " " << timestamp << std::endl;
      if (ret <= 0) {
        break;
      }
    }
    close(connfd);
    close(poll_fd);
  }
}
