#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef struct SDL_AudioStream SDL_AudioStream;
typedef struct OggVorbis_File OggVorbis_File;
typedef struct vorbis_info vorbis_info;

typedef struct sgBgm {
	char* Filename;
	int Loops;
	uint64_t LoopStart;
	uint64_t LoopEnd;
	uint64_t CurrentLoopBytesRead;
	short* Buffer;
	bool CanPlay;
	bool IsPlaying;
	OggVorbis_File* VorbisFile;
	vorbis_info* VorbisInfo;
	SDL_AudioStream* Stream;
	float Volume;

} sgBgm;

sgBgm* sgBgmNew(void);
void sgBgmLoad(sgBgm* bgm);
void sgBgmPlay(sgBgm* bgm);
void sgBgmPause(sgBgm* bgm);
void sgBgmStop(sgBgm* bgm);
void sgBgmUpdate(sgBgm* bgm);
void sgBgmDelete(sgBgm* bgm);
void sgBgmUpdateVolume(sgBgm* bgm, float volume);

#ifdef __cplusplus
}
#endif
