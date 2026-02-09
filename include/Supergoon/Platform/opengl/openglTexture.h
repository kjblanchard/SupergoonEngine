#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Texture {
	unsigned int ID;
	unsigned int Width;
	unsigned int Height;
	unsigned int VAO;
	unsigned int FBO;
	int RefCount;
	char* Name;
} Texture;

#ifdef __cplusplus
}
#endif
