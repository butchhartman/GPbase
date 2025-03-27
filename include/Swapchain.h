#ifndef GP_SWAPCHAIN_INCLUDED_
#define GP_SWAPCHAIN_INCLUDED_
#include "vulkan/vulkan.h"
#include "SDL.h"
#include "Globals.h"
#include "Device.h"
#include "Buffers.h"
#include "SingleTimeCommands.h"


/******************************************************************************//**
 *
 * March 27, 2025
 * 
 * The swapchain represents a series of (blank) images which the programmer can request from the operating system (display engine?).
 * Once the request to borrow a swapchain image is granted, the program may render to it, then request the operating system to display the image
 * to a display (surface ?). Once that image is presented, it cannot be used again until it is borrowed again
 * 
 * (https://stackoverflow.com/questions/67298942/what-exactly-are-framebuffers-and-swapchains)
 * 
 * The program draws to one swapchain image, then swaps it with the currently displayed one
 * The program then repeats this process, drawing to an image, then swapping the displayed image for the newly drawn one
 * 
 * (https://en.wikipedia.org/wiki/Swap_chain)
 * (https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Swap_chain)
 * 
 * TODO: Research the Vulkan swapchain. There is a lot more detail about the componenets of a swapchain (framebuffer, imageview, image, etc) which I do not fully understand. 
 **********************************************************************************/

 /**
  * 
  * \brief Gets and returns details about the GPU's surface capabilities, surface formats, and surface present modes
  * 
  * \param device The physical device which will have its details queried
  * \param count Pointer to an uint32_t which will sotre the number of supported surface formats
  * \param surface The surface to query details with
  * 
  * \return SwapChainSupportDetails populated with the supported present modes and present formats
  */
extern SwapChainSupportDetails swapchain_querySwapChainSupport(VkPhysicalDevice device, uint32_t *count, 
                                                               VkSurfaceKHR surface);

/**
 * \brief Chooses swap present mode based on the available present modes
 * \note: This function currently always returns FIFO present mode because it is guaranteed to be supported
 * \todo: Add present mode selection logic
 *   
 * \param availablePresentModes Pointer to an array of VkPresentModeKHR types
 * 
 * \return The selected supported present mode
 */                                                            
extern VkPresentModeKHR swapchain_chooseSwapPresentMode(const VkPresentModeKHR **availablePresentModes);

/**
 * \brief Selects a supported surface format from availableFormats
 * \note Currently this function returns the first format which is VK_FORMAT_B8G8R8A8_SRGB with a colorspace of VK_COLOR_SPACE_SRGB_NONLINEAR_KHR; If neither of those conditions are met, the function will return the first format in the availableFormats array
 * 
 * 
 * \param availableFormats Array of VkSurfaceFormatKHR which contains the supported surface formats
 * \param length Length of the passed availableFormats array
 * 
 * \return The surface format which meets the selection criteria
 */
extern VkSurfaceFormatKHR swapchain_chooseSwapSurfaceFormat(const VkSurfaceFormatKHR *availableFormats, uint32_t length);

/**
 * \brief Populates and returns a VkExtent2D struct with the current size of the window
 * 
 * \note This function literally does nothing else but populate the struct. capabilities parameter goes unused
 * 
 * \param capabilities Array of VkSurfaceCapabilitiesKHR
 * \param window The window which will have its size queried 
 * 
 * \return VkExtent2D struct describing the current size of the passed window
 */
extern VkExtent2D swapchain_chooseSwapExtent(const VkSurfaceCapabilitiesKHR* capabilities, SDL_Window *window);

/**
 * \brief Determines and returns a suported VkFormat (available image format)
 * 
 * \note This functions selection parameters are hardcoded to VK_IMAGE_TILING_LINEAR or a fallback of VK_IMAGE_TILING_LINEAR
 * 
 * \param candidates Array of VkFormat containing potentially supported formats
 * \param candidatesLength Length of the candidates array
 * \param tiling The requested tiling mode
 * \param features Bit mask of features which need to be supported
 * \param physicalDevice The physical device with which to query format properties
 * 
 * \return The format which supports the required features 
 */
extern VkFormat swapchain_findSupportedFormat(const VkFormat *candidates, uint32_t candidatesLength, 
                                              VkImageTiling tiling, VkFormatFeatureFlags features, 
                                              VkPhysicalDevice physicalDevice);

/**
 * \note This function is from the texture chapter of vulkan tutorial, I currently do not know what it does; it was added to allow depth buffering
 */
extern void swapchain_transitionImageLayout(VkImage image, VkFormat format, 
									        VkImageLayout oldLayout, VkImageLayout newLayout, 
									        VkPipelineStageFlags *sourceStage, VkPipelineStageFlags *destinationStage,
						       			    VkCommandPool commandPool, VkDevice logicalDevice,
									        VkQueue graphicsQueue);
/**
 * \note This function is from the texture chapter of vulkan tutorial, I currently do not know what it does; it was added to allow depth buffering
 */
extern VkImageView swapchain_createImageView(VkImage image, VkFormat format, 
                                             VkImageAspectFlags aspectFlags, VkDevice logicalDevice);
/**
 * \note This function is from the texture chapter of vulkan tutorial, I currently do not know what it does; it was added to allow depth buffering
 */
extern void swapchain_createImage(uint32_t width, uint32_t height, 
                                  VkFormat format, VkImageTiling tiling, 
                                  VkImageUsageFlags usage, VkMemoryPropertyFlags properties, 
                                  VkImage *image, VkDeviceMemory *imageMemory, 
                                  VkDevice logicalDevice);

/**
 * \brief Calls swapchain_findSupportedFormat and returns the format which supports a stencil buffer
 * \note this description and my understanding of this is tentative at best
 * 
 * \param physicalDevice The physical device which will be passed to swapchain_findSupportedFormat
 */
