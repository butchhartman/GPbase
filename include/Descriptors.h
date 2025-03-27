#ifndef GP_DESCRIPTORS_INCLUDED_
#define GP_DESCRIPTORS_INCLUDED_

#include "vulkan/vulkan.h"
#include "SDL.h"
#include "Globals.h"

/******************************************************************************//**
*
* A resource descriptor is a way for shaders to freely access resources like buffers and images
* 
* Descriptor Set Layout - Specifies the types of resources that are going to be accessed by the pipeline
* Descriptor Set - Specifies the buffer or image resources that will be bound to the descriptors
* Descriptor Pool - Like a command pool in that it handles the memory for descriptor sets; descriptor sets are allocated from it
*
* (https://vulkan-tutorial.com/Uniform_buffers/Descriptor_layout_and_buffer#page_Descriptor-set-layout)
*
* In this current implementation, only uniform buffers are specified by the descriptor pool and set
* The descriptor sets are specified to expect my UniformBufferObject struct which contains the model, view, and projection matrices
*
***********************************************************************************/

/**
 * \brief Creates a descriptor set layout stored in the passed VkDescriptorSetLayout parameter 
 * 
 * \param descriptorSetLayout Pointer to the variable to which the descriptor set layout will be assigned
 * \param logicalDevice The logical device the descripto set layout will be allocated from
 */
extern void descriptors_createDescriptorSetLayout(VkDescriptorSetLayout *descriptorSetLayout, VkDevice logicalDevice);

/**
 * \brief Creates a descriptor pool stoed in the passed VkDescriptorPool parameter
 * 
 * \param descriptorPool Pointer to the variable to which the descriptor pool will be assigned
 * \param logicalDevice The logical device from which the descriptor pool will be allocated
 */
extern void descriptors_createDescriptorPool(VkDescriptorPool *descriptorPool, VkDevice logicalDevice);

/**
 * \brief Creates an array of descriptor sets stored at the passed pointer's address
 * 
 * \param descriptorSets Address of a pointer where the desciptor sets will be stored
 * \param descriptorPool Descriptor pool from which the descriptor sets will be allocated
 * \param descriptorSetLayout The layout that will be attached to the created descriptor sets
 * \param logicalDevice Logical device from which the descriptor sets will be allocated
 * \param uniformBuffers Array of uniform buffers; descriptor sets will be assigned to their corresponding uniform buffer
 */
extern void descriptors_createDescriptorSets(VkDescriptorSet **descriptorSets, VkDescriptorPool descriptorPool, 
                        VkDescriptorSetLayout descriptorSetLayout, VkDevice logicalDevice, 
                        VkBuffer *uniformBuffers);

#endif