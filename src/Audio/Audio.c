#include <SDL3/SDL_events.h>
#include <SDL3/SDL_filesystem.h>
#include <Supergoon/Audio/Audio.h>
#include <Supergoon/Audio/Bgm.h>
#include <Supergoon/Audio/Sfx.h>
#include <Supergoon/Audio/Stream.h>
#include <Supergoon/Tweening/tween.h>
#include <Supergoon/events.h>
#include <Supergoon/log.h>
#include <Supergoon/state.h>
#include <assert.h>

#define MAX_TRACKS 4
#define MAX_SFX_STREAMS 8
#define SFX_CACHE_SIZE 12
static unsigned int _track = 0;
typedef struct AudioBgmAsset {
	Bgm* Bgm;
	float Volume;
	bool IsFading;
} AudioBgmAsset;
typedef struct AudioSfxAsset {
	Sfx Sfx;
	uint32_t LastUsedTicks;
} AudioSfxAsset;
typedef struct BgmLoadArgs {
	char* Name;
	int Loops;
	int Track;
	float Volume;
} BgmLoadArgs;
AudioBgmAsset _bgmAssets[MAX_TRACKS];
float _globalBgmVolume = 1.0;
float _globalSfxVolume = 1.0;
sgStream _sfxStreams[MAX_SFX_STREAMS];
AudioSfxAsset _sfxCache[SFX_CACHE_SIZE];

static void loadBgmInternal(Event* event);
static void stopBgmInternal(Event* event);
static void pauseBgmInternal(Event* event);
static void playBgmInternal(Event* event);

/**
 * @brief Called from the engine to initialize, not for consumer
 */
void initializeAudio(void) {
	for (size_t i = 0; i < MAX_TRACKS; i++) {
		_bgmAssets[i].Bgm = NULL;
		_bgmAssets[i].Volume = 0;
		_bgmAssets[i].IsFading = false;
	}

	for (size_t i = 0; i < MAX_SFX_STREAMS; i++) {
		sgStreamInit(&_sfxStreams[i]);
	}

	for (size_t i = 0; i < SFX_CACHE_SIZE; i++) {
		snprintf(_sfxCache[i].Sfx.Filename, SFX_MAX_CHARS, "");
		_sfxCache[i].Sfx.Buffer = NULL;
		_sfxCache[i].LastUsedTicks = 0;
	}
}

void closeAudio(void) {
	for (size_t i = 0; i < MAX_TRACKS; i++) {
		if (_bgmAssets[i].Bgm) {
			bgmDelete(_bgmAssets[i].Bgm);
		}
	}

	for (size_t i = 0; i < MAX_SFX_STREAMS; i++) {
		sgStreamClose(&_sfxStreams[i]);
	}

	for (size_t i = 0; i < SFX_CACHE_SIZE; i++) {
		if (_sfxCache[i].Sfx.Buffer) {
			SDL_free(_sfxCache[i].Sfx.Buffer);
		}
	}
}

/**
 * @brief Called from the engine in events.c, not for consumer
 *
 * @param event
 */
void audioEventHandler(Event* event) {
	if (!(event->type == BuiltinEventIds.PlayBgmEvent ||
		  event->type == BuiltinEventIds.StopBgmEvent ||
		  event->type == BuiltinEventIds.LoadBgmEvent ||
		  event->type == BuiltinEventIds.PauseBgmEvent)) {
		return;
	}
	if (event->type == BuiltinEventIds.PlayBgmEvent) {
		playBgmInternal(event);
	} else if (event->type == BuiltinEventIds.LoadBgmEvent) {
		loadBgmInternal(event);
	} else if (event->type == BuiltinEventIds.StopBgmEvent) {
		stopBgmInternal(event);
	} else if (event->type == BuiltinEventIds.PauseBgmEvent) {
		pauseBgmInternal(event);
	}
}

