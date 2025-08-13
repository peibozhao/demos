
#include <thrust/device_vector.h>
#include <thrust/host_vector.h>
#include <thrust/sequence.h>
#include <vector>
#include <algorithm>
#include <numeric>
#include <fstream>

__global__ void kern(float *arr, float *arr2, int size, float *ret) {
  int x = blockIdx.x * gridDim.x + threadIdx.x;
  if (x < size) {
    ret[x] = arr[x] * arr2[x];
  }
}

int main(int argc, char *argv[]) {

  std::vector<float> vv;
  std::ifstream ifs("test.txt");
  while (ifs) {
    float f;
    ifs >> f;
    if (!ifs) break;
    vv.emplace_back(f);
    std::cout << vv.back() << std::endl;
  }
  return 0;


  std::vector<float> v(100, 0.1);
  std::vector<float> v2(100, 9371);

  std::iota(v.begin(), v.end(), 10);
  std::iota(v2.begin(), v2.end(), 7);

  thrust::device_vector<float> dv(v);
  thrust::device_vector<float> dv2(v2);
  thrust::device_vector<float> dr(100);

  cudaDeviceSynchronize();
  kern<<<512, 512>>>(dv.data().get(), dv2.data().get(), 100, dr.data().get());
  cudaDeviceSynchronize();

  for (int i = 0; i < 100; ++i) {
    std::cout << dr[i] << std::endl;
  }

  return 0;
}
