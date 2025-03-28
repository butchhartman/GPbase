#include "Instance.h"


void instance_createSyncObjects(VkSemaphore **imageAvailableSemaphores, VkSemaphore **renderFinishedSemaphores,
					   VkFence **inFlightFences, VkDevice logicalDevice) {
	*imageAvailableSemaphores = (VkSemaphore*)malloc(sizeof(VkSemaphore) * MAX_FRAMES_IN_FLIGHT);
	*renderFinishedSemaphores = (VkSemaphore*)malloc(sizeof(VkSemaphore) * MAX_FRAMES_IN_FLIGHT);
	*inFlightFences = (VkFence*)malloc(sizeof(VkFence) * MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo = { 0 };
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = { 0 };
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	if (imageAvailableSemaphores == NULL || renderFinishedSemaphores == NULL || inFlightFences == NULL) {
		return;
	}

	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

		if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, NULL, &((*imageAvailableSemaphores)[i])) != VK_SUCCESS ||
			vkCreateSemaphore(logicalDevice, &semaphoreInfo, NULL, &((*renderFinishedSemaphores)[i])) != VK_SUCCESS ||
			vkCreateFence(logicalDevice, &fenceInfo, NULL, &((*inFlightFences)[i])) != VK_SUCCESS) {
			SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create sync objects");
			exit(VK_ERROR_INITIALIZATION_FAILED);
		}

	}
}

void instance_createCommandPool(VkCommandPool *commandPool, VkSurfaceKHR surface,

					   VkPhysicalDevice physicalDevice, VkDevice logicalDevice) {
	QueueFamilyIndices queueFamilyIndices = device_findQueueFamilies(physicalDevice, surface);

	VkCommandPoolCreateInfo poolInfo = { 0 };
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;

	if (vkCreateCommandPool(logicalDevice, &poolInfo, NULL, commandPool) != VK_SUCCESS) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create command pool");
		exit(VK_ERROR_INITIALIZATION_FAILED);
	}
}

void instance_updateUniformBuffer(uint32_t currentImage, VkExtent2D swapChainExtent,
								  void **uniformBuffersMapped) {
	double time = SDL_GetTicks()/1000.0f;

	UniformBufferObject ubo = {0};
	vec3s z = GLMS_VEC3_ZERO_INIT;
	z.z = 1;
	ubo.model = glms_rotate(GLMS_MAT4_IDENTITY, time * M_PI_2, z);
	vec3s twos;
	twos.x = 2;
	twos.y = 2;
	twos.z = 2;

	ubo.view = glms_lookat(twos, GLMS_VEC3_ZERO, z);
	ubo.proj = glms_perspective(glm_rad(45), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 10.0f);
	ubo.proj.m11 *= -1; // inverts the y axis because vulkan has y clip coordinates inverted compared to opengl

	memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

void instance_createRenderPass(VkRenderPass *renderPass, VkFormat swapChainImageFormat,
							   VkPhysicalDevice physicalDevice, VkDevice logicalDevice) {

	VkAttachmentDescription colorAttachment = { 0 };
	colorAttachment.format = swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	// directly referenced by layout(location = 0) out vec4 outColor in fragment shader
	VkAttachmentReference colorAttachmentRef = { 0 };
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription depthAttachment = {0};
	depthAttachment.format = swapchain_findDepthFormat(physicalDevice);
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef = {0};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = { 0 };
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	VkAttachmentDescription attachments[] = {colorAttachment, depthAttachment};

	VkRenderPassCreateInfo renderPassInfo = { 0 };
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 2; // This should be the length of the attachment array
	renderPassInfo.pAttachments = attachments;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	VkSubpassDependency dependency = { 0 };
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;

	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = 0;

	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(logicalDevice, &renderPassInfo, NULL, renderPass) != VK_SUCCESS) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create render pass");
		exit(VK_ERROR_INITIALIZATION_FAILED);
	}
}

VkVertexInputAttributeDescription *instance_getAttributeDescriptions() {
	VkVertexInputAttributeDescription *attribDescriptions = malloc(sizeof(Vertex) * 2);
	attribDescriptions[0].binding = 0;
	attribDescriptions[0].location = 0;
	attribDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attribDescriptions[0].offset = offsetof(Vertex, pos);
	attribDescriptions[1].binding = 0;
	attribDescriptions[1].location = 1;
	attribDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attribDescriptions[1].offset = offsetof(Vertex, color);
	return attribDescriptions;
}

