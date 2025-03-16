#include <Supergoon/Audio/Audio.h>
#include <Supergoon/Audio/Bgm.h>
#include <Supergoon/Audio/Sfx.h>
#include <Supergoon/Audio/Stream.h>
#include <Supergoon/events.h>
#include <Supergoon/log.h>
#include <assert.h>

#define MAX_SLOTS 4
static unsigned int _slot = 0;
typedef struct AudioBgmAsset {
	sgBgm* Bgm;
	float Volume;
} AudioBgmAsset;
typedef struct BgmLoadArgs {
	char* Name;
	int Loops;
	int Slot;
	float Volume;
} BgmLoadArgs;
AudioBgmAsset _bgmAssets[MAX_SLOTS];
float _globalBgmVolume = 1.0;

static void loadBgmInternal(Event* event) {
	BgmLoadArgs* args = (BgmLoadArgs*)event->user.data1;
	assert(args && "Cannot convert to bgmload args for some reason");
	char* fullPath = NULL;
	AudioBgmAsset* bgmAsset = &_bgmAssets[args->Slot];
	sgBgm* bgm = bgmAsset->Bgm;
	SDL_asprintf(&fullPath, "%sassets/audio/bgm/%s%s", SDL_GetBasePath(), args->Name, ".ogg");
	// If BGM is already in this slot, we should return as it's already loaded
	if (bgm && !strcmp(bgm->Filename, fullPath)) {
		goto cleanup;
	}
	// If theres already a bgm in there, then we should delete it.
	if (bgm) {
		sgBgmDelete(bgm);
		bgm = NULL;
	}
	bgm = sgBgmNew();
	bgm->Filename = fullPath;
	bgm->Loops = args->Loops;
	bgm->Volume = args->Volume * _globalBgmVolume;
	sgBgmLoad(bgm);
	bgmAsset->Volume = bgm->Volume;
cleanup:
	SDL_free(args->Name);
	SDL_free(fullPath);
	SDL_free(args);
}

static void playBgmInternal(Event* event) {
	sgBgmPlay(_bgmAssets[event->user.code].Bgm);
}

void InitializeAudio(void) {
	for (size_t i = 0; i < MAX_SLOTS; i++) {
		_bgmAssets[i].Bgm = NULL;
		_bgmAssets[i].Volume = 0;
	}
}

void AudioEventHandler(Event* event) {
	if (!(event->type == BuiltinEventIds.PlayBgmEvent ||
		  event->type == BuiltinEventIds.StopBgmEvent ||
		  event->type == BuiltinEventIds.LoadBgmEvent)) {
		return;
	}
	if (event->type == BuiltinEventIds.PlayBgmEvent) {
		playBgmInternal(event);
	}
}
void Update(void);
void SetBgmSlot(int slot) {
	if (slot < 0 || slot >= MAX_SLOTS) {
		sgLogWarn("Slot passed in is not available, slot unchanged, please use between 0 and %d", MAX_SLOTS);
		return;
	}
	_slot = slot;
}

void LoadBgm(const char* filename, float volume, int loops) {
	BgmLoadArgs* args = malloc(sizeof(*args));
	args->Name = strdup(filename);
	args->Loops = loops;
	args->Slot = _slot;
	args->Volume = volume;
	PushEvent(BuiltinEventIds.LoadBgmEvent, 0, args, NULL);
}

void PlayBgm(void) {
	PushEvent(BuiltinEventIds.LoadBgmEvent, _slot, NULL, NULL);
}

void UpdatePlayingBgmVolume(void) {
	for (size_t i = 0; i < MAX_SLOTS; i++) {
		if (!_bgmAssets[i].Bgm) continue;
		sgBgmUpdateVolume(_bgmAssets[i].Bgm, _globalBgmVolume * _bgmAssets[i].Volume);
	}
}

void PauseBgm(void);
void StopBgm(void);
void StopBgmFadeout(float fadeTime);
void SetPlayingBgmVolume(float volume);
void SetGlobalBgmVolume(float volume);
