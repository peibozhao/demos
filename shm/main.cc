#include <iostream>
#include <memory>
#include <sys/statfs.h>
#include <thread>
#include <sys/shm.h>

int main() {
 int maxkey, id, shmid;
 struct shm_info shm_info;
 struct shmid_ds shmds;
 size_t memCnt = 0;
 maxkey = shmctl(0, SHM_INFO, (shmid_ds *)&shm_info);
 for (id = 0; id <= maxkey; id++) {
   shmid = shmctl(id, SHM_STAT, &shmds);
   if (shmid < 0) {
     continue;
   } else {
     memCnt += shmds.shm_segsz;
     std::cout << id << ": " << shmds.shm_segsz / 1024.0 / 1024.0 << " " << memCnt / 1024.0 / 1024.0 << std::endl;
   }
 }
}
