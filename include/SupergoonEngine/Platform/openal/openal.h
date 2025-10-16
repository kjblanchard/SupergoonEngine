#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void SetBgmTrackImpl(int track);
void LoadBgmImpl(const char *filename, float volume, int loops);
void PlayBgmImpl(void);
void PauseBgmImpl(void);
void StopBgmImpl(void);
void StopBgmFadeoutImpl(void);
void UpdatePlayingBgmVolumeImpl(void);
void SetGlobalBgmVolumeImpl(float volume);
void SetGlobalSfxVolumeImpl(float volume);
void PlaySfxOneShotImpl(const char *name, float volume);
void AudioEventHandlerImpl(void *event);
void CloseAudioImpl(void);
void AudioUpdateImpl(void);
void InitializeAudioImpl(void);

#ifdef __cplusplus
}
#endif
