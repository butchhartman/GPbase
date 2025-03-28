#ifndef GP_INSTANCE_INCLUDED
#define GP_INSTANCE_INCLUDED
#include "vulkan/vulkan.h"
#include "SDL.h"
#include "Globals.h"
#include "ValidationLayers.h"
#include "LoadShaders.h"
#include "Swapchain.h"
/******************************************************************************//** 
 * March 28, 2025
 * A Vulkan instance is the connection between the application and the Vulkan library
 * 
 * (https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Instance)
 * 
 * TODO: These functions were some of the first I wrote and it shows; code cleanup needed
 *
***********************************************************************************/

extern void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

extern void instance_createSyncObjects(VkSemaphore **imageAvailableSemaphore, VkSemaphore **renderFinishedSemaphores,
					   VkFence **inFlightFences, VkDevice logicalDevice);

extern void instance_createCommandPool(VkCommandPool *commandPool, VkSurfaceKHR surface,
					   VkPhysicalDevice physicalDevice, VkDevice logicalDevice);

extern void instance_updateUniformBuffer(uint32_t currentImage, VkExtent2D swapChainExtent,
								  void **uniformBuffersMapped);

extern void instance_createRenderPass(VkRenderPass *renderPass, VkFormat swapChainImageFormat,
							   		  VkPhysicalDevice physicalDevice, VkDevice logicalDevice);

extern VkVertexInputAttributeDescription *instance_getAttributeDescriptions();

extern VkVertexInputBindingDescription instance_getVertexBindingDescription();

extern void instance_createGraphicsPipeline(VkPipelineLayout *pipelineLayout, VkPipeline *graphicsPipeline, 
							                VkDescriptorSetLayout *descriptorSetLayout, VkRenderPass renderPass,
                							VkExtent2D swapChainExtent, VkDevice logicalDevice);


/**
 * \brief Creates the rendering surface for the Vulkan instance
 * 
 * \param window The SDL window which will contain the surface
 * \param instance The Vulkan instance that will own the surface
 * \param surface Pointer to the surface handle variable that will be assigned the created surface
 */
extern void instance_createSurface(SDL_Window *window, VkInstance instance,
				                   VkSurfaceKHR *surface);

/**
 *  \brief Gets the vulkan extensions required by the program
 * 
 * \param count Pointer to a uint32_t that is assigned number of required extensions
 * 
 * \return An array of the names of the required extensions
 */
extern const char **instance_getRequiredExtensions(uint32_t *count);

/**
 * \brief Creates the vulkan instance
 * 
 * \param instance A pointer to a VkInstance which will be assigned the reference to the created instance
 */
extern void instance_createInstance(VkInstance *instance);

#endif