static void loadBgmInternal(Event* event) {
	BgmLoadArgs* args = (BgmLoadArgs*)event->user.data1;
	assert(args && "Cannot convert to bgmload args for some reason");
	char* fullPath = NULL;
	AudioBgmAsset* bgmAsset = &_bgmAssets[args->Track];
	// sgBgm* bgm = bgmAsset->Bgm;
	SDL_asprintf(&fullPath, "%sassets/audio/bgm/%s%s", SDL_GetBasePath(), args->Name, ".ogg");
	// If BGM is already in this track, we should return as it's already loaded
	if (bgmAsset->Bgm && !strcmp(bgmAsset->Bgm->Filename, fullPath)) {
		SDL_free(fullPath);
		goto cleanup;
	}
	// If theres already a bgm in there, then we should delete it.
	if (bgmAsset->Bgm) {
		bgmDelete(bgmAsset->Bgm);
	}
	bgmAsset->Bgm = bgmNew();
	bgmAsset->Bgm->Filename = fullPath;
	bgmAsset->Bgm->Loops = args->Loops;
	bgmAsset->Bgm->Volume = args->Volume * _globalBgmVolume;
	bgmLoad(bgmAsset->Bgm);
	bgmAsset->Volume = bgmAsset->Bgm->Volume;
cleanup:
	SDL_free(args->Name);
	SDL_free(args);
}

static void pauseBgmInternal(Event* event) {
	int track = event->user.code;
	AudioBgmAsset* _bgm = &_bgmAssets[track];
	if (!_bgm->Bgm || !_bgm->Bgm->IsPlaying) {
		return;
	}
	bgmPause(_bgm->Bgm);
}

static void stopBgmInternal(Event* event) {
	int track = event->user.code;
	AudioBgmAsset* _bgm = &_bgmAssets[track];
	if (!_bgm->Bgm || !_bgm->Bgm->IsPlaying) {
		return;
	}
	bgmStop(_bgm->Bgm);
}

static void playBgmInternal(Event* event) {
	UpdatePlayingBgmVolume();
	bgmPlay(_bgmAssets[event->user.code].Bgm);
}

/**
 * @brief Called from the engine in engine.c, not for consumer
 *
 */
void audioUpdate(void) {
	for (size_t i = 0; i < MAX_TRACKS; i++) {
		if (_bgmAssets[i].Bgm) {
			bgmUpdate(_bgmAssets[i].Bgm);
		}
	}
}

void SetBgmTrack(int track) {
	if (track < 0 || track >= MAX_TRACKS) {
		sgLogWarn("Track passed in is not available, track unchanged, please use between 0 and %d", MAX_TRACKS);
		return;
	}
	_track = track;
}

void LoadBgm(const char* filename, float volume, int loops) {
	BgmLoadArgs* args = malloc(sizeof(*args));
	args->Name = strdup(filename);
	args->Loops = loops;
	args->Track = _track;
	args->Volume = volume;
	PushEvent(BuiltinEventIds.LoadBgmEvent, 0, args, NULL);
}

void PlayBgm(void) {
	PushEvent(BuiltinEventIds.PlayBgmEvent, _track, NULL, NULL);
}

void UpdatePlayingBgmVolume(void) {
	for (size_t i = 0; i < MAX_TRACKS; i++) {
		if (!_bgmAssets[i].Bgm) continue;
		bgmUpdateVolume(_bgmAssets[i].Bgm, _globalBgmVolume * _bgmAssets[i].Volume);
	}
}

void PauseBgm(void) {
	PushEvent(BuiltinEventIds.PauseBgmEvent, _track, NULL, NULL);
}
void StopBgm(void) {
	PushEvent(BuiltinEventIds.StopBgmEvent, _track, NULL, NULL);
}

static void updateBgmFadeoutUpdateFunc(void* bgmVoid) {
	(void)bgmVoid;
	UpdatePlayingBgmVolume();
}

