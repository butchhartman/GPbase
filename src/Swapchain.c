#include "Swapchain.h"

SwapChainSupportDetails swapchain_querySwapChainSupport(VkPhysicalDevice device, uint32_t *count, VkSurfaceKHR surface) {

	SwapChainSupportDetails details = { 0 };

	// surface capabilities
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	// surface formates
	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, NULL);

	if (count != NULL) {
		*count = formatCount;
	}

	if (formatCount != 0) {
		details.formats = (VkSurfaceFormatKHR*)malloc(sizeof(VkSurfaceFormatKHR) * formatCount);
		//memset(details.formats, NULL, sizeof(VkSurfaceFormatKHR) * formatCount); // i feel like i needed these for some reason so I wont delete them yet
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats);
	}

	// presentation modes
	uint32_t presentCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentCount, NULL);

	if (presentCount != 0) {
		details.presentModes = (VkPresentModeKHR*)malloc(sizeof(VkPresentModeKHR) * presentCount);
		//memset(details.presentModes, NULL, sizeof(VkPresentModeKHR) * presentCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentCount, details.presentModes);
	}
	return details;
}

VkPresentModeKHR swapchain_chooseSwapPresentMode(const VkPresentModeKHR **availablePresentModes) {
	// TODO : ADD SELECTION LOGIC
	// Selection not needed because fifo is guaranteed.
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkSurfaceFormatKHR swapchain_chooseSwapSurfaceFormat(const VkSurfaceFormatKHR *availableFormats, uint32_t length) {
	// the children yearn for for loops...

	// return the one with our specs
	printf("%d", length);
	for (uint32_t i = 0; i < length; i++) {
		if (availableFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
			availableFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) { 
			return availableFormats[i];
		}
	}

	// Otherwise return the first one lul
	return availableFormats[0];

}

VkExtent2D swapchain_chooseSwapExtent(const VkSurfaceCapabilitiesKHR* capabilities, SDL_Window *window) {
	
	// TODO : revist this function to add more sophisticated procedures

	int width, height;

	SDL_GetWindowSizeInPixels(window, &width, &height);

	VkExtent2D actualExtent;
	actualExtent.width = (uint32_t)width;
	actualExtent.height = (uint32_t)height;

	return actualExtent;
}

VkFormat swapchain_findSupportedFormat(const VkFormat *candidates, uint32_t candidatesLength, VkImageTiling tiling, VkFormatFeatureFlags features, VkPhysicalDevice physicalDevice) {
	for (uint32_t i = 0; i < candidatesLength; i++) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, candidates[i], &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return candidates[i];
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return candidates[i];
		}

		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to find supported format");
	}
}

void swapchain_transitionImageLayout(VkImage image, VkFormat format, 
									 VkImageLayout oldLayout, VkImageLayout newLayout, 
									 VkPipelineStageFlags *sourceStage, VkPipelineStageFlags *destinationStage,
									 VkCommandPool commandPool, VkDevice logicalDevice,
									 VkQueue graphicsQueue) {
	VkCommandBuffer commandBuffer = singletimecommands_beginSingleTimeCommands(commandPool, logicalDevice);

	VkImageMemoryBarrier barrier = {0};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;

	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		if (hasStencilComponent(format)) {
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}	
	barrier.subresourceRange.baseMipLevel = 0; 
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1; 

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		*sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		*destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		*sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		*destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		*sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		*destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unsupported layout transition");
	}

	vkCmdPipelineBarrier(
		commandBuffer,
		*sourceStage, *destinationStage,
		0,
		0, NULL,
		0, NULL,
		1, &barrier
	);

	singletimecommands_endSingleTimeCommands(commandBuffer, commandPool, graphicsQueue, logicalDevice);
}

VkImageView swapchain_createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkDevice logicalDevice) {
	VkImageViewCreateInfo viewInfo = {0};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	if (vkCreateImageView(logicalDevice, &viewInfo, NULL, &imageView) != VK_SUCCESS) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create texture image view");
	}
	return imageView;
}

