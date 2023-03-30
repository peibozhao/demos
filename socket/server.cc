
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <iostream>

const short kPort = 5000;

int main(int argc, char *argv[]) {
  int listenfd = 0, connfd = 0;
  struct sockaddr_in serv_addr;

  char buffer[1024 * 10];

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
  connfd = accept(listenfd, (struct sockaddr *)NULL, NULL);
  std::cout << "accept: " << connfd << std::endl;

  while (1) {
    ret = write(connfd, buffer, sizeof(buffer));
    std::cout << "write: " << ret << std::endl;

    sleep(1);
  }
}
