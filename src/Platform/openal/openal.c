#include <AL/al.h>
#include <AL/alc.h>
#include <Supergoon/Platform/openal/openal.h>
#include <Supergoon/Platform/openal/openalBgm.h>
#include <Supergoon/Platform/openal/openalSfx.h>
#include <Supergoon/Platform/openal/openalStream.h>
#include <Supergoon/filesystem.h>
#include <Supergoon/log.h>
#include <Supergoon/tools.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <vorbis/vorbisfile.h>

#define MAX_STREAMS 2
static unsigned int _currentStreamID = 0;
Stream* _streams[MAX_STREAMS];

typedef struct BgmLoadArgs {
	char* Name;
	int Loops;
	int Track;
	float Volume;
} BgmLoadArgs;

typedef struct AudioBgmAsset {
	Bgm* BgmPtr;
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
	for (int i = 0; i < MAX_STREAMS; ++i) {
		_streams[i] = StreamNew();
	}
	InitializeSfxSystem();
}
void SetBgmTrackImpl(int track) {
	if (track < 0 || track >= MAX_STREAMS) {
		sgLogWarn("Track passed in is not available, track unchanged, please use between 0 and %d", MAX_STREAMS);
		return;
	}
	_currentStreamID = track;
}
static void loadBgmInternal(BgmLoadArgs* args, char* fullPath) {
	AudioBgmAsset* bgmAsset = &_bgmAssets[args->Track];
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
	char* fullPath = NULL;
	AudioBgmAsset* bgmAsset = &_bgmAssets[_currentStreamID];
	asprintf(&fullPath, "%sassets/audio/bgm/%s%s", GetBasePath(), filename, ".ogg");
	// If BGM is already in this track, we should return as it's already loaded
	if (bgmAsset->BgmPtr && bgmAsset->BgmPtr->Filename && strcmp(bgmAsset->BgmPtr->Filename, fullPath) == 0) {
		free(fullPath);
		return;
	}
	BgmLoadArgs args;
	args.Name = strdup(filename);
	args.Loops = loops;
	args.Track = _currentStreamID;
	args.Volume = volume;
	loadBgmInternal(&args, fullPath);
	Stream* stream = _streams[_currentStreamID];
	stream->BgmData = bgmAsset->BgmPtr;
	StreamLoad(stream);
	free(args.Name);
}

void PlayBgmImpl(void) {
	UpdatePlayingBgmVolumeImpl();
	StreamPlay(_streams[_currentStreamID]);
}

void PauseBgmImpl(void) {
}

void StopBgmImpl(void) {
	if(!_streams[_currentStreamID]) return;
	StreamStop(_streams[_currentStreamID]);
}

void StopBgmFadeoutImpl(void) {
}
void UpdatePlayingBgmVolumeImpl(void) {
	for (size_t i = 0; i < MAX_STREAMS; i++) {
		if (!_bgmAssets[i].BgmPtr) continue;
		StreamUpdateVolume(_streams[i], _globalBgmVolume * _bgmAssets[i].BgmPtr->Volume);
	}
}
void SetGlobalBgmVolumeImpl(float volume) {
	_globalBgmVolume = volume;
	UpdatePlayingBgmVolumeImpl();
}
void SetGlobalSfxVolumeImpl(float volume) {
}
void PlaySfxOneShotImpl(const char* name, float volume) {
	SfxPlayOneShot(name, volume);
}
void AudioEventHandlerImpl(void* event) {}
void CloseAudioImpl(void) {
	for (int i = 0; i < MAX_STREAMS; ++i) {
		AudioBgmAsset* asset = &_bgmAssets[i];
		BgmDelete(asset->BgmPtr);
	}
}
void AudioUpdateImpl(void) {
	for (int i = 0; i < MAX_STREAMS; ++i) {
		if (!_bgmAssets[i].BgmPtr) continue;
		StreamUpdate(_streams[i]);
	}
}
