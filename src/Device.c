#include "Device.h"


int device_checkDeviceExtensionSupport(VkPhysicalDevice device) {
	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, NULL);

	VkExtensionProperties *availableExtensions = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * extensionCount);
	vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, availableExtensions);

	if (availableExtensions == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to allocate memory for available extensions");
		return NULL;
	}

	int supportedExtensions = 0;
	printf("\n%d available extensions; %d required extensions\n", (int)extensionCount, (int)(sizeof(deviceExtensions) / sizeof(deviceExtensions[0]))); // Type casting to int to avoid warning
	for (uint32_t i = 0; i < extensionCount; i++) {
		//printf("\n\tDEVICE EXTENSION : %s", availableExtensions[i].extensionName);
		for (uint32_t j = 0; j < sizeof(deviceExtensions) / sizeof(deviceExtensions[0]); j++) {
			if (strcmp(availableExtensions[i].extensionName, deviceExtensions[j]) == 0) {
				printf("\n\tSUPPORTED DEVICE EXT : %s\n", availableExtensions[i].extensionName);
				supportedExtensions++;
				break;
			}
		}
	}
	printf("\n%d extensions supported of %d required extensions.\n", (int)supportedExtensions, (int)(sizeof(deviceExtensions) / sizeof(deviceExtensions[0])));
	free(availableExtensions);
	return supportedExtensions;
}

uint32_t device_isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface) {

	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);

	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	QueueFamilyIndices qfIndices = device_findQueueFamilies(device, surface);
	int extensionsCount = sizeof(deviceExtensions) / sizeof(deviceExtensions[0]);
	int extensionsSupported = device_checkDeviceExtensionSupport(device);

	int swapChainAdequate = 0;
	if (extensionsSupported == extensionsCount) {
		SwapChainSupportDetails swapChainSupport = { 0 }; 
		swapChainSupport = swapchain_querySwapChainSupport(device, NULL, surface);
		// This check may not be reliable TODO : TEST
		swapChainAdequate = (swapChainSupport.presentModes != NULL && swapChainSupport.formats != NULL);
		//printf("%d", swapChainAdequate);
		free(swapChainSupport.formats);
		free(swapChainSupport.presentModes);
	} 
	// Device selection process
	// Idont think graphics family is properly being checked TODO : fix
	// Can be any integer, so compare if >= 0 --If the No. of supporrted extensions = the amount of requested extensions. --- if swap chain is not null
    // TODO: Fix always true warning v
	return (qfIndices.graphicsFamily >= 0 && extensionsSupported == extensionsCount && swapChainAdequate == 1); // This may not work correctly.
}

QueueFamilyIndices device_findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
	// Logic to find graphics queue family
	QueueFamilyIndices indices = { NULL };

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);

	VkQueueFamilyProperties *queueFamilies = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

	if (queueFamilies == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to allocate memory for queue families");
		return;
	}

	for (uint32_t i = 0; i < queueFamilyCount; i++) {
		if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i; 
		}

		VkBool32 presentSupport = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (presentSupport) {
			indices.presentFamily = i;
		}

	}

	free(queueFamilies);
	return indices;
}

void device_pickPhysicalDevice(VkInstance instance, VkPhysicalDevice *physicalDevice, VkSurfaceKHR surface) {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);

	if (deviceCount == 0) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to find a graphics card with Vulkan support!");
		exit(VK_ERROR_FEATURE_NOT_PRESENT);
	}

	VkPhysicalDevice *availableDevices = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, availableDevices);

	if (availableDevices == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to allocate memory for available devices");
		return NULL;
	}

	// Sets the device to use as the first suitable one found
	for (uint32_t i = 0; i < deviceCount; i++) {
		if (device_isDeviceSuitable(availableDevices[i], surface)) {
			*physicalDevice = availableDevices[i];
			break;
		}
	}

	if (*physicalDevice == VK_NULL_HANDLE) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to find a graphics card with Vulkan support!");
		exit(VK_ERROR_FEATURE_NOT_PRESENT);
	}

	free(availableDevices);
}

void device_createLogicalDevice(VkPhysicalDevice physicalDevice, VkDevice *logicalDevice, VkQueue *graphicsQueue, VkQueue *presentQueue, VkSurfaceKHR surface) {
	float queuePriority = 1.0f;
	QueueFamilyIndices qfIndices = device_findQueueFamilies(physicalDevice, surface);

	VkDeviceQueueCreateInfo queueCreateInfos[2];


	// TODO : Future-proof
	VkDeviceQueueCreateInfo queueCreateInfo = { 0 };
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = qfIndices.graphicsFamily;
	queueCreateInfo.queueCount = 1;
	queueCreateInfo.pQueuePriorities = &queuePriority;
	// These should not be explicitly stated, but generated. Only difference should be the family index
	VkDeviceQueueCreateInfo queueCreateInfo2 = { 0 };
	queueCreateInfo2.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo2.queueFamilyIndex = qfIndices.presentFamily;
	queueCreateInfo2.queueCount = 1;
	queueCreateInfo2.pQueuePriorities = &queuePriority;

	queueCreateInfos[0] = queueCreateInfo;
	queueCreateInfos[1] = queueCreateInfo2;


	VkPhysicalDeviceFeatures deviceFeatures = { VK_FALSE };

	VkDeviceCreateInfo createInfo = { 0 };
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos;
	createInfo.queueCreateInfoCount = sizeof(queueCreateInfos) / sizeof(queueCreateInfos[0]);
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.ppEnabledExtensionNames = deviceExtensions;
	createInfo.enabledExtensionCount = sizeof(deviceExtensions)/ sizeof(deviceExtensions[0]);

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = sizeof(validationLayers) / sizeof(validationLayers[0]);
		createInfo.ppEnabledLayerNames = validationLayers;
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physicalDevice, &createInfo, NULL, logicalDevice) != VK_SUCCESS) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create logical device");
		exit(VK_ERROR_INITIALIZATION_FAILED);
	}
	// the 0 index may cause issues because I offset the indices function.
	vkGetDeviceQueue(*logicalDevice, qfIndices.graphicsFamily, 0, graphicsQueue);
	// This does not crash but it may not work... be wary
	vkGetDeviceQueue(*logicalDevice, qfIndices.presentFamily, 0, presentQueue);

}