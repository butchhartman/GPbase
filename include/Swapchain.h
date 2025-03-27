#ifndef GP_SWAPCHAIN_INCLUDED_
#define GP_SWAPCHAIN_INCLUDED_
#include "vulkan/vulkan.h"
#include "SDL.h"
#include "Globals.h"
#include "Device.h"
#include "Buffers.h"
#include "SingleTimeCommands.h"

extern SwapChainSupportDetails swapchain_querySwapChainSupport(VkPhysicalDevice device, uint32_t *count, VkSurfaceKHR surface);

extern VkPresentModeKHR swapchain_chooseSwapPresentMode(const VkPresentModeKHR **availablePresentModes);

extern VkSurfaceFormatKHR swapchain_chooseSwapSurfaceFormat(const VkSurfaceFormatKHR *availableFormats, uint32_t length);

extern VkExtent2D swapchain_chooseSwapExtent(const VkSurfaceCapabilitiesKHR* capabilities, SDL_Window *window);

extern VkFormat swapchain_findSupportedFormat(const VkFormat *candidates, uint32_t candidatesLength, VkImageTiling tiling, VkFormatFeatureFlags features, VkPhysicalDevice physicalDevice);

extern void swapchain_transitionImageLayout(VkImage image, VkFormat format, 
									 VkImageLayout oldLayout, VkImageLayout newLayout, 
									 VkPipelineStageFlags *sourceStage, VkPipelineStageFlags *destinationStage,
									 VkCommandPool commandPool, VkDevice logicalDevice,
									 VkQueue graphicsQueue);

extern VkImageView swapchain_createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkDevice logicalDevice);

extern void swapchain_createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage *image, VkDeviceMemory *imageMemory, VkDevice logicalDevice);

extern VkFormat swapchain_findDepthFormat(VkPhysicalDevice physicalDevice);

extern void swapchain_createDepthResources(VkExtent2D swapChainExtent, VkImage *depthImage,
      									   VkImageView *depthImageView, VkDeviceMemory *depthImageMemory,
                                           VkPhysicalDevice physicalDevice, VkDevice logicalDevice,
                                           VkPipelineStageFlags *sourceStage, VkPipelineStageFlags *destinationStage,
                                           VkCommandPool commandPool, VkQueue graphicsQueue);

extern void swapchain_cleanupSwapChain(Uint32 swapChainImageViewsLength, VkDevice logicalDevice,
                                VkImageView **swapChainImageViews, uint32_t swapChainFrameBuffersLength,
                                VkFramebuffer **swapChainFramebuffers, VkSwapchainKHR swapChain,
                                VkImageView depthImageView, VkImage depthImage,
                                VkDeviceMemory depthImageMemory);


extern void swapchain_createImageViews(VkImageView **swapChainImageViews, uint32_t swapChainImagesLength,
                                       uint32_t *swapChainImageViewsLength, VkImage *swapChainImages,
                                       VkFormat swapChainImageFormat, VkDevice logicalDevice);

extern void swapchain_recreateSwapChain(VkSwapchainKHR *swapChain, VkImage **swapChainImages,
                                      uint32_t *swapChainImagesLength, VkFormat *swapChainImageFormat,
                                      VkExtent2D *swapChainExtent, VkDevice logicalDevice,
                                      VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
                                      SDL_Window *window, VkFramebuffer **swapChainFrameBuffers,
                                      uint32_t *swapChainFrameBuffersLength, VkImageView **swapChainImageViews,
                                      uint32_t *swapChainImageViewsLength, VkRenderPass renderPass, 
                                      VkImageView *depthImageView, VkImage *depthImage,
                                      VkDeviceMemory *depthImageMemory, VkPipelineStageFlags *sourceStage,
                                      VkPipelineStageFlags *destinationStage, VkCommandPool commandPool,
                                      VkQueue graphicsQueue);

extern void swapchain_createSwapChain(VkSwapchainKHR *swapChain, VkImage **swapChainImages,
                                      uint32_t *swapChainImagesLength, VkFormat *swapChainImageFormat,
                                      VkExtent2D *swapChainExtent, VkDevice logicalDevice,
                                      VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
                                      SDL_Window *window);


#endif