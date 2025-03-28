#ifndef GP_BUFFERS_INCLUDED_
#define GP_BUFFERS_INCLUDED_

#include "vulkan/vulkan.h"
#include "SDL3/SDL.h"
#include "Globals.h"
#include "Device.h"


/******************************************************************************//**
 *
 * March 28, 2025
 * 
 **********************************************************************************/

/**
 * \brief Helper function used to copy data from srcBuffer to dstBuffer
 * 
 * Creates a temporary command buffer and submits the VkCmdCopyBuffer command into the graphics queue
 * 
 * \param srcBuffer The buffer containing the data to be copied
 * \param dstBuffer The buffer to copy the srcBuffer's data into
 * \param size The size of the data being copied
 * \param commandPool The command pool from which to create a temporary command buffer
 * \param logicalDevice The logical device from which to create a temporary command buffer
 * \param graphicsQueue The graphics queue the VkCmdCopyBuffer command will be submitted to
 */
extern void buffers_copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, 
                        VkDeviceSize size, VkCommandPool commandPool, 
                        VkDevice logicalDevice, VkQueue graphicsQueue);

/**
 * \brief Helper function that creates a generic VkBuffer
 * 
 * \param size Size of the buffer to be created
 * \param usage Flags specifying what the buffer is going to be used for
 * \param properties Flags specifying the required properties of the buffer's memory
 * \param buffer Pointer to the VkBuffer variable which the created buffer will be stored in
 * \param buffermemory Pointer to the VkDeviceMemory variable which the information about the allocated memory will be stored
 * \param logicalDevice The logical device the buffer memory will be allocated from
 */
extern void buffers_createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, 
                        VkMemoryPropertyFlags properties, VkBuffer *buffer, 
                        VkDeviceMemory *buffermemory, VkDevice logicalDevice,
                        VkPhysicalDevice physicalDevice);

/**
 * \brief Creates an array of framebuffers
 * 
 * \param swapChainFrameBuffers Address of the pointer which will store the allocated framebuffers
 * \param swapChainFrameBuffersLength Address of an integer which will store the amount of framebuffers created
 * \param swapChainImageViews Array of VkImageView which will each be attached to a created framebuffer
 * \param renderPass VkRenderPass object which will be attached to each created framebuffer
 * \param swapChainExtent VkExtent2D describing the width and height of the swapchain
 * \param logicalDevice The logical device the framebuffers will be allocated from
 * \param depthImageView Depth buffer image view which will be attached to each created framebuffer (revise this definition)
 */
extern void buffers_createFramebuffers(VkFramebuffer **swapChainFrameBuffers, uint32_t *swapChainFrameBuffersLength, 
                        VkImageView *swapChainImageViews, uint32_t swapChainImageViewsLength, 
                        uint32_t swapChainImagesLength, VkRenderPass renderPass,
                        VkExtent2D swapChainExtent, VkDevice logicalDevice,
                        VkImageView depthImageView);

/**
 * \brief Creates an array of command buffers
 * 
 * \param commandBuffers Address of the pointer which will store the allocated command buffers
 * \param commandPool The command pool from which the command buffers' memory will be managed by
 * \param logicalDevice The logical device from which the command buffers will be allocated
 */
extern void buffers_createCommandBuffers(VkCommandBuffer **commandBuffers, VkCommandPool commandPool, 
                        VkDevice logicalDevice);

/**
 * \brief Creates a vertex buffer
 * 
 * \param inputVertices An array containing the vertex data to store in the vertex buffer
 * \param logicalDevice The logical device from which the vertex buffer will be allocated
 * \param vertexBuffer Address of the VkBuffer variable which will store the created vertex buffer
 * \param vertexBufferMemory Address of the VkDeviceMemory variable which will store information about the allocated vertex buffer's memory
 * \param commandPool Command pool which a temporary command buffer will be allocated from
 * \param graphicsQueue Graphics queue to which the command to copy buffer data from the staging buffer to the vertex buffer will be submitted
 */
extern void buffers_createVertexBuffer(Vertex *inputVertices, VkDevice logicalDevice,
                        VkBuffer *vertexBuffer, VkDeviceMemory *vertexBufferMemory,
                        VkCommandPool commandPool, VkQueue graphicsQueue,
                        VkPhysicalDevice physicalDevice);
/**
 * \brief Creates an index buffer
 * 
 * \param inputIndices An array of 16 bit integers to be stored in the index buffer
 * \param logicalDevice The logical device from which the index buffer will be allocated
 * \param indexBuffer Address of the VkBuffer variable which will store the created index buffer
 * \param indexBufferMemory Address of the VkDeviceMemory variable which will store information about the allocated index buffer's memory
 * \param commandPool Command pool which a temporary command buffer will be allocated from
 * \param graphicsQueue Graphics queue to which the command to copy buffer data from the staging buffer to the index buffer will be submitted
 */
extern void buffers_createIndexBuffer(uint16_t *inputIndices, VkDevice logicalDevice,
                        VkBuffer *indexBuffer, VkDeviceMemory *indexBufferMemory,
                        VkCommandPool commandPool, VkQueue graphicsQueue,
                        VkPhysicalDevice physicalDevice);

/**
 * \brief Creates an array of uniform buffers
 * 
 * \param uniformBuffers Address of the pointer which will store the uniform buffers
 * \param uniformBuffersMemory Address of the pointer which will store information about the allocated uniform buffers' memory
 * \param uniformBuffersMapped Address of pointer to void* variables which "a host-accessible pointer to the beginning of the mapped range is returned"
 * \param logicalDevice Logical device from which the uniform buffers will be allocated
 */
extern void buffers_createUniformBuffers(VkBuffer **uniformBuffers, VkDeviceMemory **uniformBuffersMemory,
                          void ***uniformBuffersMapped, VkDevice logicalDevice,
                          VkPhysicalDevice physicalDevice);

#endif // GP_BUFFERS_INCLUDED