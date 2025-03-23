#pragma once
#include <stdlib.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct SDL_AudioStream SDL_AudioStream;
typedef struct sgStream {
	SDL_AudioStream* stream;
} sgStream;

// Create a new stream that you will manage the lifetime for,
sgStream* sgStreamNew(void);
// Initialize a stream, used by engine
void sgStreamInit(sgStream* stream);
bool sgStreamIsFinished(sgStream* stream);
void sgStreamFree(sgStream* stream);
// Close a stream, used by engine
void sgStreamClose(sgStream* stream);
#ifdef __cplusplus
}
#endif
