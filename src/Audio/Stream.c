#include <SDL3/SDL_audio.h>
#include <Supergoon/Audio/Stream.h>
#include <Supergoon/log.h>

sgStream* sgStreamNew(void) {
	sgStream* stream = malloc(sizeof(*stream));
	const SDL_AudioSpec spec = {SDL_AUDIO_S16LE, 2, 4800};
	stream->stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
	if (!stream->stream) {
		sgLogWarn("Failed making stream, %s", SDL_GetError());
	}
	return stream;
}

bool sgStreamIsFinished(sgStream* stream) {
	return SDL_GetAudioStreamAvailable(stream->stream) == 0;
}

void sgStreamFree(sgStream* stream) {
	SDL_DestroyAudioStream(stream->stream);
	SDL_free(stream);
	stream = NULL;
}
