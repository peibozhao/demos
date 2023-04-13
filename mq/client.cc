
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
#include <mqueue.h>

int main(int argc, char *argv[]) {

  mqd_t mq = mq_open("/testmq", O_CREAT | O_RDWR, 0777, nullptr);
  std::cout << "mq_open: " << mq << std::endl;

  struct mq_attr attr;
  int ret = mq_getattr(mq, &attr);
  std::cout << "mq_getattr: " << ret << std::endl;
  int msg_len = attr.mq_msgsize;
  char *buf = new char[msg_len];
  while (1) {
    ret = mq_receive(mq, buf, msg_len, 0);
    std::cout << "mq_receive: " << ret << std::endl;
    perror("mq_receive");
    std::cout << *(int *)buf << std::endl;
  }

  return 0;
}
