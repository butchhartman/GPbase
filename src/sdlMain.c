// SDL Code provided by :
// https://github.com/libsdl-org/SDL/blob/main/examples/renderer/01-clear/clear.c

// Much of the Vulkan code was provided by or adapted from :
// https://vulkan-tutorial.com/

 #define DEBUG_MODE
 #define DEBUG_VERBOSITY_WARNING

/*
* Through testing I have determined that the validation layers are causing memory leaks. 
* No idea why but at least the problems isnt serious since they can be disabled.
*/

#include <GPbase.h>

// TODO : Use libshaderc to compile shaders instead of batch file

// renderer is used to draw into the window every frame.
static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;

static VkInstance instance;
VkDebugUtilsMessengerEXT debugMessenger;

// This is the graphics card that will be rendered with
VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

// Logical device that interfaces with the physical device
VkDevice logicalDevice;

// Handle to interface with the logicalDevice's auto-generated queue.
VkQueue graphicsQueue;

// The surface to present rendered images to.
VkSurfaceKHR surface;

// Handle to the presentQueue.
VkQueue presentQueue;

// Handle to the swapchain
VkSwapchainKHR swapChain;

// Array of swapchain images
VkImage *swapChainImages;
uint32_t swapChainImagesLength;
// In use format of the swapchain
VkFormat swapChainImageFormat;
// In use extent of the swapchain
VkExtent2D swapChainExtent;

// Array of the image views on the swap chain
//An image view is quite literally a view into an image. 
// It describes how to access the image and which part of 
// the image to access, for example if it should be treated
// as a 2D texture depth texture without any mipmapping levels (vulkan-tutorial.com).
VkImageView *swapChainImageViews;
uint32_t swapChainImageViewsLength;
// Reference to render pass
VkRenderPass renderPass;

VkDescriptorSetLayout descriptorSetLayout;
// Reference to graphics pipeline layout
VkPipelineLayout pipelineLayout;

// Reference to the graphics pipeline itself
VkPipeline graphicsPipeline;

//  Array of framebuffers for all images on the swapchain
VkFramebuffer *swapChainFramebuffers;
uint32_t swapChainFrameBuffersLength;

// Reference to the command pool
VkCommandPool commandPool;

// Arrays of multiple command buffers / semaphores/ fences
VkCommandBuffer *commandBuffers;

VkSemaphore *imageAvailableSemaphores;
VkSemaphore *renderFinishedSemaphores;
VkFence *inFlightFences;

uint32_t currentFrame = 0;

int frameBufferResized = 0;

// Reference to the vertex buffer
VkBuffer vertexBuffer;
VkDeviceMemory vertexBufferMemory;

VkBuffer indexBuffer;
VkDeviceMemory indexBufferMemory;

VkBuffer *uniformBuffers;
VkDeviceMemory *uniformBuffersMemory;
void **uniformBuffersMapped;

VkDescriptorPool descriptorPool;
VkDescriptorSet *descriptorSets;

VkPipelineStageFlags sourceStage;
VkPipelineStageFlags destinationStage;

// Depth buffer components
VkImage depthImage;
VkDeviceMemory depthImageMemory;
VkImageView depthImageView;

/*
* When moving funcs to separate headers : 
* Document how to use the function in the header file, or more accurately close to the declaration.
*
* Document how the function works (if it's not obvious from the code) in the source file, or more accurately, close to the definition.
*/
// TODO : Fix the memory leak

/*
* *********************************************************************************************************************
*	When passed spv bytecode and the size of that code, creates and returns a VkShaderModule.
* ********************************************************************************************************************
* @param code - an unsigned char array containing the raw spv bytecode
* @param size - a uint32_t representing the length of the code array.
* 
* @return VkShaderModule
*/
/*
* *********************************************************************************************************************
* This function returns a VkExtent2D describing the window width and height
* *********************************************************************************************************************
* @param capabilities - Currently has no function
* @return VkExtent2D - Populated with window width and window height
*/

