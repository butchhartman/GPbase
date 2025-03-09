// SDL Code provided by :
// https://github.com/libsdl-org/SDL/blob/main/examples/renderer/01-clear/clear.c

// Much of the Vulkan code was provided by or adapted from :
// https://vulkan-tutorial.com/

#define DEBUG_MODE
#define DEBUG_VERBOSITY_WARNING

/*
* Through testing I have determined that the validation layers are causing memory leaks. 
* No idea why but at least the problems isnt serious since they can be disabled.
*/

#include <GPbase.h>

// TODO : Use libshaderc to compile shaders instead of batch file

// renderer is used to draw into the window every frame.
static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;

static VkInstance instance;
VkDebugUtilsMessengerEXT debugMessenger;

// This is the graphics card that will be rendered with
VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

// Logical device that interfaces with the physical device
VkDevice logicalDevice;

// Handle to interface with the logicalDevice's auto-generated queue.
VkQueue graphicsQueue;

// The surface to present rendered images to.
VkSurfaceKHR surface;

// Handle to the presentQueue.
VkQueue presentQueue;

// Handle to the swapchain
VkSwapchainKHR swapChain;

// Array of swapchain images
VkImage *swapChainImages;
uint32_t swapChainImagesLength;
// In use format of the swapchain
VkFormat swapChainImageFormat;
// In use extent of the swapchain
VkExtent2D swapChainExtent;

// Array of the image views on the swap chain
//An image view is quite literally a view into an image. 
// It describes how to access the image and which part of 
// the image to access, for example if it should be treated
// as a 2D texture depth texture without any mipmapping levels (vulkan-tutorial.com).
VkImageView *swapChainImageViews;
uint32_t swapChainImageViewsLength;
// Reference to render pass
VkRenderPass renderPass;

// Reference to graphics pipeline layout
VkPipelineLayout pipelineLayout;

// Reference to the graphics pipeline itself
VkPipeline graphicsPipeline;

//  Array of framebuffers for all images on the swapchain
VkFramebuffer *swapChainFramebuffers;
uint32_t swapChainFrameBuffersLength;

// Reference to the command pool
VkCommandPool commandPool;

// Arrays of multiple command buffers / semaphores/ fences
VkCommandBuffer *commandBuffers;

VkSemaphore *imageAvailableSemaphores;
VkSemaphore *renderFinishedSemaphores;
VkFence *inFlightFences;

uint32_t currentFrame = 0;


/*
* When moving funcs to separate headers : 
* Document how to use the function in the header file, or more accurately close to the declaration.
*
* Document how the function works (if it's not obvious from the code) in the source file, or more accurately, close to the definition.
*/
// TODO : Fix the memory leak

/*
* *********************************************************************************************************************
*	When passed spv bytecode and the size of that code, creates and returns a VkShaderModule.
* ********************************************************************************************************************
* @param code - an unsigned char array containing the raw spv bytecode
* @param size - a uint32_t representing the length of the code array.
* 
* @return VkShaderModule
*/
VkShaderModule createShaderModule(const unsigned char* code, uint32_t size) {

	VkShaderModuleCreateInfo createInfo = { 0 };
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = size;
	// This is a uint32_t*, but it works with unsigned char*. Typecast is just in case it matters.
	createInfo.pCode = (const uint32_t*)code;

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(logicalDevice, &createInfo, NULL, &shaderModule) != VK_SUCCESS) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create shader module");
		exit(VK_ERROR_INITIALIZATION_FAILED);
	}
	
	return shaderModule;
}
/*
* *********************************************************************************************************************
* This function returns a VkExtent2D describing the window width and height
* *********************************************************************************************************************
* @param capabilities - Currently has no function
* @return VkExtent2D - Populated with window width and window height
*/
VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR* capabilities) {
	
	// TODO : revist this function to add more sophisticated procedures

	int width, height;

	SDL_GetWindowSizeInPixels(window, &width, &height);

	VkExtent2D actualExtent = {
		(uint32_t)width,
		(uint32_t)(height)
	};

	return actualExtent;
}

