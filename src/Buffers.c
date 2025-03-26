#include "Buffers.h"


void buffers_copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkCommandPool commandPool, VkDevice logicalDevice, VkQueue graphicsQueue) {
	VkCommandBufferAllocateInfo allocInfo = {0};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(logicalDevice, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {0};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion = {0};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {0};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue); // If fences are used, one can schedule multiple transfers simultaneously

	vkFreeCommandBuffers(logicalDevice, commandPool, 1, &commandBuffer);
}

void buffers_createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer *buffer, VkDeviceMemory *buffermemory, VkDevice logicalDevice){
	VkBufferCreateInfo bufferInfo = {0};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	// NOTE : THE SIZE VALUE SHOULD *NOT* BE THE LENGTH OF THE ARRAY, IT IS THE SIZE OF THE ARRAY'S MEMORY BLOCK (the size you would put into malloc params)
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(logicalDevice, &bufferInfo, NULL, buffer) != VK_SUCCESS){
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create vertex buffer");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(logicalDevice, *buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {0};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties); 

	/*
	* In real applications, one should not call vkallocatememory for each individual buffer. This is due to
	* the number of simultaneous memory allocations being limited by the maxMemoryAllocationCount physical device limit.
	*  "The right way to allocate memory for a large number of objects at the same time is to create a custom allocator that 
	*   splits up a single allocation among many different objects by using the offset parameters that we've seen in many functions." (vulkan-tutorial.com)
	*
	*	One can write this themselves or use a library.
	*/

	if (vkAllocateMemory(logicalDevice, &allocInfo, NULL, buffermemory) != VK_SUCCESS) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to allocate vertex buffer memory");
	}
	vkBindBufferMemory(logicalDevice, *buffer, *buffermemory, 0);
}

void buffers_createFramebuffers(VkFramebuffer **swapChainFrameBuffers, uint32_t *swapChainFrameBuffersLength, 
                        VkImageView *swapChainImageViews, uint32_t swapChainImageViewsLength, 
                        uint32_t swapChainImagesLength, VkRenderPass renderPass,
                        VkExtent2D swapChainExtent, VkDevice logicalDevice,
                        VkImageView depthImageView) {
	*swapChainFrameBuffers = malloc(sizeof(VkFramebuffer) * swapChainImageViewsLength);

	*swapChainFrameBuffersLength = swapChainImageViewsLength;

	if (*swapChainFrameBuffers == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to allocate memory for swap chain frame buffers");
		return;
	}

	for (uint32_t i = 0; i < swapChainImagesLength; i++) {

		VkImageView attachments[] = {swapChainImageViews[i], depthImageView};

		VkFramebufferCreateInfo framebufferInfo = { 0 };
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 2; // This should be the length of attachments
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;


		if (vkCreateFramebuffer(logicalDevice, &framebufferInfo, NULL, &((*swapChainFrameBuffers)[i])) != VK_SUCCESS) {
			SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create framebuffer");
			exit(VK_ERROR_INITIALIZATION_FAILED);
		}
	}
}

void buffers_createCommandBuffers(VkCommandBuffer **commandBuffers, VkCommandPool commandPool,
                          VkDevice logicalDevice) {
	*commandBuffers = malloc(sizeof(VkCommandBuffer) * MAX_FRAMES_IN_FLIGHT);// TODO : FREE

	VkCommandBufferAllocateInfo allocInfo = { 0 };
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)MAX_FRAMES_IN_FLIGHT;

	if (vkAllocateCommandBuffers(logicalDevice, &allocInfo, *commandBuffers) != VK_SUCCESS) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create command buffer");
		exit(VK_ERROR_INITIALIZATION_FAILED);
	}
}

void buffers_createVertexBuffer(Vertex *inputVertices, VkDevice logicalDevice,
                        VkBuffer *vertexBuffer, VkDeviceMemory *vertexBufferMemory,
                        VkCommandPool commandPool, VkQueue graphicsQueue) {
	// This is just size of the source in bytes, so a simple sizeof is permittable
	VkDeviceSize  bufferSize = sizeof(Vertex) * 8;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	buffers_createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                 &stagingBuffer, &stagingBufferMemory, logicalDevice);


	void *data;
	vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, inputVertices, (size_t)bufferSize);
	vkUnmapMemory(logicalDevice, stagingBufferMemory);

	buffers_createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory, logicalDevice);

	buffers_copyBuffer(stagingBuffer, *vertexBuffer, bufferSize, commandPool, logicalDevice, graphicsQueue);

	vkDestroyBuffer(logicalDevice, stagingBuffer, NULL);
	vkFreeMemory(logicalDevice, stagingBufferMemory, NULL);
}

void buffers_createIndexBuffer(uint16_t *inputIndices, VkDevice logicalDevice,
                        VkBuffer *indexBuffer, VkDeviceMemory *indexBufferMemory,
                        VkCommandPool commandPool, VkQueue graphicsQueue) {
	// This is just size of the source in bytes, so a simple sizeof is permittable
	VkDeviceSize bufferSize = 24; //temp

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	buffers_createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory, logicalDevice);

	void *data;
	vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, inputIndices, (size_t)bufferSize);
	vkUnmapMemory(logicalDevice, stagingBufferMemory);

	buffers_createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory, logicalDevice);

	buffers_copyBuffer(stagingBuffer, *indexBuffer, bufferSize, commandPool, logicalDevice, graphicsQueue);

	vkDestroyBuffer(logicalDevice, stagingBuffer, NULL);
	vkFreeMemory(logicalDevice, stagingBufferMemory, NULL);
}

void buffers_createUniformBuffers(VkBuffer **uniformBuffers, VkDeviceMemory **uniformBuffersMemory,
                          void ***uniformBuffersMapped, VkDevice logicalDevice) {

	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	*uniformBuffers = malloc(sizeof(VkBuffer) * MAX_FRAMES_IN_FLIGHT);
	*uniformBuffersMemory = malloc(sizeof(VkDeviceMemory) * MAX_FRAMES_IN_FLIGHT);

	*uniformBuffersMapped = malloc(sizeof(void*) * MAX_FRAMES_IN_FLIGHT); // Must allocate for the # of void * this array will contain
	(*uniformBuffersMapped)[0] = malloc(sizeof(UniformBufferObject)); // Must allocate the size the void* in index 0 will store
	(*uniformBuffersMapped)[1] = malloc(sizeof(UniformBufferObject));// Must allocate the size the void* in index 1 will store

	// TODO : Automate this via a loop

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {                                                                                 // actual black magic addressing below
																																				// the tutorial did not pass these as pointers, but that might be because vector does that automatically?
		buffers_createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &((*uniformBuffers)[i]), &((*uniformBuffersMemory)[i]), logicalDevice);
		vkMapMemory(logicalDevice, (*uniformBuffersMemory)[i], 0, bufferSize, 0, &((*uniformBuffersMapped)[i])); // Initially, accidently wrote uniformBuffersMemore[i] twice instead of uniformBuffersMapped[i]...

	}
}