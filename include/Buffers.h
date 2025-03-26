#ifndef GP_BUFFERS_INCLUDED_
#define GP_BUFFERS_INCLUDED_

#include "vulkan/vulkan.h"
#include "SDL3/SDL.h"
#include "Globals.h"

extern void createFramebuffers(VkFramebuffer **swapChainFrameBuffers, uint32_t *swapChainFrameBuffersLength, 
                        VkImageView *swapChainImageViews, uint32_t swapChainImageViewsLength, 
                        uint32_t swapChainImagesLength, VkRenderPass renderPass,
                        VkExtent2D swapChainExtent, VkDevice logicalDevice,
                        VkImageView depthImageView);

extern void createCommandBuffers(VkCommandBuffer **commandBuffers, VkCommandPool commandPool,
                          VkDevice logicalDevice);

extern void createVertexBuffer(Vertex *inputVertices, VkDevice logicalDevice,
                        VkBuffer *vertexBuffer, VkDeviceMemory *vertexBufferMemory,
                        VkCommandPool commandPool, VkQueue graphicsQueue);

extern void createIndexBuffer(uint16_t *inputIndices, VkDevice logicalDevice,
                        VkBuffer *indexBuffer, VkDeviceMemory *indexBufferMemory,
                        VkCommandPool commandPool, VkQueue graphicsQueue);

extern void createUniformBuffers(VkBuffer **uniformBuffers, VkDeviceMemory **uniformBuffersMemory,
                          void ***uniformBuffersMapped, VkDevice logicalDevice);

#endif