/*
* *********************************************************************************************************************
*	Returns an available swap present mode
* *********************************************************************************************************************
* 
* @param availablePresentModes - Currently has no function
* @return VkPresentModeKHR - Currently only returns mode FIFO
* 
*/
VkPresentModeKHR chooseSwapPresentMode(const VkPresentModeKHR **availablePresentModes) {
	// TODO : ADD SELECTION LOGIC
	// Selection not needed because fifo is guaranteed.
	return VK_PRESENT_MODE_FIFO_KHR;
}
/*
* 
* *********************************************************************************************************************
*	Returns a surface format from the passed available formats that matches the selection parameters
* *********************************************************************************************************************
* 
* @param availableFormats - An array of the available surface formats
* @param length - A uint32_t representing the length of the available format array
* 
* @return VkSurfaceFormatKHR - An available format that matches the selection parameters
* 
*/
VkSurfaceFormatKHR chooseSwapSurfaceFormat(const VkSurfaceFormatKHR *availableFormats, uint32_t length) {
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
/*
* *********************************************************************************************************************
*	Populates a SwapChainSupportDetails struct with the physical device's surface capabilities, the surface formats' properties,
*	and the present modes' properties.
* *********************************************************************************************************************
* 
* @param device - The physical device with which to query information
* @param count - Pointer to a uint32_t which is assigned the length of the formatcount
* 
* @return  SwapChainSupportDetails - The populated detail struct
* 
*/
SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, uint32_t* count) {
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

/*
* *********************************************************************************************************************
*	Enumerates the extensions supported by the physical device and compares it against the extensions required by the application.
* *********************************************************************************************************************
* 
* @param device - The physical device with which to query data
* 
* @return supportedExtensions - The number of supported extensions
*/
int checkDeviceExtensionSupport(VkPhysicalDevice device) {
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

/*
* Note : This function gets instance extensions, NOT device extensions
* *********************************************************************************************************************
*	Retrieves the required instance extensions by asking SDL which ones are needed.
*	If validation layers are enabled, includes VK_EXT_debug_utils in the required extensions array.
* *********************************************************************************************************************
* 
* @param count - A uint32_t which is assigned with the length of the required extensions array.
* 
* @return extensions - A const char** which contains the names of the required extensions
*/
const char **getRequiredExtensions(uint32_t *count) {
	/*
	* This const char ** should never be freed because it points to an array owned by SDL.
	* This array should not be freed.
	*/
	const char **SDL3Extensions;

	SDL3Extensions = SDL_Vulkan_GetInstanceExtensions(count);

	if (enableValidationLayers) {
		const char **extensions = malloc(sizeof(const char*) * (*count+1));

		if (extensions == NULL) {
			SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to allocate memory for extensions");
			return NULL;
		}

		for (uint32_t i = 0; i < *count; i++) {
			extensions[i] = SDL3Extensions[i];
		}
		extensions[*(count)] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;

		for (uint32_t i = 0; i < *(count) + 1; i++) {
			SDL_Log("REQUIRED EXTENSION: %s", extensions[i]);
		}

		*count += 1;
		return extensions;
	}
	else {
		return SDL3Extensions;
	}

}


/*
* *********************************************************************************************************************
*	Populates a QueueFamilyIndices struct with the indices of a graphics queue family and a present queue family who
*	both meet the necessary requirements.
* *********************************************************************************************************************
* 
* @param device - The phyiscal device with which to query information
* 
* @return QueueFamilyIndicies - A struct containing the graphicsFamily and presentFamily indices
*/
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
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

/*
* *********************************************************************************************************************
*	Determines if the physical device supports the queueFamilies, extensions, and swapchain requirements.
* *********************************************************************************************************************
* 
* @params device - The physical device with which to query information
* 
* @return uint32_t - Returns 1 if physical device is suitable, 0 if not
* 
*/
uint32_t isDeviceSuitable(VkPhysicalDevice device) {

	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);

	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	QueueFamilyIndices indices = findQueueFamilies(device);
	int extensionsCount = sizeof(deviceExtensions) / sizeof(deviceExtensions[0]);
	int extensionsSupported = checkDeviceExtensionSupport(device);

	int swapChainAdequate = 0;
	if (extensionsSupported == extensionsCount) {
		SwapChainSupportDetails swapChainSupport = { 0 }; 
		swapChainSupport = querySwapChainSupport(device, NULL);
		// This check may not be reliable TODO : TEST
		swapChainAdequate = (swapChainSupport.presentModes != NULL && swapChainSupport.formats != NULL);
		//printf("%d", swapChainAdequate);
		free(swapChainSupport.formats);
		free(swapChainSupport.presentModes);
	} 
	// Device selection process
	// Idont think graphics family is properly being checked TODO : fix
	// Can be any integer, so compare if >= 0 --If the No. of supporrted extensions = the amount of requested extensions. --- if swap chain is not null
	return (indices.graphicsFamily >= 0 && extensionsSupported == extensionsCount && swapChainAdequate == 1); // This may not work correctly.
}

void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
	VkCommandBufferBeginInfo beginInfo = { 0 };
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = NULL;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to begin recording command buffer");
		exit(VK_ERROR_INITIALIZATION_FAILED);
	}

	VkRenderPassBeginInfo renderPassInfo = { 0 };
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];

	//renderPassInfo.renderArea.offset = { 0,0 }; should be zero by default
	renderPassInfo.renderArea.extent = swapChainExtent;

	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	// No error handling because all recording commands (vkCmd) return void
	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	
	// Bind the graphics pipeline
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

	// I set the viewport and scissor state for this pipeline to be dynamic, so they must be specified before issuing a draw command
	VkViewport viewport = { 0 };
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapChainExtent.width;
	viewport.height = (float)swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor = { 0 };
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent = swapChainExtent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	// Here it is... draw command for the triangle.
	vkCmdDraw(commandBuffer, 3, 1, 0, 0);

	vkCmdEndRenderPass(commandBuffer);

	// Now error checking can happen
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to record command buffer");
		exit(VK_ERROR_UNKNOWN);
	}
}



