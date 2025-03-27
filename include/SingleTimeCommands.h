#ifndef GP_SINGLETIMECOMMANDS_INCLUDED_
#define GP_SINGLETIMECOMMANDS_INCLUDED_
#include "vulkan/vulkan.h"
#include "SDL.h"
#include "Globals.h"


/******************************************************************************//**
 *
 * March 27, 2025
 * 
 **********************************************************************************/

extern VkCommandBuffer singletimecommands_beginSingleTimeCommands(VkCommandPool commandPool, VkDevice logicalDevice);

extern void singletimecommands_endSingleTimeCommands(VkCommandBuffer commandBuffer, VkCommandPool commandPool,
                                              VkQueue graphicsQueue, VkDevice logicalDevice);

#endif