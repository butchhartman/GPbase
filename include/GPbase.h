// GPbase.h : Include file for standard system include files,
// or project specific include files.

#pragma once
#define SDL_MAIN_USE_CALLBACKS 1 // uses callbacks instead of the main as an entry point

#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 480

#ifndef DEBUG_MODE
	const unsigned int enableValidationLayers = 0;

#else
	const unsigned int enableValidationLayers = 1;

	#ifdef DEBUG_VERBOSITY_ALL // No restruction to debug mode outputs
		#define DEBUG_VERBOSITY 0 
	#endif

	#ifdef DEBUG_VERBOSITY_CRITICAL // Only critical debug mode outpus will be logged
		#define DEBUG_VERBOSITY 4096 
	#endif 

	#ifdef DEBUG_VERBOSITY_WARNING // Only warnings and up will be shown
		#define DEBUG_VERBOSITY	256 //VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
	#endif

#endif

//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>

// Standard libraries
#include <stdio.h>
#include <string.h>

// SDL : Window creation, audio, input, and 2D graphics library
#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"
#include "SDL3/SDL_vulkan.h"

// CGLM : Graphics math library compatible with C
#include "cglm/struct.h"

// 3D graphics API
#include <vulkan/vulkan.h>

// Header containing functions involving validation layers
#include "ValidationLayers.h"

#include "LoadShaders.h"


enum deviceSuitableResult { PHYSICAL_DEVICE_NOT_SUITABLE, PHYSICAL_DEVICE_SUITABLE };

typedef struct {
	uint32_t graphicsFamily;
	uint32_t presentFamily;
} QueueFamilyIndices;

typedef struct {
	VkSurfaceCapabilitiesKHR capabilities;
	VkSurfaceFormatKHR *formats;
	VkPresentModeKHR *presentModes;
} SwapChainSupportDetails;

typedef struct {
	vec2s pos;
	vec3s color;
} Vertex;

const Vertex vertices[] = {
	{{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
	{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
	{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
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
VkDynamicState dynamicStates[] = {
	VK_DYNAMIC_STATE_VIEWPORT,
	VK_DYNAMIC_STATE_SCISSOR
};

const unsigned int MAX_FRAMES_IN_FLIGHT = 2;