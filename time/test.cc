
#include <chrono>
#include <fstream>
#include <thread>

int main(int argc, char *argv[]) {
  std::ofstream ofs("/home/caros/xlog/log/time_monitor.log", std::ios::app);
  ofs << "begin" << std::endl;
  int64_t last = 0;
  while (true) {
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::system_clock::now().time_since_epoch())
                   .count();
    ofs << (std::to_string(now)) << std::endl;
    if (now <= last) {
      ofs << "error" << std::endl;
    }
    last = now;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  return 0;
}