/*
*	A physcial device in Vulkan is a representation of the host machine's physical graphics card.
*	One can pick as many physical devices as are available and use them simultaneously.
*	However, suitability checks must be performed to make sure the physical device can support the required Vulkan features.
*	
*	Additionally, Vulkan required nearly all commands to be submitted to a queue and it is required to
*	check which queue families are supported by the physical device and if those families support the required features.
* 
*	There are different types of queues that come from queue families. These families are specialized and
*	only allow a certain subset of commands (graphics, etc).
* 
*********************************************************************************************************************** 
*	This function queries and enumerates available physical devices and checks their suitability by calling isDeviceSuitable.
*	As previously stated
***********************************************************************************************************************
*/
void pickPhysicalDevice() {
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
		if (isDeviceSuitable(availableDevices[i])) {
			physicalDevice = availableDevices[i];
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to find a graphics card with Vulkan support!");
		exit(VK_ERROR_FEATURE_NOT_PRESENT);
	}

	free(availableDevices);
}

/*
*	A Vulkan instance is the link between one's application and the vulkan library.
*	
*********************************************************************************************************************** 
*	This function creates a vulkan instance with all the required extensions specified. Some extensions are required and their names provided by SDL
*	Additionally, if debug mode is enabled, provides the instance with the required validation layers specified.
***********************************************************************************************************************
*/
void createInstance() {
	if (enableValidationLayers) {
		SDL_Log("=========Debug mode=========");
	}

	if (enableValidationLayers && !checkValidationLayerSupport()) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Validation layers requested, but not available.");
		exit(VK_ERROR_LAYER_NOT_PRESENT);
	}


	// Initializing Vulkan
	// Info about the application. Technically optional, but useful
	VkApplicationInfo appinfo = { 0 };
	appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appinfo.pApplicationName = "Hello, Vulkan";
	appinfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0); // Variant is the first variable. Should always be 0
	appinfo.pEngineName = "No Engine";
	appinfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
	appinfo.apiVersion = VK_API_VERSION_1_4;
	appinfo.pNext = NULL;

	// Not optional information. Tells Vulkan driver which extensions and validation layers to use
	VkInstanceCreateInfo createInfo = { 0 };
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appinfo;

	const char** SDL3Extensions;
	uint32_t SDL3ExtensionCount;
	SDL3Extensions = getRequiredExtensions(&SDL3ExtensionCount);

	createInfo.enabledExtensionCount = SDL3ExtensionCount;
	createInfo.ppEnabledExtensionNames = SDL3Extensions;
	createInfo.enabledLayerCount = 0;

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = { 0 };


	// verification layers
	if (enableValidationLayers) {
		SDL_Log("Validation layers active.");
		createInfo.enabledLayerCount = sizeof(validationLayers) / sizeof(validationLayers[0]);
		createInfo.ppEnabledLayerNames = validationLayers;

		populateDebugMessengerCreateInfo(&debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)(&debugCreateInfo);
	}
	else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = NULL;
	}

	VkResult result = vkCreateInstance(&createInfo, NULL, &instance);
	if (result != VK_SUCCESS) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create Vulkan instance.");
		exit(VK_ERROR_INITIALIZATION_FAILED);
	}
	else {
		SDL_Log("Vulkan instance successfully created.");
	}

	// Skipped implementing the VK_ERROR_INCOMPATIBLE_DRIVER workaround. Dont know if it's necessary, sorry mac users.

	// Getting # of supported extensions and their names
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);

	SDL_Log("%d extensions supported.", extensionCount);

	VkExtensionProperties* extensions = (VkExtensionProperties*)malloc(extensionCount * sizeof(VkExtensionProperties));

	vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, extensions);

	if (extensions == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to allocate memory for supported extensions");
		return;
	}

	for (uint32_t i = 0; i < extensionCount; i++) {
		SDL_Log("\t%s - v%d", extensions[i].extensionName, extensions[i].specVersion);
	}

	SDL_Log("Vulkan initialized.");

	free(extensions);

	if (enableValidationLayers) {
		/*
		*	This is only freed because when validation layers are active, the SDL internal extensions array
		*	is duplicated so the validation layer extensions can be appended. Therefore, I (instead of SDL)
		*	own the array which can be freed.
		* 
		*	Ts pmo ...
		*/
		free(SDL3Extensions);
	}
}
/*
*	A logical device represents an instance of the physical device's Vulkan implementation.
*	The logical device allows one to interface with the physical device.
* 
* **********************************************************************************************************************
*	This function gets the queue families from the physical devicve and creates an array of (2 in this inflexible case)
*	VKDevicequeueCreateInfo types. It then passes that info into the logical device's createInfo. Creates the logical device.
* **********************************************************************************************************************
*/
void createLogicalDevice() {
	float queuePriority = 1.0f;
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	VkDeviceQueueCreateInfo queueCreateInfos[2];


	// TODO : Future-proof
	VkDeviceQueueCreateInfo queueCreateInfo = { 0 };
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
	queueCreateInfo.queueCount = 1;
	queueCreateInfo.pQueuePriorities = &queuePriority;
	// These should not be explicitly stated, but generated. Only difference should be the family index
	VkDeviceQueueCreateInfo queueCreateInfo2 = { 0 };
	queueCreateInfo2.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo2.queueFamilyIndex = indices.presentFamily;
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

	if (vkCreateDevice(physicalDevice, &createInfo, NULL, &logicalDevice) != VK_SUCCESS) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create logical device");
		exit(VK_ERROR_INITIALIZATION_FAILED);
	}
	// the 0 index may cause issues because I offset the indices function.
	vkGetDeviceQueue(logicalDevice, indices.graphicsFamily, 0, &graphicsQueue);
	// This does not crash but it may not work... be wary
	vkGetDeviceQueue(logicalDevice, indices.presentFamily, 0, &presentQueue);

}

