#include "Globals.h"

enum deviceSuitableResult { PHYSICAL_DEVICE_NOT_SUITABLE, PHYSICAL_DEVICE_SUITABLE };

const Vertex vertices[] = {
	{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
	{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
	{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}},

	{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
	{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
	{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}}
};

const uint16_t indices[] = {
	0, 1, 2, 2, 3, 0,
	4, 5, 6, 6, 7, 4
};

// Validation layers
const char* validationLayers[] = {
	"VK_LAYER_KHRONOS_validation",
};

const int numValidationLayers = sizeof(validationLayers) / sizeof(validationLayers[0]);


// Device extensions needed for program to run properly
const char* deviceExtensions[] = {
	"VK_KHR_swapchain",
};

// The dynamic states of the graphics pipeline.
// Essentially what values can by dynamically specified at draw time
const VkDynamicState dynamicStates[] = {
	VK_DYNAMIC_STATE_VIEWPORT,
	VK_DYNAMIC_STATE_SCISSOR
};

const unsigned int MAX_FRAMES_IN_FLIGHT = 2;

const int enableValidationLayers = 1;