/*
* *********************************************************************************************************************
*	Returns an available swap present mode
* *********************************************************************************************************************
* 
* @param availablePresentModes - Currently has no function
* @return VkPresentModeKHR - Currently only returns mode FIFO
* 
*/
/*
* 
* *********************************************************************************************************************
*	Returns a surface format from the passed available formats that matches the selection parameters
* *********************************************************************************************************************
* 
* @param availableFormats - An array of the available surface formats
* @param length - A uint32_t representing the length of the available format array
* 
* @return VkSurfaceFormatKHR - An available format that matches the selection parameters
* 
*/
/*
* *********************************************************************************************************************
*	Populates a SwapChainSupportDetails struct with the physical device's surface capabilities, the surface formats' properties,
*	and the present modes' properties.
* *********************************************************************************************************************
* 
* @param device - The physical device with which to query information
* @param count - Pointer to a uint32_t which is assigned the length of the formatcount
* 
* @return  SwapChainSupportDetails - The populated detail struct
* 
*/

/*
* *********************************************************************************************************************
*	Enumerates the extensions supported by the physical device and compares it against the extensions required by the application.
* *********************************************************************************************************************
* 
* @param device - The physical device with which to query data
* 
* @return supportedExtensions - The number of supported extensions
*/

/*
* Note : This function gets instance extensions, NOT device extensions
* *********************************************************************************************************************
*	Retrieves the required instance extensions by asking SDL which ones are needed.
*	If validation layers are enabled, includes VK_EXT_debug_utils in the required extensions array.
* *********************************************************************************************************************
* 
* @param count - A uint32_t which is assigned with the length of the required extensions array.
* 
* @return extensions - A const char** which contains the names of the required extensions
*/


/*
* *********************************************************************************************************************
*	Populates a QueueFamilyIndices struct with the indices of a graphics queue family and a present queue family who
*	both meet the necessary requirements.
* *********************************************************************************************************************
* 
* @param device - The phyiscal device with which to query information
* 
* @return QueueFamilyIndicies - A struct containing the graphicsFamily and presentFamily indices
*/

/*
* *********************************************************************************************************************
*	Determines if the physical device supports the queueFamilies, extensions, and swapchain requirements.
* *********************************************************************************************************************
* 
* @params device - The physical device with which to query information
* 
* @return uint32_t - Returns 1 if physical device is suitable, 0 if not
* 
*/

void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
	VkCommandBufferBeginInfo beginInfo = { 0 };
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = NULL;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to begin recording command buffer");
		exit(VK_ERROR_INITIALIZATION_FAILED);
	}

	VkRenderPassBeginInfo renderPassInfo = { 0 };
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];

	//renderPassInfo.renderArea.offset = { 0,0 }; should be zero by default
	renderPassInfo.renderArea.extent = swapChainExtent;

	VkClearValue clearValues[2];
	VkClearColorValue clearcol = {0.0f, 0.0f, 0.0f, 1.0f};
	VkClearDepthStencilValue clrDepthStcl = {1.0f, 0.0f};
	clearValues[0].color = clearcol; 
	clearValues[1].depthStencil = clrDepthStcl;

	renderPassInfo.clearValueCount = 2; // This should be the length of the clear values array
	renderPassInfo.pClearValues = clearValues;

	// No error handling because all recording commands (vkCmd) return void
	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	
	// Bind the graphics pipeline
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

	// I set the viewport and scissor state for this pipeline to be dynamic, so they must be specified before issuing a draw command
	VkViewport viewport = { 0 };
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapChainExtent.width;
	viewport.height = (float)swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor = { 0 };
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent = swapChainExtent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	VkBuffer vertexBuffers[] = {vertexBuffer};
	VkDeviceSize offsets[] = {0};
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);

	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, NULL);

	// the size should be equal to the amount of indices (length) of array
	vkCmdDrawIndexed(commandBuffer,  sizeof(indices) / sizeof(indices[0]), 1, 0, 0 ,0);	

	vkCmdEndRenderPass(commandBuffer);

	// Now error checking can happen
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to record command buffer");
		exit(VK_ERROR_UNKNOWN);
	}
}