/*
*	Vulkan does not care what platform the user is on. It cannot interface with a platform's window system on its own.
*	To connect Vulkan and the window system, one must create a window surface using the platform-specific WSI extensions.
*	These aforementioned extensions are typically retrieved from the window library in use. SDL in this case.
* 
**********************************************************************************************************************
*	This function creates the surface via SDL's implementation of the create surface function.
**********************************************************************************************************************
*/
void createSurface() {
	if (SDL_Vulkan_CreateSurface(window, instance, NULL, &surface) == 0) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create a window surface.");
		exit(VK_ERROR_INITIALIZATION_FAILED);
	}
	printf("SURF ERR %s", SDL_GetError());

}
/*
*	The swap chain provides the ability to present rendering results to a surface via an array of images.
*	It is essentially a queue of images waiting to be rendered.
* 
* *********************************************************************************************************************
*	This function queries swap chain data from the physical device and uses it to create the swap chain.
*	It also retrieves the queue families and sets the image sharing mode accordingly.
* *********************************************************************************************************************
*/
void createSwapChain() {
	uint32_t surfaceFormatsLength = NULL;

	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice, &surfaceFormatsLength);
	
	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats, surfaceFormatsLength);

	VkPresentModeKHR presentMode = chooseSwapPresentMode(&swapChainSupport.presentModes);

	VkExtent2D extent = chooseSwapExtent(&swapChainSupport.capabilities);

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

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
	uint32_t queueFamilyIndices[] = {
		indices.graphicsFamily,
		indices.presentFamily
	};

	if (indices.graphicsFamily != indices.presentFamily) {
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

	VkResult v = vkCreateSwapchainKHR(logicalDevice, &createInfo, NULL, &swapChain);
	printf("\nCREATESWAPCHAIN RESUlT : %d\n", v);

	if (v != VK_SUCCESS) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create swapchain.");
		exit(VK_ERROR_INITIALIZATION_FAILED);
	}

	// Getting swapchain images
	uint32_t swapchainImageCount = 0;
	
	vkGetSwapchainImagesKHR(logicalDevice, swapChain, &swapchainImageCount, NULL);

	swapChainImages = (VkImage*)malloc(sizeof(VkImage) * swapchainImageCount);
	swapChainImagesLength = swapchainImageCount;

	vkGetSwapchainImagesKHR(logicalDevice, swapChain, &swapchainImageCount, swapChainImages);

	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;

	free(swapChainSupport.formats);
	free(swapChainSupport.presentModes);
}

