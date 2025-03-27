#include "Descriptors.h"


void descriptors_createDescriptorSetLayout(VkDescriptorSetLayout *descriptorSetLayout, VkDevice logicalDevice) {
	VkDescriptorSetLayoutBinding uboLayoutBinding = {0};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = NULL;

	VkDescriptorSetLayoutCreateInfo layoutInfo = {0};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;

	if (vkCreateDescriptorSetLayout(logicalDevice, &layoutInfo, NULL, descriptorSetLayout) != VK_SUCCESS) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create descriptor set layout");
	}
}

void descriptors_createDescriptorPool(VkDescriptorPool *descriptorPool, VkDevice logicalDevice) {
	VkDescriptorPoolSize poolSize = {0};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = (uint32_t)MAX_FRAMES_IN_FLIGHT;

	VkDescriptorPoolCreateInfo poolInfo = {0};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = (uint32_t)MAX_FRAMES_IN_FLIGHT;

	if (vkCreateDescriptorPool(logicalDevice, &poolInfo, NULL, descriptorPool) != VK_SUCCESS) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create descriptor pool");
	}
}

void descriptors_createDescriptorSets(VkDescriptorSet **descriptorSets, VkDescriptorPool descriptorPool, 
                        VkDescriptorSetLayout descriptorSetLayout, VkDevice logicalDevice, 
                        VkBuffer *uniformBuffers) {
	VkDescriptorSetLayout *layouts = malloc(MAX_FRAMES_IN_FLIGHT * sizeof(VkDescriptorSetLayout));
	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		layouts[i] = descriptorSetLayout;
	}

	VkDescriptorSetAllocateInfo allocInfo = {0};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = (uint32_t)MAX_FRAMES_IN_FLIGHT;
	allocInfo.pSetLayouts = layouts;

	*descriptorSets = malloc(sizeof(VkDescriptorSet) * MAX_FRAMES_IN_FLIGHT);
	if (vkAllocateDescriptorSets(logicalDevice, &allocInfo, *descriptorSets) != VK_SUCCESS) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to allocate descriptor sets");
	}

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		VkDescriptorBufferInfo bufferInfo = {0};
		bufferInfo.buffer = uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkWriteDescriptorSet descriptorWrite = {0};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = (*descriptorSets)[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;
		descriptorWrite.pImageInfo = NULL;
		descriptorWrite.pTexelBufferView = NULL;

		vkUpdateDescriptorSets(logicalDevice, 1, &descriptorWrite, 0, NULL);
	}
}