/*
* A vertex binding describes at which rate to load data from memory throughout the vertices.
* It specifies the number of bytes between data entries and whether to move to the next data entry after each vertex or each instance.
*
* This function returns a vertexBindingDescription
* 
*/



/*
*	A physcial device in Vulkan is a representation of the host machine's physical graphics card.
*	One can pick as many physical devices as are available and use them simultaneously.
*	However, suitability checks must be performed to make sure the physical device can support the required Vulkan features.
*	
*	Additionally, Vulkan required nearly all commands to be submitted to a queue and it is required to
*	check which queue families are supported by the physical device and if those families support the required features.
* 
*	There are different types of queues that come from queue families. These families are specialized and
*	only allow a certain subset of commands (graphics, etc).
* 
*********************************************************************************************************************** 
*	This function queries and enumerates available physical devices and checks their suitability by calling isDeviceSuitable.
*	As previously stated
***********************************************************************************************************************
*/



/*
*	A Vulkan instance is the link between one's application and the vulkan library.
*	
*********************************************************************************************************************** 
*	This function creates a vulkan instance with all the required extensions specified. Some extensions are required and their names provided by SDL
*	Additionally, if debug mode is enabled, provides the instance with the required validation layers specified.
***********************************************************************************************************************
*/

/*
*	A logical device represents an instance of the physical device's Vulkan implementation.
*	The logical device allows one to interface with the physical device.
* 
* **********************************************************************************************************************
*	This function gets the queue families from the physical devicve and creates an array of (2 in this inflexible case)
*	VKDevicequeueCreateInfo types. It then passes that info into the logical device's createInfo. Creates the logical device.
* **********************************************************************************************************************
*/

/*
*	Vulkan does not care what platform the user is on. It cannot interface with a platform's window system on its own.
*	To connect Vulkan and the window system, one must create a window surface using the platform-specific WSI extensions.
*	These aforementioned extensions are typically retrieved from the window library in use. SDL in this case.
* 
**********************************************************************************************************************
*	This function creates the surface via SDL's implementation of the create surface function.
**********************************************************************************************************************
*/
/*
*	The swap chain provides the ability to present rendering results to a surface via an array of images.
*	It is essentially a queue of images waiting to be rendered.
* 
* *********************************************************************************************************************
*	This function queries swap chain data from the physical device and uses it to create the swap chain.
*	It also retrieves the queue families and sets the image sharing mode accordingly.
* *********************************************************************************************************************
*/

/*
*	An image view describes how to access an image and which part of that image to access.
*	For example, an image view is what tells Vulkan that an accessed image should be treated as a 2D texture depth texture.
* 
* *********************************************************************************************************************
*	This function creates an array of image views that is the same length as the swapChainImages array.
*	In other words, creates one image view for each swap chain image
* *********************************************************************************************************************
*/

/*
*	The graphics pipeline is a sequence of operaions that take the vertices & textures of one's meshes
*	all the way to to the pixels in the render targets.
* 
*	The minimum stage programming required is the vertex and fragment shader, as shown here.
* 
* *********************************************************************************************************************
*	This function loads the fragment and vertex shaders, along with other info structs (color blending, multisampling, type of geometry to draw, etc)
*	and uses it to create the graphics pipeline.
* *********************************************************************************************************************
*
*/
/*
*	A render pass is an object that specifies information about the framebuffer attachments that will be used during rendering
*	For example, how many color and depth buffers there will be, how many samples to use for them, and how their contents should be handled
*	throughout rendering operations.
* 
* *********************************************************************************************************************
*	This function creates colorAttachment, colorAttachmentRef, subpass, and renderPass info.
*	The created info is then used to create a render pass.
* *********************************************************************************************************************
*/

