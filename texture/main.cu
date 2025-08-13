

#include <cuda_runtime_api.h>
#include <iostream>
#include <surface_functions.h>

#define CHECK(cu_ret) \
  if (cu_ret != cudaSuccess) { \
    std::cout << "line: " << __LINE__ << ". cuda error: " << cu_ret << std::endl; \
    exit(-1); \
  }

// Simple transformation kernel
__global__ void transformKernel(float* output,
                                cudaTextureObject_t texObj,
                                int width, int height,
                                float delta)
{
    // Calculate normalized texture coordinates
    unsigned int x = blockIdx.x * blockDim.x + threadIdx.x;
    unsigned int y = blockIdx.y * blockDim.y + threadIdx.y;

    float u = (x + delta) / (float)width;
    float v = (y + delta) / (float)height;

    // printf("%f %f\n", u, v);

    // Read from texture and write to global memory
    output[y * width + x] = tex2D<float>(texObj, u, v);
}

// Simple transformation kernel
__global__ void transformKernel2(float* output,
                                cudaSurfaceObject_t surObj,
                                int width, int height,
                                float delta)
{
    // Calculate normalized texture coordinates
    unsigned int x = blockIdx.x * blockDim.x + threadIdx.x;
    unsigned int y = blockIdx.y * blockDim.y + threadIdx.y;

    // float u = (x + delta) / (float)width;
    // float v = (y + delta) / (float)height;

    // printf("%f %f\n", u, v);

    // Read from texture and write to global memory
    // output[y * width + x] = tex2D<float>(texObj, u, v);
    // surf2Dread<float>(output + y * width + x, texObj, u, v);
    surf2Dread(&output[y * width * x], surObj, x * 4, y, cudaBoundaryModeTrap);
}

// Host code
int main()
{
    // const int height = 1024;
    // const int width = 1024;
    const int height = 16;
    const int width = 16;
    float delta = 1.0;

    // Allocate and set some host data
    float *h_data = (float *)std::malloc(sizeof(float) * width * height);
    for (int i = 0; i < height; ++i) {
      for (int j = 0; j < width; ++j) {
        h_data[i * width + j] = i + j;
        std::cout << h_data[i * width + j] << "\t";
      }
      std::cout << std::endl;
    }

    // Allocate CUDA array in device memory
    cudaChannelFormatDesc channelDesc =
        cudaCreateChannelDesc(32, 0, 0, 0, cudaChannelFormatKindFloat);
    cudaArray_t cuArray;
    CHECK(cudaMallocArray(&cuArray, &channelDesc, width, height, cudaArraySurfaceLoadStore));

    // Set pitch of the source (the width in memory in bytes of the 2D array pointed
    // to by src, including padding), we dont have any padding
    // pitch = stride = lda
    const size_t spitch = width * sizeof(float);
    // Copy data located at address h_data in host memory to device memory
    CHECK(cudaMemcpy2DToArray(cuArray, 0, 0, h_data, spitch, width * sizeof(float),
                        height, cudaMemcpyHostToDevice));

    // Specify texture
    struct cudaResourceDesc resDesc;
    memset(&resDesc, 0, sizeof(resDesc));
    resDesc.resType = cudaResourceTypeArray;
    resDesc.res.array.array = cuArray;

    // Specify texture object parameters
    struct cudaTextureDesc texDesc;
    memset(&texDesc, 0, sizeof(texDesc));
    texDesc.addressMode[0] = cudaAddressModeClamp;
    texDesc.addressMode[1] = cudaAddressModeClamp;
    texDesc.filterMode = cudaFilterModeLinear;
    // texDesc.filterMode = cudaFilterModePoint;
    texDesc.readMode = cudaReadModeElementType;
    texDesc.normalizedCoords = 1;

    // Create texture object
    // cudaTextureObject_t texObj = 0;
    // CHECK(cudaCreateTextureObject(&texObj, &resDesc, &texDesc, NULL));
    cudaSurfaceObject_t surObj = 0;
    CHECK(cudaCreateSurfaceObject(&surObj, &resDesc));

    // Allocate result of transformation in device memory
    float *output;
    CHECK(cudaMalloc(&output, width * height * sizeof(float)));

    // Invoke kernel
    dim3 threadsperBlock(16, 16);
    dim3 numBlocks((width + threadsperBlock.x - 1) / threadsperBlock.x,
                    (height + threadsperBlock.y - 1) / threadsperBlock.y);
    transformKernel2<<<numBlocks, threadsperBlock>>>(output, surObj, width, height, delta);

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


    // Destroy texture object
    // CHECK(cudaDestroyTextureObject(texObj));

    // Free device memory
    CHECK(cudaFreeArray(cuArray));
    CHECK(cudaFree(output));

    // Free host memory
    free(h_data);

    return 0;
}
