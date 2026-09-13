#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Texture {
	unsigned int ID;
	int Width;
	int Height;
	unsigned int VAO;
	unsigned int VBO;
	unsigned int FBO;
	// int RefCount;
	char* Name;
} Texture;

#ifdef __cplusplus
}
#endif
