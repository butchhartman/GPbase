#ifndef GP_GLOBALS_INCLUDED_
#define GP_GLOBALS_INCLUDED_

#pragma once

#define CGLM_FORCE_DEPTH_ZERO_TO_ONE

#include "vulkan/vulkan.h"
#include "cglm/struct.h"

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
	vec3s pos;
	vec3s color;
} Vertex;

typedef struct {
	mat4s model;
	mat4s view;
	mat4s proj;
} UniformBufferObject;


extern const Vertex vertices[8];

extern const uint16_t indices[12];

// Validation layers
extern const char* validationLayers[1];

extern const int numValidationLayers;


// Device extensions needed for program to run properly
extern const char* deviceExtensions[1];

// The dynamic states of the graphics pipeline.
// Essentially what values can by dynamically specified at draw time
extern const VkDynamicState dynamicStates[2];

extern const unsigned int MAX_FRAMES_IN_FLIGHT;

extern const int enableValidationLayers;

/*
* Notes on header files :
* Header file source code simply replaces the #include in files which include them
* Extern keyword means that a variable of the specified type is defined, and the compiler(linker?) 
* will look for its definition in other source (.c) files. 
* 
* So, essentially, all this header file does is tell the compiler(linker?) that, in each file including this header,
* it should look for the definitions of these variables in a separate source file.
*
* For example, in sdlMain.c, the #include<GPbase.h> includes this file at the top. This is me making a promise to the compiler that
* enableValidationLayers is defined in another source file. It then looks for it and finds it in Globals.c, allowing me to access its value in
* sdlMain.c
*
* This is also the reason that normal variables (i guess structs are exempt) cannot be defined in header files. It results in
* multiple files defining these variables (since their definitions replace the #include), which causes compiler errors
*/

#endif