extern VkFormat swapchain_findDepthFormat(VkPhysicalDevice physicalDevice);

/**
 * \brief Populates the passed depthImage, depthImageView, and depthImageMemory
 * 
 * \todo Research definitions of unknown parameters
 * 
 * \param swapChainExtent Struct describing the current size of the window
 * \param depthImage Pointer to a depth image which will be populated by the function
 * \param depthImageView Pointer to a depth image view which will be populated by the function
 * \param logicalDevice Logical device to be used in creating the depth image view
 * \param sourceStage I do not know what this does as of now
 * \param destinationStage I do not know what this does as of now 
 * \param commandPool Command pool to allocate a temporary command buffer from
 * \param graphicsQueue Graphics queue which will be passed to swapchain_transitionImageLayout
 */
extern void swapchain_createDepthResources(VkExtent2D swapChainExtent, VkImage *depthImage,
      									   VkImageView *depthImageView, VkDeviceMemory *depthImageMemory,
                                           VkPhysicalDevice physicalDevice, VkDevice logicalDevice,
                                           VkPipelineStageFlags *sourceStage, VkPipelineStageFlags *destinationStage,
                                           VkCommandPool commandPool, VkQueue graphicsQueue);

/**
 * \brief Destroys the swap chain and all resources it uses  
 * 
 * \param swapChainImageViewsLength Length of the swapChainImageViews array
 * \param logicalDevice Logical device the swapchain was allocated from
 * \param swapChainImageViews Pointer the array of the swapchain's image views
 * \param swapChainFrameBuffersLength Length of the swapChainFrameBuffers array
 * \param swapChainFrameBuffers Pointer to an array of the swapchain's framebuffer
 * \param swapChain The swap chain to be destroyed
 * \param depthImageView The depth image view to be destroyed
 * \param depthImage The depth image to be destroyed
 * \param depthImageMemory The depth image memory to be destroyed
 */                                           
extern void swapchain_cleanupSwapChain(Uint32 swapChainImageViewsLength, VkDevice logicalDevice,
                                VkImageView **swapChainImageViews, uint32_t swapChainFrameBuffersLength,
                                VkFramebuffer **swapChainFramebuffers, VkSwapchainKHR swapChain,
                                VkImageView depthImageView, VkImage depthImage,
                                VkDeviceMemory depthImageMemory);

/**
 * \brief Allocates and populates an array of swap chain image views
 * 
 * \param swapChainImageViews Pointer to the destination array of the swap chain image views
 * \param swapChainImagesLength Length of the swapChainImages array
 * \param swapChainImageViewsLength Pointer to an uint32_t which will be assigned the length of the created swapChainImageViews array
 * \param swapChainImages Array of swap chain images
 * \param swapChainImageFormat The format of the images in the swapChainImages array
 * \param logicalDevice The logical device the image views are allocated from 
 */
extern void swapchain_createImageViews(VkImageView **swapChainImageViews, uint32_t swapChainImagesLength,
                                       uint32_t *swapChainImageViewsLength, VkImage *swapChainImages,
                                       VkFormat swapChainImageFormat, VkDevice logicalDevice);

/**
 * \note I am aware this function is a mess of parameters. I do not know how else to handle this, though
 * \note Don't cry little function, uncle monger's got enough parameters to feed you your entire life
 * 
 * \brief This function is meant to recreate the swapchain when the window is resized. Due to the amount of parameters I'll only describe the muddier ones
 * 
 * \param swapChainImages Pointer to an array of the swapchain's images. Its value will be set to the new image array
 * \param swapChainFrameBuffers Pointer to an array of the swapchain's framebuffers. Its value will ne set to the new framebuffer array
 * 
 * \todo Document which pointer parameters are modified by the function (it might be all of them lol)
 * 
 * 
 */
extern void swapchain_recreateSwapChain(VkSwapchainKHR *swapChain, VkImage **swapChainImages,
                                      uint32_t *swapChainImagesLength, VkFormat *swapChainImageFormat,
                                      VkExtent2D *swapChainExtent, VkDevice logicalDevice,
                                      VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
                                      SDL_Window *window, VkFramebuffer **swapChainFrameBuffers,
                                      uint32_t *swapChainFrameBuffersLength, VkImageView **swapChainImageViews,
                                      uint32_t *swapChainImageViewsLength, VkRenderPass renderPass, 
                                      VkImageView *depthImageView, VkImage *depthImage,
                                      VkDeviceMemory *depthImageMemory, VkPipelineStageFlags *sourceStage,
                                      VkPipelineStageFlags *destinationStage, VkCommandPool commandPool,
                                      VkQueue graphicsQueue);

/**
 * \brief Creates a swapchain
 * 
 * \param swapChain Pointer to the swapchain variable where the swapchain reference will be stored
 * \param swapChainImages Pointer to an array of VkImage which will be populated with the swap chain images
 * \param swapChainImageLength Pointer to a uin32_t which will be assigned the length of the swap chain images array
 * \param swapChainImageFormat Pointer to a VkFormat which will be assigned with the image format of the swapchain
 * \param swapChainExtent Pointer to a VkExtent2D struct which will be populated with the size of the current window
 * \param logicalDevice The logical device which swap chain resources will be allocated from
 * \param physicalDevice Physical devices which will have its properties queried
 * \param surface The surface that will be rendered to
 * \param window The SDL window that will be queried for its size
 */
extern void swapchain_createSwapChain(VkSwapchainKHR *swapChain, VkImage **swapChainImages,
                                      uint32_t *swapChainImagesLength, VkFormat *swapChainImageFormat,
                                      VkExtent2D *swapChainExtent, VkDevice logicalDevice,
                                      VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
                                      SDL_Window *window);


#endif