#ifdef miniaudio
#include <SupergoonEngine/Platform/openal/openal.h>
#else
#include <SupergoonEngine/Platform/sdl/sdlAudio.h>
#endif

void SetBgmTrack(int track) { SetBgmTrackImpl(track); }
void LoadBgm(const char* filename, float volume, int loops) { LoadBgmImpl(filename, volume, loops); }
void PlayBgm(void) { PlayBgmImpl(); }
void UpdatePlayingBgmVolume(void) { UpdatePlayingBgmVolumeImpl(); }
void PauseBgm(void) { PauseBgmImpl(); }
void StopBgm(void) { StopBgmImpl(); }
void StopBgmFadeout(void) { StopBgmFadeoutImpl(); }
void SetGlobalBgmVolume(float volume) { SetGlobalBgmVolumeImpl(volume); }
void SetGlobalSfxVolume(float volume) { SetGlobalSfxVolumeImpl(volume); }
void PlaySfxOneShot(const char* name, float volume) { PlaySfxOneShotImpl(name, volume); }
void AudioEventHandler(void* event) { AudioEventHandlerImpl(event); }
void CloseAudio(void) { CloseAudioImpl(); }
void AudioUpdate(void) { AudioUpdateImpl(); }
void InitializeAudio(void) { InitializeAudioImpl(); }