void swapchain_createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage *image, VkDeviceMemory *imageMemory, VkDevice logicalDevice) {
	VkImageCreateInfo imageInfo = {0};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(logicalDevice, &imageInfo, NULL, image)) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create image");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(logicalDevice, *image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {0};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(logicalDevice, &allocInfo, NULL, imageMemory) != VK_SUCCESS) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to allocate image memory");
	}

	vkBindImageMemory(logicalDevice, *image, *imageMemory, 0);
}

VkFormat swapchain_findDepthFormat(VkPhysicalDevice physicalDevice) {
	VkFormat candidates[] = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
	return swapchain_findSupportedFormat(candidates, 3, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, physicalDevice);
}

void swapchain_createDepthResources(VkExtent2D swapChainExtent, VkImage *depthImage,
									VkImageView *depthImageView, VkDeviceMemory *depthImageMemory,
									VkPhysicalDevice physicalDevice, VkDevice logicalDevice,
									VkPipelineStageFlags *sourceStage, VkPipelineStageFlags *destinationStage,
									VkCommandPool commandPool, VkQueue graphicsQueue) {
	VkFormat depthFormat = swapchain_findDepthFormat(physicalDevice);
	swapchain_createImage(swapChainExtent.width, swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory, logicalDevice);
	*depthImageView = swapchain_createImageView(*depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, logicalDevice);
	swapchain_transitionImageLayout(*depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, sourceStage, destinationStage, commandPool, logicalDevice, graphicsQueue);
}

void swapchain_cleanupSwapChain(Uint32 swapChainImageViewsLength, VkDevice logicalDevice,
                                VkImageView **swapChainImageViews, uint32_t swapChainFrameBuffersLength,
                                VkFramebuffer **swapChainFramebuffers, VkSwapchainKHR swapChain,
                                VkImageView depthImageView, VkImage depthImage,
                                VkDeviceMemory depthImageMemory) {

	for (uint32_t i = 0; i < swapChainImageViewsLength; i++) {
		vkDestroyImageView(logicalDevice, (*swapChainImageViews)[i], NULL);
	}
	free(*swapChainImageViews);

	for (uint32_t i = 0; i < swapChainFrameBuffersLength; i++) {
		vkDestroyFramebuffer(logicalDevice, (*swapChainFramebuffers)[i], NULL);
	}
	free(*swapChainFramebuffers);

	vkDestroySwapchainKHR(logicalDevice, swapChain, NULL);
	vkDestroyImageView(logicalDevice, depthImageView, NULL);
	vkDestroyImage(logicalDevice, depthImage, NULL);
	vkFreeMemory(logicalDevice, depthImageMemory, NULL);
}

void swapchain_createImageViews(VkImageView **swapChainImageViews, uint32_t swapChainImagesLength,
                      uint32_t *swapChainImageViewsLength, VkImage *swapChainImages,
                      VkFormat swapChainImageFormat, VkDevice logicalDevice) {
	*swapChainImageViews = (VkImageView*)malloc(sizeof(VkImage) * swapChainImagesLength);

	if (swapChainImageViews == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to allocate memory for image views");
		return;
	}

	*swapChainImageViewsLength = swapChainImagesLength;
	for (uint32_t i = 0; i < swapChainImagesLength; i++) {
		VkImageViewCreateInfo createInfo = { 0 };
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapChainImages[i];

		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapChainImageFormat;

		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(logicalDevice, &createInfo, NULL, &((*swapChainImageViews)[i])) != VK_SUCCESS) {
			SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create image views");
			exit(VK_ERROR_INITIALIZATION_FAILED);
		}
	}
}

