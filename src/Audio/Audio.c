#include <Supergoon/Audio/Audio.h>
#include <Supergoon/Audio/Bgm.h>
#include <Supergoon/Audio/Sfx.h>
#include <Supergoon/Audio/Stream.h>
#include <Supergoon/Tweening/tween.h>
#include <Supergoon/events.h>
#include <Supergoon/log.h>
#include <assert.h>

#define MAX_TRACKS 4
static unsigned int _track = 0;
typedef struct AudioBgmAsset {
	Bgm* Bgm;
	float Volume;
	bool IsFading;
} AudioBgmAsset;
typedef struct BgmLoadArgs {
	char* Name;
	int Loops;
	int Track;
	float Volume;
} BgmLoadArgs;
AudioBgmAsset _bgmAssets[MAX_TRACKS];
float _globalBgmVolume = 1.0;

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
