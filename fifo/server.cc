
#include <chrono>
#include <errno.h>
#include <fcntl.h>
#include <iomanip>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <thread>

const std::string fname = "/tmp/fifo";

int main(int argc, char *argv[]) {
  int ret = 0;
  int fd = open(fname.c_str(), O_RDWR);
  std::cout << "open: " << fd << std::endl;

  uint64_t timestamp = 0;
  while (1) {
    timestamp = std::chrono::system_clock::now().time_since_epoch().count();
    ret = write(fd, &timestamp, sizeof(timestamp));
    std::cout << "write: " << ret << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
  }
}
