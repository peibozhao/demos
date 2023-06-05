
#include <arpa/inet.h>
#include <chrono>
#include <errno.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <errno.h>
#include <string.h>

const std::string ip = "127.0.0.1";
const short kPort = 5000;

int main(int argc, char *argv[]) {
  int sockfd = 0, n = 0;
  char buffer[1024 * 10];
  struct sockaddr_in serv_addr;
  int ret = 0;

  memset(buffer, 0, sizeof(buffer));
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  std::cout << "socket: " << sockfd << std::endl;

  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(kPort);

  ret = inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr);
  std::cout << "inet_pton: " << ret << std::endl;

  ret = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  std::cout << "connect: " << ret << std::endl;

  // uint64_t timestamp = 0;
  uint64_t timestamp[3];
  while (1) {
    timestamp[0] = std::chrono::system_clock::now().time_since_epoch().count();
    timestamp[1] = std::chrono::system_clock::now().time_since_epoch().count();
    timestamp[2] = std::chrono::system_clock::now().time_since_epoch().count();
    ret = send(sockfd, &timestamp, sizeof(timestamp), MSG_NOSIGNAL);
    std::cout << "write: " << ret << " " << errno << " " << strerror(errno) << std::endl;
    // std::this_thread::sleep_for(std::chrono::milliseconds(1));
    sleep(1);
  }

  return 0;
}