void swapchain_recreateSwapChain(VkSwapchainKHR *swapChain, VkImage **swapChainImages,
                                      uint32_t *swapChainImagesLength, VkFormat *swapChainImageFormat,
                                      VkExtent2D *swapChainExtent, VkDevice logicalDevice,
                                      VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
                                      SDL_Window *window, VkFramebuffer **swapChainFrameBuffers,
                                      uint32_t *swapChainFrameBuffersLength, VkImageView **swapChainImageViews,
                                      uint32_t *swapChainImageViewsLength, VkRenderPass renderPass, 
                                      VkImageView *depthImageView, VkImage *depthImage,
                                      VkDeviceMemory *depthImageMemory, VkPipelineStageFlags *sourceStage,
									  VkPipelineStageFlags *destinationStage, VkCommandPool commandPool,
									  VkQueue graphicsQueue) {
	// Do not recreate swapchain until window not minimized.
	// The minimum size of the window is 1 so there should never be a time the window size is 0 besides minimization
	SDL_WindowFlags minimizedFlagMask = SDL_GetWindowFlags(window);
	if (SDL_WINDOW_MINIMIZED & minimizedFlagMask) {
		return;
	}
	// TODO :  Note that we don't recreate the renderpass here for simplicity. In theory it can be possible for the swap chain image format to change during an applications' lifetime, e.g. when moving a window from an standard range to an high dynamic range monitor. This may require the application to recreate the renderpass to make sure the change between dynamic ranges is properly reflected.

	vkDeviceWaitIdle(logicalDevice);

	swapchain_cleanupSwapChain(*swapChainImageViewsLength, logicalDevice,
                                swapChainImageViews, *swapChainFrameBuffersLength,
                                swapChainFrameBuffers, *swapChain,
                                *depthImageView, *depthImage,
                                *depthImageMemory);

	swapchain_createSwapChain(swapChain, swapChainImages,
							  swapChainImagesLength, swapChainImageFormat,
							  swapChainExtent, logicalDevice,
							  physicalDevice, surface,
							  window);

	swapchain_createImageViews(swapChainImageViews, *swapChainImagesLength,
                               swapChainImageViewsLength, *swapChainImages,
                               *swapChainImageFormat, logicalDevice);

	swapchain_createDepthResources(*swapChainExtent, depthImage, depthImageView, depthImageMemory, physicalDevice, logicalDevice, sourceStage, destinationStage, commandPool, graphicsQueue);

	buffers_createFramebuffers(swapChainFrameBuffers, swapChainFrameBuffersLength,
						*swapChainImageViews, *swapChainImageViewsLength,
						*swapChainImagesLength, renderPass,
						*swapChainExtent, logicalDevice,
						*depthImageView);
}

void swapchain_createSwapChain(VkSwapchainKHR *swapChain, VkImage **swapChainImages,
                                      uint32_t *swapChainImagesLength, VkFormat *swapChainImageFormat,
                                      VkExtent2D *swapChainExtent, VkDevice logicalDevice,
                                      VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
                                      SDL_Window *window) {
	uint32_t surfaceFormatsLength = NULL;

	SwapChainSupportDetails swapChainSupport = swapchain_querySwapChainSupport(physicalDevice, &surfaceFormatsLength, surface);
	
	VkSurfaceFormatKHR surfaceFormat = swapchain_chooseSwapSurfaceFormat(swapChainSupport.formats, surfaceFormatsLength);

	VkPresentModeKHR presentMode = swapchain_chooseSwapPresentMode(&swapChainSupport.presentModes);

	VkExtent2D extent = swapchain_chooseSwapExtent(&swapChainSupport.capabilities, window);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	// 0 means there is no max
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = { 0 };
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices qfIndices = device_findQueueFamilies(physicalDevice, surface);
	uint32_t queueFamilyIndices[] = {
		qfIndices.graphicsFamily,
		qfIndices.presentFamily
	};

	if (qfIndices.graphicsFamily != qfIndices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = NULL;
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	
	// Swap chains must be recreated on window resize, so this would hold the old swapchain
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	VkResult v = vkCreateSwapchainKHR(logicalDevice, &createInfo, NULL, swapChain);
	printf("\nCREATESWAPCHAIN RESUlT : %d\n", v);

	if (v != VK_SUCCESS) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create swapchain.");
		exit(VK_ERROR_INITIALIZATION_FAILED);
	}

	// Getting swapchain images
	uint32_t swapchainImageCount = 0;
	
	vkGetSwapchainImagesKHR(logicalDevice, *swapChain, &swapchainImageCount, NULL);

	*swapChainImages = (VkImage*)malloc(sizeof(VkImage) * swapchainImageCount);
	*swapChainImagesLength = swapchainImageCount;

	vkGetSwapchainImagesKHR(logicalDevice, *swapChain, &swapchainImageCount, *swapChainImages);

	*swapChainImageFormat = surfaceFormat.format;
	*swapChainExtent = extent;

	free(swapChainSupport.formats);
	free(swapChainSupport.presentModes);
}