static void stopBgmFadeoutEndFunc(void* bgmVoid) {
	AudioBgmAsset* bgmAsset = (AudioBgmAsset*)bgmVoid;
	assert(bgmAsset && "Bad bgm asset passed in, something is wrong");
	bgmAsset->IsFading = false;
	bgmAsset->Volume = 0;
	if (!bgmAsset->Bgm || !bgmAsset->Bgm->IsPlaying) {
		return;
	}
	bgmStop(bgmAsset->Bgm);
}

void StopBgmFadeout(void) {
	AudioBgmAsset* bgmAsset = &_bgmAssets[_track];
	if (!(bgmAsset && bgmAsset->Bgm)) {
		sgLogInfo("Trying to fadeout an invalid BGM");
		return;
	}
	if (bgmAsset->IsFading) {
		sgLogInfo("Trying to fadeout a BGM that is already fading");
		return;
	}
	bgmAsset->IsFading = true;
	Tween tween = CreateFloatTween(&bgmAsset->Volume, bgmAsset->Volume, 0.0, 0.25, TweenEasingLinear);
	SetTweenUserdata(tween, bgmAsset);
	SetTweenFunctions(tween, NULL, updateBgmFadeoutUpdateFunc, stopBgmFadeoutEndFunc);
	StartTween(tween);
}
void SetPlayingBgmVolume(float volume) {
	if (!_bgmAssets[_track].Bgm) return;
	_bgmAssets[_track].Bgm->Volume = volume;
	UpdatePlayingBgmVolume();
}

void SetGlobalBgmVolume(float volume) {
	_globalBgmVolume = volume;
	UpdatePlayingBgmVolume();
}

void SetGlobalSfxVolume(float volume) {
	_globalSfxVolume = volume;
}

void PlaySfxOneShot(const char* name, float volume) {
	sgStream* stream = NULL;
	for (size_t i = 0; i < MAX_SFX_STREAMS; i++) {
		if (sgStreamIsFinished(&_sfxStreams[i])) {
			stream = &_sfxStreams[i];
			break;
		}
	}
	if (!stream) {
		sgLogWarn("No free sfx streams, not playing SFX");
	}
	// Check the cache if we already loaded this
	char fullPathBuffer[SFX_MAX_CHARS];
	if (snprintf(fullPathBuffer, sizeof(fullPathBuffer), "%sassets/audio/sfx/%s%s", SDL_GetBasePath(), name, ".ogg") >= (int)sizeof(fullPathBuffer) - 1) {
		sgLogError("Buffer size to small for comparing, probably use an alloc, or increase SFX_MAX_CHARS");
		return;
	}
	Sfx* sfx = NULL;
	for (size_t i = 0; i < SFX_CACHE_SIZE; i++) {
		if (strcmp(fullPathBuffer, _sfxCache[i].Sfx.Filename) == 0) {
			sfx = &_sfxCache[i].Sfx;
			break;
		}
	}
	if (sfx) {
		sfx->Volume = volume * _globalSfxVolume;
		SfxPlay(sfx, stream);
		return;
	}
	// Clean the sfx used a while ago, and cache a new one
	uint32_t oldestCache = UINT32_MAX;
	int oldestCacheNum = 0;
	for (size_t i = 0; i < SFX_CACHE_SIZE; i++) {
		if (_sfxCache[i].LastUsedTicks < oldestCache) {
			oldestCache = _sfxCache[i].LastUsedTicks;
			oldestCacheNum = i;
		}
	}
	AudioSfxAsset* sfxAsset = &_sfxCache[oldestCacheNum];
	sfxAsset->LastUsedTicks = Ticks;
	SDL_free(sfxAsset->Sfx.Buffer);
	sfxAsset->Sfx.Buffer = NULL;
	memcpy(sfxAsset->Sfx.Filename, fullPathBuffer, SFX_MAX_CHARS);
	sfxAsset->Sfx.Filename[SFX_MAX_CHARS - 1] = '\0';  // Ensure null termination f
	SfxLoad(&sfxAsset->Sfx);
	sfxAsset->Sfx.Volume = volume * _globalSfxVolume;
	SfxPlay(&sfxAsset->Sfx, stream);
}
