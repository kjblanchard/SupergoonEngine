#include <stdint.h>
#include <vorbis/vorbisfile.h>
typedef struct Bgm {
	char* Filename;
	int Loops;
	uint64_t LoopStart;
	uint64_t LoopEnd;
	int IsLoaded;
	int IsPlaying;
	OggVorbis_File* VorbisFile;
	vorbis_info* VorbisInfo;
	int Format;
	float Volume;
} Bgm;

Bgm* BgmNew(void);
void BgmLoad(Bgm* bgm);
void BgmDelete(Bgm* bgm);
// void BgmPlay(Bgm* bgm);
