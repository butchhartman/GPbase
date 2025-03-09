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