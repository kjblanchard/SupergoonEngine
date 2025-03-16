#include <SDL3/SDL.h>
#include <Supergoon/Audio/Bgm.h>
#include <Supergoon/log.h>
#include <stdlib.h>
#include <vorbis/vorbisfile.h>

#ifdef _WIN32
#define strncasecmp(x, y, z) _strnicmp(x, y, z)
#endif

#define BGM_NUM_BUFFERS 4		  // Amount of "Buffers" we should have buffered in the SDL stream
#define BGM_BUFFER_SIZE 8192	  // 8kb
#define VORBIS_REQUEST_SIZE 4096  // Size of vorbis requests, usually recommend to be 4096
// #define MINIMUM_STREAM_SIZE (BGM_BUFFER_SIZE * BGM_NUM_BUFFERS) - BGM_BUFFER_SIZE  // If our sdl stream is less than this, add another buffer
#define MINIMUM_STREAM_SIZE BGM_BUFFER_SIZE *BGM_NUM_BUFFERS  // If our sdl stream is less than this, add another buffer

static void getLoopPointsFromVorbisComments(sgBgm *bgm, double *loopBegin, double *loopEnd) {
	vorbis_comment *vc = ov_comment(bgm->VorbisFile, -1);
	if (!vc) {
		// SDL_LogWarn(SG_LOG_LEVEL_sound, "Error retrieving vorbis comments for %s , setting to 0", bgm->Filename);
		sgLogWarn("Error retrieving vorbis comments for %s , setting to 0", bgm->Filename);
		*loopBegin = *loopEnd = 0;
		return;
	}
	const char *start = "LOOPSTART=";
	const char *end = "LOOPEND=";
	for (int i = 0; i < vc->comments; ++i) {
		char *comment = vc->user_comments[i];
		if (strncasecmp(comment, start, strlen(start)) == 0) {
			float startTime = atof(comment + strlen(start));
			*loopBegin = startTime;
		} else if (strncasecmp(comment, end, strlen(end)) == 0) {
			float endTime = atof(comment + strlen(end));
			*loopEnd = endTime;
		}
	}
}

static void setBgmLoopPoints(sgBgm *bgm) {
	double loopBegin = 0, loopEnd = 0;
	if (bgm->Loops != 0) {
		getLoopPointsFromVorbisComments(bgm, &loopBegin, &loopEnd);
	}
	if (loopBegin >= loopEnd) {
		loopEnd = 0;
	}
	bgm->LoopStart = loopBegin > 0 ? (int64_t)(loopBegin * bgm->VorbisInfo->rate)
								   : ov_pcm_tell(bgm->VorbisFile);
	// Loop end needs to be measured against our buffers loading,
	//	so they will be multiplied by channels and sizeof,Due to us checking this on every step.
	if (loopEnd && loopEnd > 0) {
		int64_t samplesOffset = (int64_t)(loopEnd * bgm->VorbisInfo->rate);
		bgm->LoopEnd = samplesOffset * bgm->VorbisInfo->channels * sizeof(short);
	} else {
		bgm->LoopEnd = ov_pcm_total(bgm->VorbisFile, -1) * bgm->VorbisInfo->channels * sizeof(short);
	}
}

static void loadDataToStream(sgBgm *bgm) {
	if (!bgm->CanPlay) {
		return;
	}
	uint64_t requestSize = VORBIS_REQUEST_SIZE;
	long total_buffer_bytes_read = 0;
	bool isLooped = false;
	// Try and load a full buffer
	while (total_buffer_bytes_read < BGM_BUFFER_SIZE) {
		// Request size should be either the full request size, or until we can fill our buffer.
		requestSize = (total_buffer_bytes_read + requestSize <= BGM_BUFFER_SIZE)
						  ? requestSize
						  : (uint64_t)BGM_BUFFER_SIZE - total_buffer_bytes_read;
		//   Update to not go past the loop end
		requestSize = (total_buffer_bytes_read + requestSize + bgm->CurrentLoopBytesRead <= bgm->LoopEnd)
						  ? requestSize
						  : bgm->LoopEnd - (total_buffer_bytes_read + bgm->CurrentLoopBytesRead);
		if (requestSize == 0) {
			// We are at the end of the loop point and should loop or stop.
			if (bgm->Loops == 0) {
				sgBgmStop(bgm);
				break;
			}
			ov_pcm_seek_lap(bgm->VorbisFile, bgm->LoopStart);
			// Convert samples to bytes and set that as our current location in the loop
			bgm->CurrentLoopBytesRead = ov_pcm_tell(bgm->VorbisFile) * bgm->VorbisInfo->channels * sizeof(short);
			--bgm->Loops;
			isLooped = true;
			break;
		}
		// Actually read the file into our memory buffer
		int current_pass_bytes_read = ov_read(bgm->VorbisFile, (char *)bgm->Buffer + total_buffer_bytes_read, requestSize, 0, sizeof(short), 1, 0);
		if (current_pass_bytes_read == 0) {
			sgLogWarn("We reached the end of the song somehow, probably something is wrong with loops\n");
			break;
		}
		total_buffer_bytes_read += current_pass_bytes_read;
	}
	// Load our buffer into the sdl stream
	int rc = SDL_PutAudioStreamData(bgm->Stream, bgm->Buffer, total_buffer_bytes_read);
	if (rc == -1) {
		printf("Uhoh, failed to put samples in stream: %s\n", SDL_GetError());
		return;
	}
	// If we looped, we already set where our playback is, else add all of our bytes to it
	if (!isLooped) {
		bgm->CurrentLoopBytesRead += total_buffer_bytes_read;
	}
}

