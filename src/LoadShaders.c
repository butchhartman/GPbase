#include "LoadShaders.h"


// TODO : Add error checking and documentation
const unsigned char* readFileBin(const char* filename, unsigned int *size) {
	FILE* file_ptr;

	unsigned char *data;

	file_ptr = fopen(filename, "rb");

	if (file_ptr == NULL) {
		printf("\n%s cannot be opened.\n", filename);
		return;
	}
	
	fseek(file_ptr, 0, SEEK_END);
	unsigned int fileSize = ftell(file_ptr);
	if (fileSize <= 0) {
		printf("\nFile has length of zero.\n");
		fclose(file_ptr);
		return;
	}


	printf("\nFile size : %d\n", fileSize);
	fseek(file_ptr, 0, SEEK_SET);

	data = (unsigned char*)malloc(sizeof(unsigned char) * fileSize);

	printf("\nSize of malloc : %d\n", fileSize);

	if (data == NULL) {
		printf("\nFile has no data!\n");
		return NULL;
	}

	fread(data, sizeof(unsigned char), fileSize, file_ptr);

	//for (unsigned int i = 0; i < fileSize; i++) {
	//	printf("%p [%d]", data[i], i);
	//}

	fclose(file_ptr);
	if (data != NULL) {
		printf("\nShader sucessfully loaded from %s\n", filename);
	}
	*size = fileSize;
	return data;
}


VkShaderModule createShaderModule(const unsigned char* code, uint32_t size,
								  VkDevice logicalDevice) {

	VkShaderModuleCreateInfo createInfo = { 0 };
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = size;
	// This is a uint32_t*, but it works with unsigned char*. Typecast is just in case it matters.
	createInfo.pCode = (const uint32_t*)code;

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(logicalDevice, &createInfo, NULL, &shaderModule) != VK_SUCCESS) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create shader module");
		exit(VK_ERROR_INITIALIZATION_FAILED);
	}
	
	return shaderModule;
}