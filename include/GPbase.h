// GPbase.h : Include file for standard system include files,
// or project specific include files.

#pragma once
// Ensures GLFW won't include the development environment header. This prevents conflicts upon inclusion of gl3w .
#define SDL_MAIN_USE_CALLBACKS 1 // uses callbacks instead of the main as an entry point

#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 480

#ifndef DEBUG_MODE
const unsigned int enableValidationLayers = 0;
#else
const unsigned int enableValidationLayers = 1;
#endif

const char* validationLayers[] = {
	"VK_LAYER_KHRONOS_validation"
};

#include <stdio.h>
#include <string.h>

// Window creation library. Also handles other things like input.
#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"
#include "SDL3/SDL_vulkan.h"
// C compatible math library for OpenGL
#include "cglm/cglm.h"
#include <vulkan/vulkan.h>
// Custom defines
// Global variables
// Bad practice but useful for this application.



 