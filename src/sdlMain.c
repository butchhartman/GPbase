// SDL Code provided by :
// https://github.com/libsdl-org/SDL/blob/main/examples/renderer/01-clear/clear.c

// Much of the Vulkan code was provided by or adapted from :
// https://vulkan-tutorial.com/

#define DEBUG_MODE
#define DEBUG_VERBOSITY_WARNING

#include <GPbase.h>

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
/*
Returns a list of extensions needed by Vulkan for the application.
Assignes the passed count pointer to the number of extensions required.
If validation layers are enabled, appends VK_EXT_DEBUG_UTILS_EXTENSION_NAME to the
end of the returned array and adds 1 to the count.
*/
const char **getRequiredExtensions(uint32_t *count) {
	const char **SDL3Extensions;

	SDL3Extensions = SDL_Vulkan_GetInstanceExtensions(count);

	if (enableValidationLayers) {
		const char **extensions = malloc(sizeof(const char*) * (*(count)+1));


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

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
	// Logic to find graphics queue family
	QueueFamilyIndices indices = { NULL };

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);

	VkQueueFamilyProperties *queueFamilies = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

	for (uint32_t i = 0; i < queueFamilyCount; i++) {
		if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i+1; // Adds one to have an offset so I can check truth by comparing it against 0
		}

		VkBool32 presentSupport = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (presentSupport) {
			indices.presentFamily = i+1;
		}

	}

	free(queueFamilies);
	return indices;
}

uint32_t isDeviceSuitable(VkPhysicalDevice device) {
	uint32_t hasRequiredFeatures = 0;

	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);

	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	QueueFamilyIndices indices = findQueueFamilies(device);
	// TEST CONDITION! TODO: REMOVE
	//if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
		hasRequiredFeatures = 1;
	//}

	// Device selection process
	// Can be any integer, so compare if >= 0
	return (indices.graphicsFamily > 0 && hasRequiredFeatures); // indices are offset by 1
}

void pickPhysicalDevice() {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);

	if (deviceCount == 0) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to find a graphics card with Vulkan support!");
		exit(VK_ERROR_FEATURE_NOT_PRESENT);
	}

	VkPhysicalDevice *availableDevices = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, availableDevices);

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

	for (uint32_t i = 0; i < extensionCount; i++) {
		SDL_Log("\t%s - v%d", extensions[i].extensionName, extensions[i].specVersion);
	}

	SDL_Log("Vulkan initialized.");

	free(extensions);

}

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
	createInfo.enabledExtensionCount = 0;

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

void createSurface() {
	// This function only returns success if the instance and window parameters are flipped....
	// SDL_Vulkan_CreateSurface returns 0 upon success.
	if (SDL_Vulkan_CreateSurface(window, instance, NULL, &surface) == 0) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create a window surface.");
		exit(VK_ERROR_INITIALIZATION_FAILED);
	}
}

/* 
	Function that creates the Vulkan instance. Must be called after
	loading Vulkan via SDL3.
*/
VkResult Vk_Init() {
	createInstance();

	setupDebugMessenger(instance, &debugMessenger);
	
	// Must be called right after create instance because it can influence physical device selection
	createSurface();

	pickPhysicalDevice();

	createLogicalDevice();
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
	if (!SDL_CreateWindowAndRenderer("examples/renderer/clear", DEFAULT_WIDTH, DEFAULT_HEIGHT, SDL_WINDOW_VULKAN, &window, &renderer)) {
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
	const double now = ((double)SDL_GetTicks()) / 1000.0f; // converts from ms to s

	// The RGB values of the color that is drawn. The sin functions make it fade
	const float red = (float)(0.5 + 0.5 * SDL_sin(now));
	const float green = (float)(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 2 / 3));
	const float blue = (float)(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 4 / 3));
	SDL_SetRenderDrawColorFloat(renderer, red, green, blue, SDL_ALPHA_OPAQUE_FLOAT); // new color, full alpha

	// Clears the window with the draw color
	SDL_RenderClear(renderer);

	// Put the newly-cleared rendering to the screen.
	SDL_RenderPresent(renderer);

	return SDL_APP_CONTINUE; // Carry on with the program
}

/* 
	Function ran by SDL when the application closes. Stores cleanup procedures
*/
void SDL_AppQuit(void* appstate, SDL_AppResult result) {
	// SDL cleans up the window/renderer for us
	SDL_Log("Application terminating.");
	/*
		I could never get the validation layers to proc an error when removing either
		the destruction of the validation layer or the destruction of the instance....
		However, it did proc in other situations so I can only assume the validation layers work.
	*/
	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, NULL);
	}

	vkDestroyDevice(logicalDevice, NULL);
	vkDestroySurfaceKHR(instance, surface, NULL);
	vkDestroyInstance(instance, NULL);
}