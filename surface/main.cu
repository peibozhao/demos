
#include <iostream>
#include <cuda_runtime_api.h>
#include <surface_functions.h>

#define CHECK(cu_ret) \
  if (cu_ret != cudaSuccess) { \
    std::cout << "cuda error: " << cu_ret << std::endl; \
    exit(-1); \
  }

// Simple transformation kernel
__global__ void transformKernel(float* output,
                                cudaSurfaceObject_t surfObj,
                                int width, int height)
{
    unsigned int x = blockIdx.x * blockDim.x + threadIdx.x;
    unsigned int y = blockIdx.y * blockDim.y + threadIdx.y;

    surf2Dread(output + y * width + x, surfObj, x * 4 + 4, y, cudaBoundaryModeClamp);
}

// Host code
int main()
{
    const int height = 16;
    const int width = 16;

    float *h_data = (float *)std::malloc(sizeof(float) * width * height);
    for (int i = 0; i < height; ++i) {
      for (int j = 0; j < width; ++j) {
        h_data[i * width + j] = i + j;
        std::cout << h_data[i * width + j] << "\t";
      }
      std::cout << std::endl;
    }

    cudaChannelFormatDesc channelDesc =
        cudaCreateChannelDesc(32, 0, 0, 0, cudaChannelFormatKindFloat);
    cudaArray_t cuArray;
    CHECK(cudaMallocArray(&cuArray, &channelDesc, width, height, cudaArraySurfaceLoadStore));

    const size_t spitch = width * sizeof(float);
    CHECK(cudaMemcpy2DToArray(cuArray, 0, 0, h_data, spitch, width * sizeof(float),
                        height, cudaMemcpyHostToDevice));

    struct cudaResourceDesc resDesc;
    memset(&resDesc, 0, sizeof(resDesc));
    resDesc.resType = cudaResourceTypeArray;
    resDesc.res.array.array = cuArray;

    cudaSurfaceObject_t surfObj;
    CHECK(cudaCreateSurfaceObject(&surfObj, &resDesc));

    float *output;
    CHECK(cudaMalloc(&output, width * height * sizeof(float)));

    // Invoke kernel
    dim3 threadsperBlock(16, 16);
    dim3 numBlocks((width + threadsperBlock.x - 1) / threadsperBlock.x,
                    (height + threadsperBlock.y - 1) / threadsperBlock.y);
    transformKernel<<<numBlocks, threadsperBlock>>>(output, surfObj, width, height);

    float *h_ret = (float *)std::malloc(sizeof(float) * width * height);
    memset(h_ret, 0, sizeof(float) * width * height);
    // Copy data from device back to host
    CHECK(cudaMemcpy(h_ret, output, width * height * sizeof(float),
                cudaMemcpyDeviceToHost));

    CHECK(cudaDeviceSynchronize());
    std::cout << "----" << std::endl;
    for (int i = 0; i < height; ++i) {
      for (int j = 0; j < width; ++j) {
        std::cout << h_ret[i * width + j] << "\t";
      }
      std::cout << std::endl;
    }


    CHECK(cudaDestroySurfaceObject(surfObj));

    // Free device memory
    CHECK(cudaFreeArray(cuArray));
    CHECK(cudaFree(output));

    // Free host memory
    free(h_data);

    return 0;
}