VkVertexInputBindingDescription instance_getVertexBindingDescription() {
	VkVertexInputBindingDescription bindingDescription = {0};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindingDescription;
}

void instance_createGraphicsPipeline(VkPipelineLayout *pipelineLayout, VkPipeline *graphicsPipeline, 
							VkDescriptorSetLayout *descriptorSetLayout, VkRenderPass renderPass,
							VkExtent2D swapChainExtent, VkDevice logicalDevice) {
	uint32_t vertShaderSize;
	uint32_t fragShaderSize;

	const unsigned char* vertShaderBin = readFileBin("../../shaders/vert.spv", &vertShaderSize);
	const unsigned char* fragShaderBin = readFileBin("../../shaders/frag.spv", &fragShaderSize);

	VkShaderModule vertShaderModule = createShaderModule(vertShaderBin, vertShaderSize, logicalDevice);
	VkShaderModule fragShaderModule = createShaderModule(fragShaderBin, fragShaderSize, logicalDevice);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = { 0 };
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = { 0 };
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = {
		vertShaderStageInfo, fragShaderStageInfo
	};

	// TODO: expand in vertex buffer chapter
	// There are many concerning validation layer warnings here so keep in mind this may not work correctly
	VkVertexInputBindingDescription bindingDescription = instance_getVertexBindingDescription();
	VkVertexInputAttributeDescription *attribDescriptions = instance_getAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = { 0 };
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription; 
	vertexInputInfo.vertexAttributeDescriptionCount = 2; // This is supposed to be equal to the attribDescription array size. I just put two here since it is always two for now.
	vertexInputInfo.pVertexAttributeDescriptions = attribDescriptions;

	// Defines what type of geometry will be drawn from the vertices
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = { 0 };
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	// Describes the region of the framebuffer that the output will be rendered to
	VkViewport viewport = { 0 };
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapChainExtent.width;
	viewport.height = (float)swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkPipelineDynamicStateCreateInfo dynamicState = { 0 };
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = sizeof(dynamicStates)/sizeof(dynamicStates[0]);
	dynamicState.pDynamicStates = dynamicStates;

	// Only must specify the count because they are dynamic and will be setup at run time
	VkPipelineViewportStateCreateInfo viewportState = { 0 };
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizer = { 0 };
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f;
	rasterizer.depthBiasClamp = 0.0f;
	rasterizer.depthBiasSlopeFactor = 0.0f;

	// Disabled for now, requires a GPU feature
	VkPipelineMultisampleStateCreateInfo multisampling = { 0 };
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f;
	multisampling.pSampleMask = NULL;
	multisampling.alphaToCoverageEnable = VK_FALSE;
	multisampling.alphaToOneEnable = VK_FALSE;

	// Depth and/or stencil buffer
	VkPipelineDepthStencilStateCreateInfo depthStencil = {0};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f;
	depthStencil.maxDepthBounds = 1.0f;
	depthStencil.stencilTestEnable = VK_FALSE;
	// depthStencil.front = {0}; // optional
	// depthStencil.back = {}; // optional

	// stuff for blending colors. Currently disabled.
	VkPipelineColorBlendAttachmentState colorBlendAttachment = { 0 };
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT 
		| VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending = { 0 };
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	// Create graphics pipeline layout
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = { 0 };
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = descriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = NULL;

	if (vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, NULL, pipelineLayout) != VK_SUCCESS) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create pipeline layout");
		exit(VK_ERROR_INITIALIZATION_FAILED);
	}

	// FINALLY!! CREATING THE GRAPHICS PIPELINE!!!
	VkGraphicsPipelineCreateInfo pipelineInfo = { 0 };
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;

	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;

	pipelineInfo.layout = *pipelineLayout;

	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = NULL;
	pipelineInfo.basePipelineIndex = -1;


	if (vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, graphicsPipeline) != VK_SUCCESS) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create graphics pipeline");
		exit(VK_ERROR_INITIALIZATION_FAILED);
	}

	// These can be deleted after they are used because the bytecode gets linked and compiled by the GPU.
	vkDestroyShaderModule(logicalDevice, vertShaderModule, NULL);
	vkDestroyShaderModule(logicalDevice, fragShaderModule, NULL);
	free(vertShaderBin);
	free(fragShaderBin);
	free(attribDescriptions);

}

