#include <AL/al.h>
#include <AL/alc.h>
#include <Supergoon/filesystem.h>
#include <Supergoon/log.h>
#include <SupergoonEngine/Platform/openal/openal.h>
#include <SupergoonEngine/Platform/openal/openalBgm.h>
#include <SupergoonEngine/Platform/openal/openalStream.h>
#include <SupergoonEngine/tools.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <vorbis/vorbisfile.h>

#define MAX_STREAMS 4
static unsigned int _stream = 0;
Stream* _mainStream = NULL;

typedef struct BgmLoadArgs {
	char* Name;
	int Loops;
	int Track;
	float Volume;
} BgmLoadArgs;

typedef struct AudioBgmAsset {
	Bgm* BgmPtr;
	// char* Name;
	// float Volume;
	bool IsFading;
} AudioBgmAsset;

float _globalBgmVolume = 1.0;

AudioBgmAsset _bgmAssets[MAX_STREAMS];

#ifdef _WIN32
#define strncasecmp(x, y, z) _strnicmp(x, y, z)
#endif

static bool initializeAL(void) {
	const ALCchar* name;
	ALCdevice* device;
	ALCcontext* ctx;
	/* Open and initialize a device */
	device = NULL;
	if (!device)
		device = alcOpenDevice(NULL);
	if (!device) {
		sgLogError("Could not open a device!\n");
		return false;
	}
	ctx = alcCreateContext(device, NULL);
	if (ctx == NULL || alcMakeContextCurrent(ctx) == ALC_FALSE) {
		if (ctx != NULL)
			alcDestroyContext(ctx);
		alcCloseDevice(device);
		sgLogError("Could not set a context!\n");
		return false;
	}
	name = NULL;
	if (alcIsExtensionPresent(device, "ALC_ENUMERATE_ALL_EXT"))
		name = alcGetString(device, ALC_ALL_DEVICES_SPECIFIER);
	if (!name || alcGetError(device) != AL_NO_ERROR)
		name = alcGetString(device, ALC_DEVICE_SPECIFIER);
	sgLogDebug("Opened %s", name);
	return true;
}

void InitializeAudioImpl(void) {
	bool result = initializeAL();
	if (!result) {
		sgLogError("Could not load audio system!");
	}
	_mainStream = StreamNew();
}
void SetBgmTrackImpl(int track) {
	if (track < 0 || track >= MAX_STREAMS) {
		sgLogWarn("Track passed in is not available, track unchanged, please use between 0 and %d", MAX_STREAMS);
		return;
	}
	_stream = track;
}
static void loadBgmInternal(BgmLoadArgs* args) {
	AudioBgmAsset* bgmAsset = &_bgmAssets[args->Track];
	char* fullPath = NULL;
	asprintf(&fullPath, "%sassets/audio/bgm/%s%s", GetBasePath(), args->Name, ".ogg");
	// If BGM is already in this track, we should return as it's already loaded
	if (bgmAsset->BgmPtr && bgmAsset->BgmPtr->Filename && !strcmp(bgmAsset->BgmPtr->Filename, fullPath)) {
		free(fullPath);
		return;
	}
	// If theres already a bgm in there, then we should delete it.
	if (bgmAsset->BgmPtr) {
		BgmDelete(bgmAsset->BgmPtr);
		bgmAsset->BgmPtr = NULL;
	}
	bgmAsset->BgmPtr = BgmNew();
	bgmAsset->BgmPtr->Filename = fullPath;
	bgmAsset->BgmPtr->Loops = args->Loops;
	bgmAsset->BgmPtr->Volume = args->Volume * _globalBgmVolume;
	BgmLoad(bgmAsset->BgmPtr);
	// bgmAsset->Volume = bgmAsset->BgmPtr->Volume;
}

void LoadBgmImpl(const char* filename, float volume, int loops) {
	BgmLoadArgs args;
	args.Name = strdup(filename);
	args.Loops = loops;
	args.Track = _stream;
	args.Volume = volume;
	loadBgmInternal(&args);
	AudioBgmAsset* bgmAsset = &_bgmAssets[_stream];
	_mainStream->BgmData = bgmAsset->BgmPtr;
	StreamLoad(_mainStream);
}

void PlayBgmImpl(void) {
	UpdatePlayingBgmVolumeImpl();
	StreamPlay(_mainStream);
	// BgmPlay(_bgmAssets[event->user.code].BgmPtr);
}

void PauseBgmImpl(void) {
}
void StopBgmImpl(void) {
}
void StopBgmFadeoutImpl(void) {
}
void UpdatePlayingBgmVolumeImpl(void) {
	for (size_t i = 0; i < MAX_STREAMS; i++) {
		if (!_bgmAssets[i].BgmPtr) continue;
		// Update the stream volume
		StreamUpdateVolume(_mainStream, _globalBgmVolume * _bgmAssets[i].BgmPtr->Volume);
		// _bgmAssets[i].BgmPtr->Volume = _globalBgmVolume * _bgmAssets[i].Volume;
	}
}
void SetGlobalBgmVolumeImpl(float volume) {
}
void SetGlobalSfxVolumeImpl(float volume) {
}
void PlaySfxOneShotImpl(const char* name, float volume) {
}
void AudioEventHandlerImpl(void* event) {}
void CloseAudioImpl(void) {
}
void AudioUpdateImpl(void) {
	if (_mainStream) {
		StreamUpdate(_mainStream);
	}
}
