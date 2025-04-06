#pragma once
#include <SDL3/SDL_audio.h>

#ifdef __cplusplus
extern "C" {
#endif
#define SFX_MAX_CHARS 255
struct sgStream;

typedef struct Sfx {
	char Filename[SFX_MAX_CHARS];
	short* Buffer;
	int BufferLength;
	//  between 0 and 1.0f;
	float Volume;
	SDL_AudioSpec spec;

} Sfx;

// Creates a SFX, still need to set filename
Sfx* SfxNew(void);
// Load all data from the sfx to the buffer for playing.
void SfxLoad(Sfx*);
// Put data from the buffer into the stream
void SfxPlay(Sfx*, struct sgStream*);
// void SfxPlayOneShot(const char* filename, sgStream*);
// Frees a sfx
void SfxDelete(Sfx*);

#ifdef __cplusplus
}
#endif
