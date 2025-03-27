#ifndef GP_DEVICE_INCLUDED_
#define GP_DEVICE_INCLUDED_

#include "vulkan/vulkan.h"
#include "SDL.h"
#include "Globals.h"
#include "Swapchain.h"


/******************************************************************************//**
 * A physical device is a representation of the host machine's graphics card.
 * A logical device is the abstraction which allows the programmer to interface with and modify the physical device and its resources 
 **********************************************************************************/

 /**
  * \brief Checks the passed physical device's supported extensions against the global deviceExtensions array and returns an int representing the number of the required extensions that are supported
  * 
  * \param device The physical device that will have its supported extensions enumerated
  * 
  * \return The number of supported extensions
  */
extern int device_checkDeviceExtensionSupport(VkPhysicalDevice device);

/**
 * \brief Checks if the passed physical device has the required queue families, supports the required extensions, and swapchain present modes & formats
 * 
 * \param device The physical device that will have its suitability tested
 * \param surface The surface that output will be presented to
 * 
 * \return 1 if true; 0 if false
 */
extern uint32_t device_isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);

/**
 * \brief Enumerates the physical device's queue families and selects the graphics and queue families if available
 * 
 * \param device The physical device that will have its queue families enumerated
 * \param surface The surface which will be presented to
 * 
 * \return Struct containing the indices of the queue and present family
 */
extern QueueFamilyIndices device_findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

/**
 * \brief Picks physical device from list of availble units; currently returns the first suitable device found
 * 
 * \param instance The current Vulkan instance
 * \param physicalDevice Pointer to the VkPhysicalDevice variable which will be assigned with the data of the suitable physical device
 * \param surface Surface which will be presented to
 */
extern void device_pickPhysicalDevice(VkInstance instance, VkPhysicalDevice *physicalDevice, VkSurfaceKHR surface);

/**
 * \brief Creates a logical device to interface with the passed physical device
 * 
 * \param physicalDevice The targeted physical device
 * \param logicalDevice Pointer to the VkDevice variable which will be assinged with the logical device's data
 * \param graphicsQueue Pointer to the VkQueue variable which will be assigned with the graphics queue data
 * \param presentQueue Pointer to the VkQueue variable which will be assigned with the present queue data
 * \param surface The surface which will be presented to
 */
extern void device_createLogicalDevice(VkPhysicalDevice physicalDevice, VkDevice *logicalDevice, VkQueue *graphicsQueue, VkQueue *presentQueue, VkSurfaceKHR surface);

#endif // GP_DEVICE_INCLUDED_
