#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_misc.h>
#include <Supergoon/Audio/Sfx.h>
#include <Supergoon/Audio/Stream.h>
#include <Supergoon/log.h>
#include <vorbis/vorbisfile.h>
#define VORBIS_REQUEST_SIZE 4096  // Size of vorbis requests, usually recommend to be 4096

Sfx* SfxNew(void) {
	Sfx* sfx = SDL_malloc(sizeof(*sfx));
	memset(sfx->Filename, 0, sizeof(sfx->Filename));
	// sfx->Filename = NULL;
	sfx->Buffer = NULL;
	sfx->BufferLength = 0;
	sfx->Volume = 1.0f;
	return sfx;
}

void SfxLoad(Sfx* sfx) {
	OggVorbis_File ovf;
	int result = ov_fopen(sfx->Filename, &ovf);
	if (result != 0) {
		sgLogError("Could not open audio in %s: %d\n", sfx->Filename, result);
		return;
	}
	vorbis_info* ovi = ov_info(&ovf, -1);
	sfx->BufferLength = ov_pcm_total(&ovf, -1) * ovi->channels * sizeof(short);
	sfx->Buffer = SDL_malloc(sfx->BufferLength);
	int totalBytesRead = 0;
	int fullyLoaded = false;
	while (!fullyLoaded) {
		int bytes_read = ov_read(&ovf, (char*)sfx->Buffer + totalBytesRead, VORBIS_REQUEST_SIZE, 0, sizeof(short), 1, 0);
		totalBytesRead += bytes_read;
		if (bytes_read == 0) {
			fullyLoaded = true;
		}
	}
	const SDL_AudioSpec srcspec = {SDL_AUDIO_S16LE, ovi->channels, ovi->rate};
	sfx->spec = srcspec;
	ov_clear(&ovf);
}

void SfxPlay(Sfx* sfx, sgStream* stream) {
	if (!stream || !sfx->Buffer) {
		sgLogWarn("Trying to play a sfx without setting a stream or loading properly: %s", sfx->Filename);
		return;
	}
	SDL_SetAudioStreamFormat(stream->stream, &sfx->spec, NULL);
	SDL_SetAudioStreamGain(stream->stream, sfx->Volume);
	bool result = SDL_PutAudioStreamData(stream->stream, sfx->Buffer, sfx->BufferLength);
	if (!result) {
		sgLogWarn("Could not put data into stream for %s: %s", sfx->Filename, SDL_GetError());
	}
	SDL_ResumeAudioStreamDevice(stream->stream);
}
// void SfxPlayOneShot(const char* filename, sgStream* stream) {
// 	Sfx* sfx = SfxNew();
// 	sfx->Filename = strdup(filename);
// 	SfxLoad(sfx);
// 	SfxPlay(sfx, stream);
// 	SfxDelete(sfx);
// }

void SfxDelete(Sfx* sfx) {
	// if (sfx->Filename) {
	// 	SDL_free(sfx->Filename);
	// }
	SDL_free(sfx->Buffer);
	// ov_clear(sfx->VorbisFile);
	// SDL_ClearAudioStream(sfx->Stream);
	SDL_free(sfx);
}

// void sgSfxUpdateVolume(Sfx* sfx, float volume) {
// 	if (!_sgSfxStream || volume > 1.0 || volume < 0) {
// 		return;
// 	}
// 	sfx->Volume = volume;
// 	SDL_SetAudioStreamGain(_sgSfxStream, volume);
// }
