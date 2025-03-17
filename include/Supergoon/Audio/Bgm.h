#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef struct SDL_AudioStream SDL_AudioStream;
typedef struct OggVorbis_File OggVorbis_File;
typedef struct vorbis_info vorbis_info;

typedef struct Bgm {
	char* Filename;
	int Loops;
	uint64_t LoopStart;
	uint64_t LoopEnd;
	uint64_t CurrentLoopBytesRead;
	short* Buffer;
	bool IsLoaded;
	bool IsPlaying;
	OggVorbis_File* VorbisFile;
	vorbis_info* VorbisInfo;
	SDL_AudioStream* Stream;
	float Volume;

} Bgm;

Bgm* bgmNew(void);
void bgmLoad(Bgm* bgm);
void bgmPlay(Bgm* bgm);
void bgmPause(Bgm* bgm);
void bgmStop(Bgm* bgm);
void bgmUpdate(Bgm* bgm);
void bgmDelete(Bgm* bgm);
void bgmUpdateVolume(Bgm* bgm, float volume);
double getBgmDuration(Bgm* bgm);
double getBgmCurrentPosition(Bgm* bgm);

#ifdef __cplusplus
}
#endif
