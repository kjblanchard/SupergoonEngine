#include <AL/al.h>
#include <AL/alc.h>
#include <Supergoon/Platform/openal/openalStream.h>
#include <Supergoon/filesystem.h>
#include <Supergoon/log.h>
#include <assert.h>
#include <stdlib.h>
#include <vorbis/vorbisfile.h>
#define MAX_SFX_STREAMS 1

static ALuint Buffers[MAX_SFX_STREAMS];
static ALuint Sources[MAX_SFX_STREAMS];

typedef struct Sfx {
	ALenum Format;
	ALsizei Size;
	long SampleRate;
	short *Data;

} Sfx;

Sfx sfx;

void InitializeSfxSystem(void) {
	alGenBuffers(MAX_SFX_STREAMS, Buffers);
	assert(alGetError() == AL_NO_ERROR && "Could not create buffers");
	alGenSources(MAX_SFX_STREAMS, Sources);
	assert(alGetError() == AL_NO_ERROR && "Could not create source");
	for (int i = 0; i < MAX_SFX_STREAMS; ++i) {
		alSource3f(Sources[i], AL_POSITION, 0, 0, -1);
		alSourcei(Sources[i], AL_SOURCE_RELATIVE, AL_TRUE);
		alSourcei(Sources[i], AL_ROLLOFF_FACTOR, 0);
		assert(alGetError() == AL_NO_ERROR && "Could not set source parameters");
	}
}

void LoadSfxFile(const char *filename) {
	vorbis_info *vbinfo;
	OggVorbis_File vbfile;
	char *fullPath = NULL;
	asprintf(&fullPath, "%sassets/audio/sfx/%s%s", GetBasePath(), filename, ".ogg");

	int result = ov_fopen(fullPath, &vbfile);
	if (result != 0) {
		sgLogError("Could not open audio in %s: %d", filename, result);
		return;
	}
	vbinfo = ov_info(&vbfile, -1);
	if (vbinfo->channels == 1) {
		sfx.Format = AL_FORMAT_MONO16;
	} else {
		sfx.Format = AL_FORMAT_STEREO16;
	}
	if (!sfx.Format) {
		sgLogError("Unsupported channel count: %d", vbinfo->channels);
		ov_clear(&vbfile);
		return;
	}
	sfx.SampleRate = vbinfo->rate;

	// Get the size of the file in pcm.
	sfx.Size = ov_pcm_total(&vbfile, -1) * vbinfo->channels * sizeof(short);
	sfx.Data = malloc(sfx.Size);
	int total_buffer_bytes_read = 0;
	int fully_loaded = 0;
	while (!fully_loaded) {
		int bytes_read = ov_read(&vbfile, (char *)sfx.Data + total_buffer_bytes_read, VORBIS_REQUEST_SIZE, 0, sizeof(short), 1, 0);
		total_buffer_bytes_read += bytes_read;
		if (bytes_read == 0)
			fully_loaded = 1;
	}
	ov_clear(&vbfile);
	free(fullPath);
}

void UpdateSfxSystem(void) {
	if (!sfx.Data) return;
	ALint processed_buffers;
	int processed_buffer_nums[MAX_SFX_STREAMS];
	int buffs_processed = 0;
	for (size_t i = 0; i < MAX_SFX_STREAMS; ++i) {
		ALuint buf_num = i;
		alGetSourcei(Sources[i], AL_BUFFERS_PROCESSED, &processed_buffers);
		if (alGetError() != AL_NO_ERROR) {
			sgLogError("Error checking source state");
			return;
		}
		while (processed_buffers > 0) {
			alSourceUnqueueBuffers(Sources[i], 1, &Buffers[i]);
			processed_buffer_nums[buffs_processed++] = buf_num;
			--processed_buffers;
		}
	}
}

void SfxPlayOneShot(const char *filename, float volume) {
	LoadSfxFile(filename);
	alSourceRewind(Sources[0]);
	alSourcei(Sources[0], AL_BUFFER, 0);
	alSourcef(Sources[0], AL_GAIN, volume);
	alBufferData(Buffers[0], sfx.Format, sfx.Data, sfx.Size, sfx.SampleRate);
	alSourceQueueBuffers(Sources[0], 1, &Buffers[0]);
	alSourcePlay(Sources[0]);
}
