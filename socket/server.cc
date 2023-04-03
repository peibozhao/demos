
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
  connfd = accept(listenfd, (struct sockaddr *)NULL, NULL);
  std::cout << "accept: " << connfd << std::endl;

  uint64_t lastnow = 0;
  while (1) {
    uint64_t now = std::chrono::system_clock::now().time_since_epoch().count();
    ret = recv(connfd, &timestamp, sizeof(timestamp), 0);
    std::cout << std::fixed << std::setprecision(3) << "recv: " << ret
              << " time " << timestamp << " now " << (double)now / 1e9
              << std::endl;
    if (lastnow != 0 && double(now - lastnow) / 1e9 > 0.15) {
      abort();
    }
    lastnow = now;
    // sleep(1);
  }
}
