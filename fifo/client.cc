
#include <chrono>
#include <errno.h>
#include <fcntl.h>
#include <iomanip>
#include <iostream>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <thread>

const std::string fname = "/tmp/fifo";

int main(int argc, char *argv[]) {
  int fd = open(fname.c_str(), O_RDWR);
  std::cout << "open: " << fd << std::endl;

  uint64_t timestamp = 0;
  int ret = 0;
  while (1) {
    uint64_t now = std::chrono::system_clock::now().time_since_epoch().count();
    ret = read(fd, &timestamp, sizeof(timestamp));
    std::cout << std::fixed << std::setprecision(3) << "recv: " << ret
              << " time " << timestamp << " now " << (double)now / 1e9
              << std::endl;
    if (ret <= 0) {
      std::cout << "errono: " << errno << std::endl;
      perror("errno: ");
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }

  return 0;
}
