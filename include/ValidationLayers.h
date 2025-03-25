
#ifndef GP_VALIDATIONLAYERS_INCLUDED_
#define GP_VALIDATIONLAYERS_INCLUDED_

#pragma once

#include <stdio.h>
#include "SDL3/SDL.h"
#include "SDL3/SDL_vulkan.h"
#include <vulkan/vulkan.h>
#include "Globals.h"

/*
Gets the vkDestroyDebugUtilsMessengerEXT function
extension and uses it to desroy / clean up the given instance's debug messenger
*/
void DestroyDebugUtilsMessengerEXT(VkInstance vinstance,
	VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

/*
Loads the CreateDebugUtilMessengerEXT function extension and uses it to
create a debug messenger for the given instance
*/
VkResult CreateDebugUtilMessengerEXT(VkInstance vinstance, 
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
	const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

/*
Debug callback. Is called and provided with pCallbackData when a validation layer message is sent.
Decides to output the message if its verbosity level is greater than or equal to the requested debug
verbosity.
*/
VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
	VkDebugUtilsMessageTypeFlagsEXT messageType, 
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, 
	void* pUserData);

/*
When passed with a createInfo pointer, initializes the data instance that struct.
*/
void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* createInfo);


/*
If validation layers are enabled, initalizes a VkDebugUtilsMessengerCreateInfoEXT struct with
populateDebugMessengerCreateInfo call, then creates a debug messenger with a CreateDebugUtilMessengerEXT
*/
void setupDebugMessenger(VkInstance vinstance, VkDebugUtilsMessengerEXT* pDebugMessenger);


/*
	Checks the amount of available layers and sees if any of them
	match the defined validation layers for the application.
	TODO: This function is poorly designed and only supports having a single valiaiton layer requested. Fix.
	Ignore all outputs from this function.
*/
uint32_t checkValidationLayerSupport();

#endif