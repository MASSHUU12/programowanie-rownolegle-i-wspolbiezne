#define CL_TARGET_OPENCL_VERSION 300

#include <CL/cl.h>
#include <iostream>
#include <vector>

int main() {
  cl_uint platformCount = 0;
  std::vector<cl_platform_id> platforms;

  // Get the number of OpenCL platforms available
  cl_int status = clGetPlatformIDs(0, nullptr, &platformCount);
  if (status != CL_SUCCESS) {
    std::cerr << "Error getting platform IDs\n";
    return 1;
  }

  platforms.resize(platformCount);

  // Get the list of platforms
  status = clGetPlatformIDs(platformCount, platforms.data(), nullptr);
  if (status != CL_SUCCESS) {
    std::cerr << "Error getting platform list\n";
    return 1;
  }

  std::cout << "Number of OpenCL platforms found: " << platformCount << "\n";

  // Iterate over each platform
  for (cl_uint i = 0; i < platformCount; ++i) {
    char platformName[128];
    status = clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME,
                               sizeof(platformName), platformName, nullptr);
    if (status != CL_SUCCESS) {
      std::cerr << "Error getting platform info\n";
      continue;
    }
    std::cout << "\nPlatform " << i << ": " << platformName << "\n";

    cl_uint deviceCount = 0;
    std::vector<cl_device_id> devices;

    // Get the number of devices available on this platform
    status = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, nullptr,
                            &deviceCount);
    if (status != CL_SUCCESS) {
      std::cerr << "Error getting device count\n";
      continue;
    }

    devices.resize(deviceCount);

    // Get the list of devices
    status = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, deviceCount,
                            devices.data(), nullptr);
    if (status != CL_SUCCESS) {
      std::cerr << "Error getting device list\n";
      continue;
    }

    std::cout << "Number of devices: " << deviceCount << "\n";

    // Iterate over each device
    for (cl_uint j = 0; j < deviceCount; ++j) {
      char deviceName[128];
      status = clGetDeviceInfo(devices[j], CL_DEVICE_NAME, sizeof(deviceName),
                               deviceName, nullptr);
      if (status != CL_SUCCESS) {
        std::cerr << "Error getting device info\n";
        continue;
      }
      std::cout << "  Device " << j << ": " << deviceName << "\n";
    }
  }

  return 0;
}
