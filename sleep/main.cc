
#include <chrono>
#include <stdio.h>
#include <sys/time.h>
#include <thread>
#include <time.h>
#include <unistd.h>

int main() {
  const int sleep_ms = 7;

  while (true) {
    // usleep
    struct timeval tv;
    gettimeofday(&tv, NULL);
    usleep(sleep_ms * 1000);
    struct timeval tv2;
    gettimeofday(&tv2, NULL);
    // printf("%ld %ld\n", tv.tv_sec, tv.tv_usec / 1000);
    // printf("%ld %ld\n", tv2.tv_sec, tv2.tv_usec / 1000);
    printf("usleep %ld\n", tv2.tv_sec * 1000 + tv2.tv_usec / 1000 -
                               tv.tv_sec * 1000 - tv.tv_usec / 1000);

    // chrono
    gettimeofday(&tv, NULL);
    std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
    gettimeofday(&tv2, NULL);
    // printf("%ld %ld\n", tv.tv_sec, tv.tv_usec / 1000);
    // printf("%ld %ld\n", tv2.tv_sec, tv2.tv_usec / 1000);
    printf("chrono sleep %ld\n", tv2.tv_sec * 1000 + tv2.tv_usec / 1000 -
                                     tv.tv_sec * 1000 - tv.tv_usec / 1000);

    // nanosleep
    struct timespec req;
    req.tv_sec = 0;
    req.tv_nsec = sleep_ms * 1000 * 1000;
    gettimeofday(&tv, NULL);
    nanosleep(&req, NULL);
    gettimeofday(&tv2, NULL);
    // printf("%ld %ld\n", tv.tv_sec, tv.tv_usec / 1000);
    // printf("%ld %ld\n", tv2.tv_sec, tv2.tv_usec / 1000);
    printf("nanosleep %ld\n", tv2.tv_sec * 1000 + tv2.tv_usec / 1000 -
                                  tv.tv_sec * 1000 - tv.tv_usec / 1000);

    // select
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = sleep_ms * 1000;
    gettimeofday(&tv, NULL);
    select(0, NULL, NULL, NULL, &timeout);
    gettimeofday(&tv2, NULL);
    printf("select %ld\n", tv2.tv_sec * 1000 + tv2.tv_usec / 1000 -
                               tv.tv_sec * 1000 - tv.tv_usec / 1000);
  }
}
