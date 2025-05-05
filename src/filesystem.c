#include <SDL3/SDL_filesystem.h>
#include <Supergoon/filesystem.h>
#include <Supergoon/log.h>
#include <stdio.h>

static char *_systemFilePath = NULL;

void GetFilenameWithPrefPathFilepath(char *buffer, size_t bufferSize, const char *filename) {
	if (_systemFilePath != NULL) {
		snprintf(buffer, bufferSize, "%s%s", _systemFilePath, filename);
		return;
	}
	const char *basePath = SDL_GetPrefPath("Supergoon Games", "EscapeTheFate");

	if (basePath == NULL) {
		sgLogError("Somehow the basepath we got was null, returning current path");
		snprintf(buffer, bufferSize, "./%s", filename);
	} else {
		if (snprintf(buffer, bufferSize, "%s%s", basePath, filename) >= (int)bufferSize) {
			sgLogWarn("Path is likely incorrect, buffer passed is not log enough for %s", filename);
		}
		_systemFilePath = strdup(basePath);
		SDL_free((void *)basePath);
	}
}

void GetFilenameWithExeFilepath(char *buffer, size_t bufferSize, const char *filename) {
	const char *basePath = SDL_GetBasePath();
	if (snprintf(buffer, bufferSize, "%s%s", basePath, filename) >= (int)bufferSize) {
		sgLogWarn("Path is likely incorrect, buffer passed is not log enough for %s", filename);
	}
}

void shutdownEngineFilesystem(void) {
	SDL_free(_systemFilePath);
}
