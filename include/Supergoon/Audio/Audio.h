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
typedef union SDL_Event Event;

// Engine Functions

// Game functions
void SetBgmTrack(int track);
void LoadBgm(const char* filename, float volume, int loops);
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
void StopBgmFadeout(float fadeTime);
void UpdatePlayingBgmVolume(void);

// inline void SetGlobalSfxVolume(float volume) { _globalSfxVolume = volume; };
// void PlaySfx(Sfx *sfx, float volume = 1.0);
// void PlaySfxOneShot(const char *name, float volume = 1.0);
// static inline Sound *Instance() { return _instance; }
//  Updates all internal bgms
// making public for now to test threading.

#ifdef __cplusplus
}
#endif