void instance_createSurface(SDL_Window *window, VkInstance instance,
		  				    VkSurfaceKHR *surface) {
	if (SDL_Vulkan_CreateSurface(window, instance, NULL, surface) == 0) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create a window surface.");
		exit(VK_ERROR_INITIALIZATION_FAILED);
	}
	printf("SURF ERR %s", SDL_GetError());

}

const char **instance_getRequiredExtensions(uint32_t *count) {
	/*
	* This const char ** should never be freed because it points to an array owned by SDL.
	* This array should not be freed.
	*/
	const char **SDL3Extensions = SDL_Vulkan_GetInstanceExtensions(count);

	if (enableValidationLayers) {
		const char **extensions = malloc(sizeof(const char*) * (*count+1));

		if (extensions == NULL) {
			SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to allocate memory for extensions");
			return NULL;
		}

		for (uint32_t i = 0; i < *count; i++) {
			extensions[i] = SDL3Extensions[i];
		}
		extensions[*(count)] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;

		for (uint32_t i = 0; i < *(count) + 1; i++) {
			SDL_Log("REQUIRED EXTENSION: %s", extensions[i]);
		}

		*count += 1;
		return extensions;
	}
	else {
		return SDL3Extensions;
	}

}

void instance_createInstance(VkInstance *instance) {
	if (enableValidationLayers) {
		SDL_Log("=========Debug mode=========");
	}

	if (enableValidationLayers && !checkValidationLayerSupport()) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Validation layers requested, but not available.");
		exit(VK_ERROR_LAYER_NOT_PRESENT);
	}


	// Initializing Vulkan
	// Info about the application. Technically optional, but useful
	VkApplicationInfo appinfo = { 0 };
	appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appinfo.pApplicationName = "Hello, Vulkan";
	appinfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0); // Variant is the first variable. Should always be 0
	appinfo.pEngineName = "No Engine";
	appinfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
	appinfo.apiVersion = VK_API_VERSION_1_4;
	appinfo.pNext = NULL;

	// Not optional information. Tells Vulkan driver which extensions and validation layers to use
	VkInstanceCreateInfo createInfo = { 0 };
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appinfo;

	const char** SDL3Extensions;
	uint32_t SDL3ExtensionCount;
	SDL3Extensions = instance_getRequiredExtensions(&SDL3ExtensionCount);

	createInfo.enabledExtensionCount = SDL3ExtensionCount;
	createInfo.ppEnabledExtensionNames = SDL3Extensions;
	createInfo.enabledLayerCount = 0;

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = { 0 };


	// verification layers
	if (enableValidationLayers) {
		SDL_Log("Validation layers active.");
		createInfo.enabledLayerCount = sizeof(validationLayers) / sizeof(validationLayers[0]);
		createInfo.ppEnabledLayerNames = validationLayers;

		populateDebugMessengerCreateInfo(&debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)(&debugCreateInfo);
	}
	else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = NULL;
	}

	VkResult result = vkCreateInstance(&createInfo, NULL, instance);
	if (result != VK_SUCCESS) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create Vulkan instance.");
		exit(VK_ERROR_INITIALIZATION_FAILED);
	}
	else {
		SDL_Log("Vulkan instance successfully created.");
	}

	// Skipped implementing the VK_ERROR_INCOMPATIBLE_DRIVER workaround. Dont know if it's necessary, sorry mac users.

	// Getting # of supported extensions and their names
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);

	SDL_Log("%d extensions supported.", extensionCount);

	VkExtensionProperties* extensions = (VkExtensionProperties*)malloc(extensionCount * sizeof(VkExtensionProperties));

	vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, extensions);

	if (extensions == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to allocate memory for supported extensions");
		return;
	}

	for (uint32_t i = 0; i < extensionCount; i++) {
		SDL_Log("\t%s - v%d", extensions[i].extensionName, extensions[i].specVersion);
	}

	SDL_Log("Vulkan initialized.");

	free(extensions);

	if (enableValidationLayers) {
		/*
		*	This is only freed because when validation layers are active, the SDL internal extensions array
		*	is duplicated so the validation layer extensions can be appended. Therefore, I (instead of SDL)
		*	own the array which can be freed.
		* 
		*	Ts pmo ...
		*/
		free(SDL3Extensions);
	}
}