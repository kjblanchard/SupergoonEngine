#include <AL/al.h>
#include <AL/alc.h>
#include <Supergoon/filesystem.h>
#include <Supergoon/log.h>
#include <Supergoon/tools.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <vorbis/vorbisfile.h>

#define MAX_SFX_STREAMS 8
#define MAX_SFX_CACHE 16
#define VORBIS_READ_SIZE 4096

static unsigned long _usedCounter = 0;

typedef struct Sfx {
	ALenum Format;
	ALsizei Size;
	long SampleRate;
	short *Data;
} Sfx;

typedef struct {
	char *Name;
	Sfx SfxData;
	unsigned long LastUsed;

} SfxCacheEntry;

static SfxCacheEntry SfxCache[MAX_SFX_CACHE];
static int SfxCacheCount = 0;

static ALuint Sources[MAX_SFX_STREAMS];

static int FindEvictionIndex(void) {
	unsigned long oldest = (unsigned long)-1;
	int oldestIndex = 0;

	for (int i = 0; i < SfxCacheCount; i++) {
		if (SfxCache[i].LastUsed < oldest) {
			oldest = SfxCache[i].LastUsed;
			oldestIndex = i;
		}
	}
	return oldestIndex;
}

// ======================================================
// Initialization
// ======================================================
void InitializeSfxSystem(void) {
	alGenSources(MAX_SFX_STREAMS, Sources);
	assert(alGetError() == AL_NO_ERROR && "Could not create sound sources");

	for (int i = 0; i < MAX_SFX_STREAMS; ++i) {
		alSource3f(Sources[i], AL_POSITION, 0, 0, -1);
		alSourcei(Sources[i], AL_SOURCE_RELATIVE, AL_TRUE);
		alSourcei(Sources[i], AL_ROLLOFF_FACTOR, 0);
	}
}

static Sfx *FindCachedSfx(const char *filename) {
	for (int i = 0; i < SfxCacheCount; i++) {
		if (strcmp(SfxCache[i].Name, filename) == 0) {
			SfxCache[i].LastUsed = _usedCounter++;
			return &SfxCache[i].SfxData;
		}
	}
	return NULL;
}

static void AddToCache(const char *filename, Sfx *sfx) {
	int index = SfxCacheCount;
	if (SfxCacheCount >= MAX_SFX_CACHE) {
		index = FindEvictionIndex();
		free(SfxCache[index].Name);
		free(SfxCache[index].SfxData.Data);
	} else {
		SfxCacheCount++;
	}

	SfxCacheEntry *entry = &SfxCache[index];
	entry->Name = strdup(filename);
	entry->SfxData = *sfx;
}

static Sfx *LoadSfxFile(const char *filename) {
	Sfx *cached = FindCachedSfx(filename);
	if (cached) return cached;
	Sfx newSfx = {0};
	OggVorbis_File vf;
	vorbis_info *info;
	char *fullPath = NULL;
	asprintf(&fullPath, "%sassets/audio/sfx/%s.ogg", GetBasePath(), filename);
	int result = ov_fopen(fullPath, &vf);
	free(fullPath);
	if (result != 0) {
		sgLogError("Could not open audio '%s'. ov error: %d", filename, result);
		return NULL;
	}
	info = ov_info(&vf, -1);
	if (info->channels == 1)
		newSfx.Format = AL_FORMAT_MONO16;
	else if (info->channels == 2)
		newSfx.Format = AL_FORMAT_STEREO16;
	else {
		sgLogError("Unsupported channel count %d in '%s'", info->channels, filename);
		ov_clear(&vf);
		return NULL;
	}
	newSfx.SampleRate = info->rate;
	newSfx.Size = ov_pcm_total(&vf, -1) * info->channels * sizeof(short);
	newSfx.Data = malloc(newSfx.Size);
	long total = 0;
	while (total < newSfx.Size) {
		long bytes = ov_read(&vf, ((char *)newSfx.Data) + total, VORBIS_READ_SIZE, 0, 2, 1, NULL);
		if (bytes <= 0)
			break;
		total += bytes;
	}
	ov_clear(&vf);
	AddToCache(filename, &newSfx);
	return FindCachedSfx(filename);
}

static int GetFreeSource(void) {
	ALint state;
	for (int i = 0; i < MAX_SFX_STREAMS; i++) {
		alGetSourcei(Sources[i], AL_SOURCE_STATE, &state);
		if (state != AL_PLAYING)
			return i;
	}
	// No free source: steal source 0
	return 0;
}

void SfxPlayOneShot(const char *filename, float volume) {
	Sfx *snd = LoadSfxFile(filename);
	if (!snd) return;
	int src = GetFreeSource();
	ALuint buffer;
	alGenBuffers(1, &buffer);
	alBufferData(buffer, snd->Format, snd->Data, snd->Size, snd->SampleRate);
	alSourceStop(Sources[src]);
	alSourceRewind(Sources[src]);
	alSourcei(Sources[src], AL_BUFFER, buffer);
	alSourcef(Sources[src], AL_GAIN, volume);
	alSourcePlay(Sources[src]);
}