/*
*	An image view describes how to access an image and which part of that image to access.
*	For example, an image view is what tells Vulkan that an accessed image should be treated as a 2D texture depth texture.
* 
* *********************************************************************************************************************
*	This function creates an array of image views that is the same length as the swapChainImages array.
*	In other words, creates one image view for each swap chain image
* *********************************************************************************************************************
*/
void createImageViews() {
	swapChainImageViews = (VkImageView*)malloc(sizeof(VkImage) * swapChainImagesLength);

	if (swapChainImageViews == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to allocate memory for image views");
		return;
	}

	swapChainImageViewsLength = swapChainImagesLength;
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

		if (vkCreateImageView(logicalDevice, &createInfo, NULL, &swapChainImageViews[i]) != VK_SUCCESS) {
			SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create image views");
			exit(VK_ERROR_INITIALIZATION_FAILED);
		}
	}
}

/*
*	The graphics pipeline is a sequence of operaions that take the vertices & textures of one's meshes
*	all the way to to the pixels in the render targets.
* 
*	The minimum stage programming required is the vertex and fragment shader, as shown here.
* 
* *********************************************************************************************************************
*	This function loads the fragment and vertex shaders, along with other info structs (color blending, multisampling, type of geometry to draw, etc)
*	and uses it to create the graphics pipeline.
* *********************************************************************************************************************
*
*/
void createGraphicsPipeline() {
	uint32_t vertShaderSize;
	uint32_t fragShaderSize;

	const unsigned char* vertShaderBin = readFileBin("../../shaders/vert.spv", &vertShaderSize);
	const unsigned char* fragShaderBin = readFileBin("../../shaders/frag.spv", &fragShaderSize);

	VkShaderModule vertShaderModule = createShaderModule(vertShaderBin, vertShaderSize);
	VkShaderModule fragShaderModule = createShaderModule(fragShaderBin, fragShaderSize);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = { 0 };
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = { 0 };
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = {
		vertShaderStageInfo, fragShaderStageInfo
	};


	// TODO: expand in vertex buffer chapter
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = { 0 };
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.pVertexBindingDescriptions = NULL;
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	vertexInputInfo.pVertexAttributeDescriptions = NULL;

	// Defines what type of geometry will be drawn from the vertices
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = { 0 };
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	// Describes the region of the framebuffer that the output will be rendered to
	VkViewport viewport = { 0 };
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapChainExtent.width;
	viewport.height = (float)swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkPipelineDynamicStateCreateInfo dynamicState = { 0 };
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = sizeof(dynamicStates)/sizeof(dynamicStates[0]);
	dynamicState.pDynamicStates = dynamicStates;

	// Only must specify the count because they are dynamic and will be setup at run time
	VkPipelineViewportStateCreateInfo viewportState = { 0 };
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizer = { 0 };
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f;
	rasterizer.depthBiasClamp = 0.0f;
	rasterizer.depthBiasSlopeFactor = 0.0f;

	// Disabled for now, requires a GPU feature
	VkPipelineMultisampleStateCreateInfo multisampling = { 0 };
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f;
	multisampling.pSampleMask = NULL;
	multisampling.alphaToCoverageEnable = VK_FALSE;
	multisampling.alphaToOneEnable = VK_FALSE;

	// Depth and/or stencil buffer
	//////////////////////////////

	// stuff for blending colors. Currently disabled.
	VkPipelineColorBlendAttachmentState colorBlendAttachment = { 0 };
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT 
		| VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending = { 0 };
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	// Create graphics pipeline layout
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = { 0 };
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = NULL;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = NULL;

	if (vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, NULL, &pipelineLayout) != VK_SUCCESS) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create pipeline layout");
		exit(VK_ERROR_INITIALIZATION_FAILED);
	}

	// FINALLY!! CREATING THE GRAPHICS PIPELINE!!!
	VkGraphicsPipelineCreateInfo pipelineInfo = { 0 };
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;

	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = NULL;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;

	pipelineInfo.layout = pipelineLayout;

	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = NULL;
	pipelineInfo.basePipelineIndex = -1;


	if (vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &graphicsPipeline) != VK_SUCCESS) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create graphics pipeline");
		exit(VK_ERROR_INITIALIZATION_FAILED);
	}

	// These can be deleted after they are used because the bytecode gets linked and compiled by the GPU.
	vkDestroyShaderModule(logicalDevice, vertShaderModule, NULL);
	vkDestroyShaderModule(logicalDevice, fragShaderModule, NULL);
	free(vertShaderBin);
	free(fragShaderBin);

}
/*
*	A render pass is an object that specifies information about the framebuffer attachments that will be used during rendering
*	For example, how many color and depth buffers there will be, how many samples to use for them, and how their contents should be handled
*	throughout rendering operations.
* 
* *********************************************************************************************************************
*	This function creates colorAttachment, colorAttachmentRef, subpass, and renderPass info.
*	The created info is then used to create a render pass.
* *********************************************************************************************************************
*/
void createRenderPass() {

	VkAttachmentDescription colorAttachment = { 0 };
	colorAttachment.format = swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	// directly referenced by layout(location = 0) out vec4 outColor in fragment shader
	VkAttachmentReference colorAttachmentRef = { 0 };
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = { 0 };
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkRenderPassCreateInfo renderPassInfo = { 0 };
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	VkSubpassDependency dependency = { 0 };
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;

	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;

	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(logicalDevice, &renderPassInfo, NULL, &renderPass) != VK_SUCCESS) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create render pass");
		exit(VK_ERROR_INITIALIZATION_FAILED);
	}
}

