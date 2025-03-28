#include <stdio.h>
#include <malloc.h>
#include "vulkan/vulkan.h"
#include "SDL.h"
// March 28 2025
const unsigned char* readFileBin(const char* filename, unsigned int *size);

VkShaderModule createShaderModule(const unsigned char* code, uint32_t size,
                                  VkDevice logicalDevice);