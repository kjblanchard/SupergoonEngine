#pragma once
#include <stdlib.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct SDL_AudioStream SDL_AudioStream;
typedef struct sgStream {
	SDL_AudioStream* stream;
} sgStream;

sgStream* sgStreamNew(void);
bool sgStreamIsFinished(sgStream* stream);
void sgStreamFree(sgStream* stream);
#ifdef __cplusplus
}
#endif