/*
*	A framebuffer object references all of the image views which represent all the framebuffer attachments. However, the attachment used depends on
*	which image the swapchain returns when one is retrieved.
* 
*	Therefore, there must be one framebuffer for each image on the swapchain. The framebuffer corresponding to the
*	retrieve image is the one that will be used. In other words, there must be one framebuffer that accesses images
*	in the way described by the image views for each image on the swapchain.
* 
* *********************************************************************************************************************
*	This function copies the attachments specified by the swapChainImageViews and creates a framebuffer for each image 
*	on the swapchain (which is the same as the ).
* *********************************************************************************************************************
*
*/
void createFramebuffers() {
	swapChainFramebuffers =
	(VkFramebuffer*)malloc(sizeof(VkFramebuffer) * swapChainImageViewsLength);

	swapChainFrameBuffersLength = swapChainImageViewsLength;

	if (swapChainFramebuffers == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to allocate memory for swap chain frame buffers");
		return;
	}

	for (uint32_t i = 0; i < swapChainImagesLength; i++) {
		VkImageView attachments[] = {
			swapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo = { 0 };
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;


		if (vkCreateFramebuffer(logicalDevice, &framebufferInfo, NULL, &swapChainFramebuffers[i]) != VK_SUCCESS) {
			SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create framebuffer");
			exit(VK_ERROR_INITIALIZATION_FAILED);
		}
	}
}
/*
*	Commands in Vulkan are not directly executed, they are instead recorded into a command buffer and
*	submitted together.
* 
*	Commands pools manage the memory that is used to store command buffers.
* 
* *********************************************************************************************************************
*	This function creates a command pool for the graphics queue family.
* *********************************************************************************************************************
*
*/
void createCommandPool() {
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

	VkCommandPoolCreateInfo poolInfo = { 0 };
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;

	if (vkCreateCommandPool(logicalDevice, &poolInfo, NULL, &commandPool) != VK_SUCCESS) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create command pool");
		exit(VK_ERROR_INITIALIZATION_FAILED);
	}
}
/*
*	Command buffers store Vulkan rendering commands so they can be submitted all at once from the same place.
* 
* *********************************************************************************************************************
*	This function uses the command pool to create a command buffer
* *********************************************************************************************************************
*
*/
void createCommandBuffers() {
	commandBuffers = (VkCommandBuffer*)malloc(sizeof(VkCommandBuffer) * MAX_FRAMES_IN_FLIGHT);// TODO : FREE

	VkCommandBufferAllocateInfo allocInfo = { 0 };
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)MAX_FRAMES_IN_FLIGHT;

	if (vkAllocateCommandBuffers(logicalDevice, &allocInfo, commandBuffers) != VK_SUCCESS) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create command buffer");
		exit(VK_ERROR_INITIALIZATION_FAILED);
	}
}
/*
*	Vulkan leaves it to the user to handle synchronization. This is done by using fences and semaphores
* 
*	Semaphores - Either signaled or unsignaled, can be used by the GPU to perform commands sequentially
* 
*	Fences - Either signaled or unsignaled, can be used by the CPU to wait for a GPU command to finish.
* 
* *********************************************************************************************************************
*	This function creates a semaphore and a fence.
* *********************************************************************************************************************
*
*/
void createSyncObjects() {
	imageAvailableSemaphores = (VkSemaphore*)malloc(sizeof(VkSemaphore) * MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores = (VkSemaphore*)malloc(sizeof(VkSemaphore) * MAX_FRAMES_IN_FLIGHT);
	inFlightFences = (VkFence*)malloc(sizeof(VkFence) * MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo = { 0 };
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = { 0 };
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

		if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, NULL, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(logicalDevice, &semaphoreInfo, NULL, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(logicalDevice, &fenceInfo, NULL, &inFlightFences[i]) != VK_SUCCESS) {
			SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create sync objects");
			exit(VK_ERROR_INITIALIZATION_FAILED);
		}

	}
}

/*
* *********************************************************************************************************************
*	Waits for the global inFlightFence fence to signal, then draws frame to screen by pulling an image from the swap chain and submitting it to the queue.
*	Also creates two semaphores to wait for the image to be available and to wait for the render to finish.
* *********************************************************************************************************************
*/
void drawFrame() {
	// semaphores - gpu
	// fences - cpu
	vkWaitForFences(logicalDevice, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
	vkResetFences(logicalDevice, 1, &inFlightFences[currentFrame]);

	uint32_t imageIndex;
	vkAcquireNextImageKHR(logicalDevice, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
	
	vkResetCommandBuffer(commandBuffers[currentFrame], 0);
	
	recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

	VkSubmitInfo submitInfo = { 0 };
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame]};
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to submit draw command buffer");
		exit(VK_ERROR_UNKNOWN); 
	}

	VkPresentInfoKHR presentInfo = { 0 };
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = NULL;

	vkQueuePresentKHR(presentQueue, &presentInfo);

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}



