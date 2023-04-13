
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
#include <mqueue.h>

int main(int argc, char *argv[]) {
  mqd_t mq = mq_open("/testmq", O_CREAT | O_RDWR, 0777, nullptr);
  std::cout << "mq_open: " << mq << std::endl;
  perror("mq_open");

  int i = 0;
  unsigned prio = 0;
  while (1) {
    int ret = mq_send(mq, (char *)&i, sizeof(i), 0);
    std::cout << "mq_send: " << ret << std::endl;
    i += 1;
    sleep(1);
  }
}
