
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <iostream>

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

  while (1) {
    ret = write(sockfd, buffer, sizeof(buffer));
    std::cout << "write: " << ret << std::endl;
    sleep(1);
  }

  return 0;
}
