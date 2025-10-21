/**
 * @file Audio.h
 * @author Kevin B (kevin@supergoon.com)
 * @brief Audio System for engine.  Handles loading, playing, etc for all audio
 * @version 0.1
 * @date 2025-03-16
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifdef __cplusplus
extern "C" {
#endif

#pragma once
// Game functions
void SetBgmTrack(int track);
void LoadBgm(const char *filename, float volume, int loops);
/**
 * @brief Plays a BGM, additionally unpauses it if it is paused.
 */
void PlayBgm(void);
/**
 * @brief Pauses a BGM, can be resumed with PlayBGm afterwards
 */
void PauseBgm(void);
/**
 * @brief Stops a bgm, returning it to the start but does noe unload it
 */
void StopBgm(void);
void StopBgmFadeout(void);
void UpdatePlayingBgmVolume(void);
void SetGlobalBgmVolume(float volume);
void SetGlobalSfxVolume(float volume);
// void PlaySfx(Sfx *sfx, float volume = 1.0);
void PlaySfxOneShot(const char *name, float volume);
void AudioEventHandler(void *event);
void ShutdownAudioSystem(void);
void UpdateAudioSystem(void);
void InitializeAudioSystem(void);

#ifdef __cplusplus
}
#endif
