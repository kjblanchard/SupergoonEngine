#ifdef sdlbackend
#include <sgsound/openal.h>

#endif
void SetBgmTrack(int track) { SetBgmTrackImpl(track); }
void LoadBgm(const char* filename, float volume, int loops) { LoadBgmFImpl(filename, volume, loops); }
void LoadBgmBuffer(const char* filename, float volume, int loops, char* data, size_t dataSize) { LoadBgmImpl(filename, data, dataSize, volume, loops); }
void PlayBgm(void) { PlayBgmImpl(); }
void UpdatePlayingBgmVolume(void) { UpdatePlayingBgmVolumeImpl(); }
void PauseBgm(void) { PauseBgmImpl(); }
void StopBgm(void) { StopBgmImpl(); }
void SetGlobalBgmVolume(float volume) { SetGlobalBgmVolumeImpl(volume); }
void SetGlobalSfxVolume(float volume) { SetGlobalSfxVolumeImpl(volume); }
void PlaySfxOneShot(const char* name, float volume) { PlaySfxOneShotFImpl(name, volume); }
void ShutdownAudioSystem(void) { CloseAudioImpl(); }
void UpdateAudioSystem(void) { AudioUpdateImpl(); }
void InitializeAudioSystem(void) { InitializeAudioImpl(); }