/* 
	Function that initializes all the structures needed prior to
	Vulkan outputting to the screen.
*/
VkResult Vk_Init() {
	createInstance();

	setupDebugMessenger(instance, &debugMessenger);
	
	// Must be called right after create instance because it can influence physical device selection
	createSurface();

	pickPhysicalDevice();

	createLogicalDevice();

	createSwapChain();

	createImageViews();

	createRenderPass();

	createGraphicsPipeline();

	createFramebuffers();

	createCommandPool();

	createCommandBuffers();

	createSyncObjects();

	return VK_SUCCESS;
}



/*
	Initialization callback ran by SDL when the application starts.
*/
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {

	SDL_SetAppMetadata("Example Renderer Clear", "1.0", "com.example.renderer-clear");

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}
	
	// Loads Vulkan by passing SDL_WINDOW_VULKAN as a window flag
	window = SDL_CreateWindow("examples/renderer/clear", DEFAULT_WIDTH, DEFAULT_HEIGHT, SDL_WINDOW_VULKAN);
	if (window == NULL) {
		SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	if (Vk_Init() != VK_SUCCESS) {
		return SDL_APP_FAILURE;
	}

	return SDL_APP_CONTINUE; // The init function ran successfully
}

/*
	Function that is called by SDL whenever an input event is proc'd
*/
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {

	if (event->key.scancode == SDL_SCANCODE_ESCAPE) {
		return SDL_APP_SUCCESS;
	}

	if (event->type == SDL_EVENT_QUIT) {
		return SDL_APP_SUCCESS; // Ends the program, reports a success to the OS. "Program ran successfully"
	}

	return SDL_APP_CONTINUE; // Carry on with the program
}

