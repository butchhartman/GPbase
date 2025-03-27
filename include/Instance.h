#ifndef GP_INSTANCE_INCLUDED
#define GP_INSTANCE_INCLUDED
#include "vulkan/vulkan.h"
#include "SDL.h"
#include "Globals.h"
#include "ValidationLayers.h"
/******************************************************************************//** 
 *
 * A Vulkan instance is the connection between the application and the Vulkan library
 * 
 * (https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Instance)
 * 
 * TODO: These functions where some of the first I wrote and it shows; code cleanup needed
 *
***********************************************************************************/

/**
 *  \brief Gets the vulkan extensions required by the program
 * 
 * \param count Pointer to a uint32_t that is assigned number of required extensions
 * 
 * \return An array of the names of the required extensions
 */
extern const char **instance_getRequiredExtensions(uint32_t *count);

/**
 * \brief Creates the vulkan instance
 * 
 * \param instance A pointer to a VkInstance which will be assigned the reference to the created instance
 */
extern void instance_createInstance(VkInstance *instance);

#endif