sgBgm *sgBgmNew(void) {
	sgBgm *bgm = SDL_malloc(sizeof(*bgm));
	bgm->VorbisFile = SDL_malloc(sizeof(*bgm->VorbisFile));
	bgm->Filename = NULL;
	bgm->Buffer = SDL_malloc(BGM_BUFFER_SIZE);
	bgm->LoopStart = bgm->LoopEnd = bgm->Loops = bgm->CurrentLoopBytesRead = 0;
	bgm->CanPlay = bgm->IsPlaying = false;
	bgm->Volume = 1.0f;
	return bgm;
}

void sgBgmLoad(sgBgm *bgm) {
	int result = ov_fopen(bgm->Filename, bgm->VorbisFile);
	if (result != 0) {
		SDL_LogError(0, "Could not open audio in %s: %d\n", bgm->Filename, result);
		return;
	}
	bgm->VorbisInfo = ov_info(bgm->VorbisFile, -1);
	setBgmLoopPoints(bgm);
	const SDL_AudioSpec srcspec = {SDL_AUDIO_S16LE, bgm->VorbisInfo->channels, bgm->VorbisInfo->rate};
	bgm->Stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &srcspec, NULL, NULL);
	if (bgm->Stream == NULL) {
		sgLogWarn("Stream failed to create: %s\n", SDL_GetError());
		return;
	}
	bgm->CanPlay = true;
	while (SDL_GetAudioStreamAvailable(bgm->Stream) < MINIMUM_STREAM_SIZE) {
		loadDataToStream(bgm);
	}
}

void sgBgmPlay(sgBgm *bgm) {
	if (!bgm->CanPlay) {
		return;
	}
	bgm->IsPlaying = true;
	if (!SDL_ResumeAudioStreamDevice(bgm->Stream)) {
		sgLogWarn("Could not resume audio stream %s", SDL_GetError());
	}
}

void sgBgmPause(sgBgm *bgm) {
	if (!bgm->IsPlaying) {
		return;
	}
	SDL_PauseAudioStreamDevice(bgm->Stream);
	bgm->IsPlaying = false;
}

void sgBgmStop(sgBgm *bgm) {
	SDL_PauseAudioStreamDevice(bgm->Stream);
	ov_pcm_seek_lap(bgm->VorbisFile, 0);
	SDL_ClearAudioStream(bgm->Stream);
	loadDataToStream(bgm);
	bgm->IsPlaying = false;
}

void sgBgmUpdate(sgBgm *bgm) {
	if (!bgm->IsPlaying || !bgm->CanPlay) {
		return;
	}
	while (SDL_GetAudioStreamAvailable(bgm->Stream) < MINIMUM_STREAM_SIZE) {
		loadDataToStream(bgm);
	}
}
void sgBgmDelete(sgBgm *bgm) {
	SDL_free(bgm->Buffer);
	SDL_free(bgm->Filename);
	ov_clear(bgm->VorbisFile);
	SDL_DestroyAudioStream(bgm->Stream);
	SDL_free(bgm);
}

void sgBgmUpdateVolume(sgBgm *bgm, float volume) {
	if (!bgm->Stream || volume > 1.0 || volume < 0) {
		return;
	}
	bgm->Volume = volume;
	SDL_SetAudioStreamGain(bgm->Stream, volume);
}