/*
	Function ran once per frame by SDL. Similar to the Update() loop of game engines.
*/
SDL_AppResult SDL_AppIterate(void* appstate) {

	drawFrame();

	return SDL_APP_CONTINUE; // Carry on with the program
}

/* 
	Function ran by SDL when the application closes. Stores cleanup procedures
*/
void SDL_AppQuit(void* appstate, SDL_AppResult result) {
	// SDL cleans up the window/renderer for us
	SDL_Log("Application terminating.");
	vkDeviceWaitIdle(logicalDevice);
	/*
		I could never get the validation layers to proc an error when removing either
		the destruction of the validation layer or the destruction of the instance....
		However, it did proc in other situations so I can only assume the validation layers work.
	*/
	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, NULL);
	}
	// the children yearn for for loops
	for (uint32_t i = 0; i < swapChainImageViewsLength; i++) {
		vkDestroyImageView(logicalDevice, swapChainImageViews[i], NULL);
	}
	free(swapChainImageViews);

	for (uint32_t i = 0; i < swapChainFrameBuffersLength; i++) {
		vkDestroyFramebuffer(logicalDevice, swapChainFramebuffers[i], NULL);
	}
	
	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(logicalDevice, imageAvailableSemaphores[i], NULL);
		vkDestroySemaphore(logicalDevice, renderFinishedSemaphores[i], NULL);
		vkDestroyFence(logicalDevice, inFlightFences[i], NULL);
	}

	free(swapChainFramebuffers);
	free(swapChainImages);
	vkDestroyCommandPool(logicalDevice, commandPool, NULL);
	vkDestroyPipeline(logicalDevice, graphicsPipeline, NULL);
	vkDestroyPipelineLayout(logicalDevice, pipelineLayout, NULL);
	vkDestroyRenderPass(logicalDevice, renderPass, NULL);
	vkDestroySwapchainKHR(logicalDevice, swapChain, NULL);
	vkDestroyDevice(logicalDevice, NULL);
	vkDestroySurfaceKHR(instance, surface, NULL);
	vkDestroyInstance(instance, NULL);
	SDL_Quit();
	//_CrtDumpMemoryLeaks();
}