/*
*	A framebuffer object references all of the image views which represent all the framebuffer attachments. However, the attachment used depends on
*	which image the swapchain returns when one is retrieved.
* 
*	Therefore, there must be one framebuffer for each image on the swapchain. The framebuffer corresponding to the
*	retrieve image is the one that will be used. In other words, there must be one framebuffer that accesses images
*	in the way described by the image views for each image on the swapchain.
* 
* *********************************************************************************************************************
*	This function copies the attachments specified by the swapChainImageViews and creates a framebuffer for each image 
*	on the swapchain (which is the same as the ).
* *********************************************************************************************************************
*
*/

/*
*	Commands in Vulkan are not directly executed, they are instead recorded into a command buffer and
*	submitted together.
* 
*	Commands pools manage the memory that is used to store command buffers.
* 
* *********************************************************************************************************************
*	This function creates a command pool for the graphics queue family.
* *********************************************************************************************************************
*
*/
/*
*	Command buffers store Vulkan rendering commands so they can be submitted all at once from the same place.
* 
* *********************************************************************************************************************
*	This function uses the command pool to create a command buffer
* *********************************************************************************************************************
*
*/
/*
*	Vulkan leaves it to the user to handle synchronization. This is done by using fences and semaphores
* 
*	Semaphores - Either signaled or unsignaled, can be used by the GPU to perform commands sequentially
* 
*	Fences - Either signaled or unsignaled, can be used by the CPU to wait for a GPU command to finish.
* 
* *********************************************************************************************************************
*	This function creates a semaphore and a fence.
* *********************************************************************************************************************
*
*/





// Taken from the texture mapping chapter on images. Textures are not currently implemented but this function is necessary for depth testing

// Also part of the texture mapping chapter, but needed for depth buffering

// Another texture mapping function needed for depth testing.. ts pmo


// Another texture mapping function needed for depth buffering






/*
* *********************************************************************************************************************
*	Waits for the global inFlightFence fence to signal, then draws frame to screen by pulling an image from the swap chain and submitting it to the queue.
*	Also creates two semaphores to wait for the image to be available and to wait for the render to finish.
* *********************************************************************************************************************
*/
void drawFrame() {
		
	// semaphores - gpu
	// fences - cpu
	vkWaitForFences(logicalDevice, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;

	VkResult imageAcquireResult = vkAcquireNextImageKHR(logicalDevice, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
	
	if (imageAcquireResult == VK_ERROR_OUT_OF_DATE_KHR) {
		swapchain_recreateSwapChain(&swapChain, &swapChainImages,
									 &swapChainImagesLength, &swapChainImageFormat,
									 &swapChainExtent, logicalDevice,
									 physicalDevice, surface,
									 window, &swapChainFramebuffers,
									 &swapChainFrameBuffersLength, &swapChainImageViews,
									 &swapChainImageViewsLength, renderPass,
									 &depthImageView, &depthImage,
									 &depthImageMemory, &sourceStage,
									 &destinationStage, commandPool,
									 graphicsQueue);
		return;
	}
	else if (imageAcquireResult != VK_SUCCESS && imageAcquireResult != VK_SUBOPTIMAL_KHR) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to acquire swap chain image");
		// Does not exit because VK_SUBOPTIMAL_KHR is considered a success return code.
	}
	// Only reset the fence if it is known that work will be submitted
	vkResetFences(logicalDevice, 1, &inFlightFences[currentFrame]);

	vkResetCommandBuffer(commandBuffers[currentFrame], 0);
	
	recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

	instance_updateUniformBuffer(currentFrame, swapChainExtent,
								 uniformBuffersMapped);

	VkSubmitInfo submitInfo = { 0 };
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame]};
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to submit draw command buffer");
		exit(VK_ERROR_UNKNOWN); 
	}

	VkPresentInfoKHR presentInfo = { 0 };
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = NULL;

	VkResult queuePresentResult = vkQueuePresentKHR(presentQueue, &presentInfo);

	if (queuePresentResult == VK_ERROR_OUT_OF_DATE_KHR || queuePresentResult == VK_SUBOPTIMAL_KHR || frameBufferResized) {
		frameBufferResized = 0;
		swapchain_recreateSwapChain(&swapChain, &swapChainImages,
									 &swapChainImagesLength, &swapChainImageFormat,
									 &swapChainExtent, logicalDevice,
									 physicalDevice, surface,
									 window, &swapChainFramebuffers,
									 &swapChainFrameBuffersLength, &swapChainImageViews,
									 &swapChainImageViewsLength, renderPass,
									 &depthImageView, &depthImage,
									 &depthImageMemory, &sourceStage, 
									 &destinationStage, commandPool,
									 graphicsQueue);
	}
	else if (queuePresentResult != VK_SUCCESS) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to present swap chain image");
	}

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

