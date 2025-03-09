#include "ValidationLayers.h"


/*
Gets the vkDestroyDebugUtilsMessengerEXT function
extension and uses it to desroy / clean up the given instance's debug messenger
*/
void DestroyDebugUtilsMessengerEXT(VkInstance vinstance,
	VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {

	PFN_vkDestroyDebugUtilsMessengerEXT func =
		(PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vinstance,
			"vkDestroyDebugUtilsMessengerEXT");

	if (func != NULL) {
		func(vinstance, debugMessenger, pAllocator);
	}

}

/*
Loads the CreateDebugUtilMessengerEXT function extension and uses it to
create a debug messenger for the given instance
*/
VkResult CreateDebugUtilMessengerEXT(VkInstance vinstance,
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator,
	VkDebugUtilsMessengerEXT* pDebugMessenger) {

	PFN_vkDestroyDebugUtilsMessengerEXT func =
		(PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vinstance, "vkDestroyDebugUtilsMessengerEXT");

	if (func != NULL) {
		func(vinstance, *pDebugMessenger, pAllocator);
		return VK_SUCCESS;
	}
	return VK_ERROR_EXTENSION_NOT_PRESENT;
}

/*
Debug callback. Is called and provided with pCallbackData when a validation layer message is sent.
Decides to output the message if its verbosity level is greater than or equal to the requested debug
verbosity.
*/
VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {

	if (messageSeverity >= 256) {
		printf("\nvalidation layer: %s", pCallbackData->pMessage);
	}

	return VK_FALSE;
}

/*
If validation layers are enabled, initalizes a VkDebugUtilsMessengerCreateInfoEXT struct with
populateDebugMessengerCreateInfo call, then creates a debug messenger with a CreateDebugUtilMessengerEXT
*/
void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* createInfo) {
	memset(createInfo, 0, sizeof(VkDebugUtilsMessengerCreateInfoEXT));
	createInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;  // VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
	//VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo->pfnUserCallback = debugCallback;
	createInfo->pUserData = NULL;
}


/*
If validation layers are enabled, initalizes a VkDebugUtilsMessengerCreateInfoEXT struct with
populateDebugMessengerCreateInfo call, then creates a debug messenger with a CreateDebugUtilMessengerEXT
*/
void setupDebugMessenger(VkInstance vinstance, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo = { 0 };
	populateDebugMessengerCreateInfo(&createInfo);

	if (CreateDebugUtilMessengerEXT(vinstance, &createInfo, NULL, pDebugMessenger) != VK_SUCCESS) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to set up debug messenger.");
	}

}

/*
	Checks the amount of available layers and sees if any of them
	match the defined validation layers for the application.
	TODO: This function is poorly designed and only supports having a single valiaiton layer requested. Fix.
	Ignore all outputs from this function.
*/
uint32_t checkValidationLayerSupport() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, NULL);


	VkLayerProperties* availableLayers = (VkLayerProperties*)malloc(layerCount * sizeof(VkLayerProperties));
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

	if (availableLayers == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to allocate memory for available layers");
		return NULL;
	}

	SDL_Log("%d available validation layers.", layerCount);


	for (uint32_t i = 0; i < layerCount; i++) {
		SDL_Log("\t%s", availableLayers[i].layerName);
	}

	uint32_t layerFound = 0;

	for (uint32_t i = 0; i < layerCount; i++) {
		//	SDL_Log("Finding validation layer %d...", i);
					//validation layers had to be dereferenced for this to work for some reason...
		for (uint32_t j = 0; j < numValidationLayers; j++) {

			if (strcmp(availableLayers[i].layerName, validationLayers[j]) == 0) {
				layerFound = 1;
				break;
			}

		}

		//if (layerFound) {
		//	SDL_Log("Validation layer found");
		//}
		//else {
		//	SDL_Log("Failed to find validation layer");
		//}
	}
	if (!layerFound) {
		return 0;
	}

	return 1;

}