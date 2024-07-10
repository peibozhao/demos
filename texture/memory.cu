
#include <chrono>
#include <cuda_runtime_api.h>
#include <fstream>
#include <iostream>
#include <cuda_runtime.h>

constexpr int height = 1536;
constexpr int width = 1920;

__global__ void undistort(const float *dev_src, const float *dev_map1, const float *dev_map2, uint8_t *dev_dst) {
  int idx = blockIdx.x * blockDim.x + threadIdx.x;

  float map_x = dev_map1[idx];
  float map_y = dev_map2[idx];

  int x1 = int(map_x);
  int x2 = x1 + 1;
  int y1 = int(map_y);
  int y2 = y1 + 1;

  float scale[4] = {(x2 - map_x) * (y2 - map_y), (map_x - x1) * (y2 - map_y),
    (x2 - map_x) * (map_y - y1), (map_x - x1) * (map_y - y1)};

  float p[4] = {dev_src[idx], dev_src[idx + 1], dev_src[idx + width], dev_src[idx + width + 1]};

  dev_dst[idx] = scale[0] * p[0] + scale[1] * p[1] + scale[2] * p[2] + scale[3] * p[3];

  // int map_x = dev_map1[idx];
  // int map_y = dev_map2[idx];

  // dev_dst[idx] = dev_src[map_y * width + map_x];
}

int main(int argc, char *argv[])
{
  std::ifstream map1_fstream("map1.txt");
  std::ifstream map2_fstream("map2.txt");
  float *map1 = new float[height * width];
  float *map2 = new float[height * width];
  for (int i = 0; i < height * width; ++i) {
    // map1_fstream >> map1[i];
    // map2_fstream >> map2[i];

    float f;
    map1_fstream >> f;
    map1[i] = f;
    map2_fstream >> f;
    map2[i] = f;

    // std::cout << map1[i] << std::endl;
  }

  uint8_t *src = new uint8_t[height * width];
  std::ifstream src_fstream("test.gray", std::ios::binary);
  if (!src_fstream.is_open()) {
    std::cout << "file is not open" << std::endl;
    return -1;
  }
  src_fstream.read((char *)src, height * width);
  src_fstream.close();

  float *src_f = new float[height * width];
  for (int i = 0; i < height * width; ++i) {
    src_f[i] = src[i];
  }

  // uint8_t *dev_src;
  // cudaMalloc(&dev_src, height * width);
  // cudaMemcpy(dev_src, src, height * width, cudaMemcpyHostToDevice);
  float *dev_src;
  cudaMalloc(&dev_src, height * width * sizeof(float));
  cudaMemcpy(dev_src, src_f, height * width * sizeof(float), cudaMemcpyHostToDevice);

  float *dev_map1, *dev_map2;
  cudaMalloc(&dev_map1, height * width * sizeof(float));
  cudaMalloc(&dev_map2, height * width * sizeof(float));

  cudaMemcpy(dev_map1, map1, height * width * sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(dev_map2, map2, height * width * sizeof(float), cudaMemcpyHostToDevice);


  uint8_t *dst = new uint8_t[height * width];
  uint8_t *dev_dst;
  cudaMalloc(&dev_dst, height * width * sizeof(uint8_t));

  cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);

  for (int i = 0; i < 10; ++i) {
    cudaDeviceSynchronize();
    auto s = std::chrono::system_clock::now();
    cudaEventRecord(start);
    undistort<<<1536 * 1920 / 512, 512>>>(dev_src, dev_map1, dev_map2, dev_dst);
    cudaEventRecord(stop);
    cudaDeviceSynchronize();
    auto e = std::chrono::system_clock::now();
    cudaError_t err = cudaGetLastError();
    std::cout << "Cost: " << std::chrono::duration_cast<std::chrono::microseconds>(e - s).count() << "us " << err << std::endl;

    float ms;
    cudaEventElapsedTime(&ms, start, stop);
    std::cout << ms << std::endl;
  }

  cudaMemcpy(dst, dev_dst, height * width, cudaMemcpyDeviceToHost);

  std::ofstream ofs("output.gray", std::ios::binary);
  ofs.write((char *)dst, width * height);
  ofs.close();

  return 0;
}