// TODO: Fix potential memory loadk when resizing the window

/* 
	Function that initializes all the structures needed prior to
	Vulkan outputting to the screen.
*/
VkResult Vk_Init() {
	instance_createInstance(&instance);

	setupDebugMessenger(instance, &debugMessenger);
	
	// Must be called right after create instance because it can influence physical device selection
	instance_createSurface(window, instance, 
						   &surface);

	device_pickPhysicalDevice(instance, &physicalDevice, surface);

	device_createLogicalDevice(physicalDevice, &logicalDevice, &graphicsQueue, &presentQueue, surface);

	swapchain_createSwapChain(&swapChain, &swapChainImages,
							  &swapChainImagesLength, &swapChainImageFormat,
							  &swapChainExtent, logicalDevice,
							  physicalDevice, surface,
							  window);

	swapchain_createImageViews(&swapChainImageViews, swapChainImagesLength,
							   &swapChainImageViewsLength, swapChainImages,
							   swapChainImageFormat, logicalDevice);

	instance_createRenderPass(&renderPass, swapChainImageFormat,
							  physicalDevice, logicalDevice);

	descriptors_createDescriptorSetLayout(&descriptorSetLayout, logicalDevice);

	instance_createGraphicsPipeline(&pipelineLayout, &graphicsPipeline,
									&descriptorSetLayout, renderPass,
									swapChainExtent, logicalDevice);

	instance_createCommandPool(&commandPool, surface,
							   physicalDevice, logicalDevice);

	swapchain_createDepthResources(swapChainExtent, &depthImage,
								   &depthImageView, &depthImageMemory,
								   physicalDevice, logicalDevice,
								   &sourceStage, &destinationStage,
								   commandPool, graphicsQueue);

	buffers_createFramebuffers(&swapChainFramebuffers, &swapChainFrameBuffersLength,
						swapChainImageViews, swapChainImageViewsLength,
						swapChainImagesLength, renderPass,
						swapChainExtent, logicalDevice,
						depthImageView);

	buffers_createVertexBuffer(vertices, logicalDevice, 
							   &vertexBuffer, &vertexBufferMemory, 
							   commandPool, graphicsQueue,
							   physicalDevice);

	buffers_createIndexBuffer(indices, logicalDevice, 
							  &indexBuffer, &indexBufferMemory, 
							  commandPool, graphicsQueue,
							  physicalDevice);

	buffers_createUniformBuffers(&uniformBuffers, &uniformBuffersMemory, 
								 &uniformBuffersMapped, logicalDevice,
								 physicalDevice);

	descriptors_createDescriptorPool(&descriptorPool, logicalDevice);

	descriptors_createDescriptorSets(&descriptorSets, descriptorPool, 
									 descriptorSetLayout, logicalDevice, 
									 uniformBuffers);

	buffers_createCommandBuffers(&commandBuffers, commandPool, 
								 logicalDevice);

	instance_createSyncObjects(&imageAvailableSemaphores, &renderFinishedSemaphores,
							   &inFlightFences, logicalDevice);

	return VK_SUCCESS;
}



