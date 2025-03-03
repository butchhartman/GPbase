// Code provided by :
// https://github.com/libsdl-org/SDL/blob/main/examples/renderer/01-clear/clear.c
#define DEBUG_MODE
#include <GPbase.h>

// renderer is used to draw into the window every frame.
static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static VkInstance instance;
VkDebugUtilsMessengerEXT debugMessenger;


void DestroyDebugUtilsMessengerEXT(VkInstance vinstance,
	VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {

	PFN_vkDestroyDebugUtilsMessengerEXT func =
		(PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vinstance, 
			"vkDestroyDebugUtilsMessengerEXT");

	if (func != NULL) {
		func(vinstance, debugMessenger, pAllocator);
	}

}

VkResult CreateDebugUtilMessengerEXT(VkInstance vinstance,
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator,
	VkDebugUtilsMessengerEXT* pDebugMessenger) {

	PFN_vkDestroyDebugUtilsMessengerEXT func = 
		(PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vinstance, "vkDestroyDebugUtilsMessengerEXT");
	
	if (func != NULL) {
		func(vinstance, debugMessenger, pAllocator);
	}
}


static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {


	if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
		printf("\nvalidation layer: %s", pCallbackData->pMessage);
	}

	return VK_FALSE;
}

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT *createInfo) {
	memset(createInfo, 0, sizeof(VkDebugUtilsMessengerCreateInfoEXT));
	createInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo->pfnUserCallback = debugCallback;
	createInfo->pUserData = NULL;
}

void setupDebugMessenger() {
	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo = { 0 };
	populateDebugMessengerCreateInfo(&createInfo);

	if (CreateDebugUtilMessengerEXT(instance, &createInfo, NULL, &debugMessenger) != VK_SUCCESS) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to set up debug messenger.");
	}

}



const char* getRequiredExtensions(uint32_t *count) {
	uint32_t SDL3ExtensionCount = 0;
	const char** SDL3Extensions;

	SDL3Extensions = SDL_Vulkan_GetInstanceExtensions(count);
	if (enableValidationLayers) {
		const char **extensions = (const char**)malloc(sizeof(SDL3Extensions) + sizeof(SDL3Extensions[0]));
		memcpy(extensions, SDL3Extensions, sizeof(SDL3Extensions) + sizeof(SDL3Extensions[0]));
		extensions[SDL3ExtensionCount+1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;

		for (uint32_t i = 0; i < SDL3ExtensionCount; i++) {
			printf("\EXT: %s\n", extensions[SDL3ExtensionCount]);
		}

		return extensions;
	}
	else {
		return SDL3Extensions;
	}

}

uint32_t checkValidationLayerSupport() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, NULL);

	VkLayerProperties *availableLayers = (VkLayerProperties*)malloc(layerCount * sizeof(VkLayerProperties));
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

	uint32_t layerFound = 0;

	for (uint32_t i = 0; i < layerCount; i++) {

		for (uint32_t j = 0; j < sizeof(validationLayers) / sizeof(validationLayers[0]); j++) {
			if (strcmp(availableLayers[i].layerName, validationLayers[j]) == 0) {
				layerFound = 1;
				break;
			}

		}


	}

	if (!layerFound) {
		return 0;
	}

	return 1;
}


/* Function that creates the Vulkan instance. Must be called after
loading Vulkan via SDL3.*/
VkResult Vk_Init() {
	if (enableValidationLayers) {
		SDL_Log("=========Debug mode=========");
	}

	if (enableValidationLayers && !checkValidationLayerSupport()) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Validation layers requested, but not available.");
		return VK_ERROR_LAYER_NOT_PRESENT;
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
		createInfo.enabledLayerCount = sizeof(validationLayers)/sizeof(validationLayers[0]);
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
		return VK_ERROR_INITIALIZATION_FAILED;
	}
	else {
		SDL_Log("Vulkan instance successfully created.");
	}

	setupDebugMessenger();

	// Skipped implementing the VK_ERROR_INCOMPATIBLE_DRIVER workaround. Dont know if it's necessary, sorry mac.

	// Getting # of supported extensions and their names
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);

	SDL_Log("%d extensions supported.", extensionCount);

	VkExtensionProperties  *extensions = (VkExtensionProperties*)malloc(extensionCount * sizeof(VkExtensionProperties));

	vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, extensions);

	for (uint32_t i = 0; i < extensionCount; i++) {
		SDL_Log("\t%s - v%d", extensions[i].extensionName, extensions[i].specVersion);
	}

	SDL_Log("Vulkan initialized.");
	
	free(extensions);
	return VK_SUCCESS;
}

// This function runs once on startup
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

// This function runs when a new event (mouse input, keypress, etc) occurs.
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {

	if (event->key.scancode == SDL_SCANCODE_ESCAPE) {
		return SDL_APP_SUCCESS;
	}

	if (event->type == SDL_EVENT_QUIT) {
		return SDL_APP_SUCCESS; // Ends the program, reports a success to the OS. "Program ran successfully"
	}

	return SDL_APP_CONTINUE; // Carry on with the program
}

// This function runs once per frame. It is the beating heart behind the program
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

// This function runs once when the program is closed.
void SDL_AppQuit(void* appstate, SDL_AppResult result) {
	// SDL cleans up the window/renderer for us
	SDL_Log("Application terminating.");

	if (enableValidationLayers) {
	//	DestroyDebugUtilsMessengerEXT(instance, debugMessenger, NULL);
	}

	//vkDestroyInstance(instance, NULL);
}