/*
	Initialization callback ran by SDL when the application starts.
*/
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {

	SDL_SetAppMetadata("Example Renderer Clear", "1.0", "com.example.renderer-clear");
	
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}
	
	// Loads Vulkan by passing SDL_WINDOW_VULKAN as a window flag
	window = SDL_CreateWindow("examples/renderer/clear", DEFAULT_WIDTH, DEFAULT_HEIGHT, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
	if (window == NULL) {
		SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}
	
	SDL_SetWindowMinimumSize(window, 1, 1);

	if (Vk_Init() != VK_SUCCESS) {
		return SDL_APP_FAILURE;
	}

	return SDL_APP_CONTINUE; // The init function ran successfully
}

/*
	Function that is called by SDL whenever an input event is proc'd
*/
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {

	if (event->key.scancode == SDL_SCANCODE_ESCAPE) {
		return SDL_APP_SUCCESS;
	}

	if (event->type == SDL_EVENT_QUIT) {
		return SDL_APP_SUCCESS; // Ends the program, reports a success to the OS. "Program ran successfully"
	}

	if (event->type == SDL_EVENT_WINDOW_RESIZED) {
		frameBufferResized = 1;
	}

	return SDL_APP_CONTINUE; // Carry on with the program
}

/*
	Function ran once per frame by SDL. Similar to the Update() loop of game engines.
*/
SDL_AppResult SDL_AppIterate(void* appstate) {

	drawFrame();

	return SDL_APP_CONTINUE; // Carry on with the program
}

/* 
	Function ran by SDL when the application closes. Stores cleanup procedures
*/
void SDL_AppQuit(void* appstate, SDL_AppResult result) {
	// SDL cleans up the window/renderer for us
	SDL_Log("Application terminating.");
	vkDeviceWaitIdle(logicalDevice);
	/*
		I could never get the validation layers to proc an error when removing either
		the destruction of the validation layer or the destruction of the instance....
		However, it did proc in other situations so I can only assume the validation layers work.
	*/
	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, NULL);
	}
	// the children yearn for for loops

	swapchain_cleanupSwapChain(swapChainImageViewsLength, logicalDevice,
					 &swapChainImageViews, swapChainFrameBuffersLength,
					 &swapChainFramebuffers, swapChain,
					 depthImageView, depthImage,
					 depthImageMemory);

	vkDestroyDescriptorPool(logicalDevice, descriptorPool, NULL);

	vkDestroyDescriptorSetLayout(logicalDevice, descriptorSetLayout, NULL);

	vkDestroyBuffer(logicalDevice, indexBuffer, NULL);
	vkFreeMemory(logicalDevice, indexBufferMemory, NULL);

	vkDestroyBuffer(logicalDevice, vertexBuffer, NULL);
	vkFreeMemory(logicalDevice, vertexBufferMemory, NULL);
	
	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(logicalDevice, imageAvailableSemaphores[i], NULL);
		vkDestroySemaphore(logicalDevice, renderFinishedSemaphores[i], NULL);
		vkDestroyFence(logicalDevice, inFlightFences[i], NULL);
		
		vkDestroyBuffer(logicalDevice, uniformBuffers[i], NULL);
		vkFreeMemory(logicalDevice, uniformBuffersMemory[i], NULL);
	}

	free(imageAvailableSemaphores);
	free(renderFinishedSemaphores);
	free(inFlightFences);

	free(swapChainImages);

	free(commandBuffers);
	vkDestroyCommandPool(logicalDevice, commandPool, NULL);
	vkDestroyPipeline(logicalDevice, graphicsPipeline, NULL);
	vkDestroyPipelineLayout(logicalDevice, pipelineLayout, NULL);
	vkDestroyRenderPass(logicalDevice, renderPass, NULL);
	vkDestroyDevice(logicalDevice, NULL);
	vkDestroySurfaceKHR(instance, surface, NULL);
	vkDestroyInstance(instance, NULL);
	SDL_Quit();
	//_CrtDumpMemoryLeaks();
}