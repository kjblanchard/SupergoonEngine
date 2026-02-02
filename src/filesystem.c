// #include <SDL3/SDL_filesystem.h>
#include <Supergoon/filesystem.h>
#include <Supergoon/log.h>
#include <stdio.h>
#include <string.h>
#ifndef sdlbackend
#include <libgen.h>
#include <mach-o/dyld.h>
#include <unistd.h>
#else
#include <SDL3/SDL_filesystem.h>
#endif

static char* _systemFilePath = NULL;
static char* _systemPrefPath = NULL;

static char* getPrefPath() {
#ifdef sdlbackend
	if (_systemPrefPath) return _systemPrefPath;
	_systemPrefPath = SDL_GetPrefPath("SupergoonGames", "EscapeTheFate");

#else
	snprintf(buffer, bufferSize, "./%s", filename);
#endif
	return NULL;
}

static const char* getBaseExePath(void) {
#ifndef sdlbackend
	char path[1024];
	uint32_t size = sizeof(path);
	if (_NSGetExecutablePath(path, &size) != 0) {
		sgLogError("Can not figure out executable path, returning current dir");
		_systemFilePath = strdup("./");
		return _systemFilePath;
	}
	// if (realpath(path, path) == NULL) {
	// 	_systemFilePath = strdup("./");
	// 	return _systemFilePath;
	// }
	// Use dirname() to strip executable name
	char* dir = dirname(path);
	_systemFilePath = strdup(dir);
	// OPTIONAL: ensure it ends with '/'
	if (_systemFilePath[strlen(_systemFilePath) - 1] != '/') {
		char* withSlash;
		asprintf(&withSlash, "%s/", _systemFilePath);
		free(_systemFilePath);
		_systemFilePath = withSlash;
	}
	return _systemFilePath;
#elif sdlbackend
	_systemFilePath = strdup(SDL_GetBasePath());
	return _systemFilePath;
#endif
}

void GetFilenameWithPrefFilepath(char* buffer, size_t bufferSize, const char* filename) {
	const char* path = _systemPrefPath ? _systemPrefPath : getPrefPath();
	if (snprintf(buffer, bufferSize, "%s%s", path, filename) >= (int)bufferSize) {
		sgLogWarn("PrefPath is likely incorrect, buffer passed is not log enough for %s", filename);
	}
}

void GetFilenameWithExeFilepath(char* buffer, size_t bufferSize, const char* filename) {
	const char* path = _systemFilePath ? _systemFilePath : getBaseExePath();
	if (snprintf(buffer, bufferSize, "%s%s", path, filename) >= (int)bufferSize) {
		sgLogWarn("Path is likely incorrect, buffer passed is not log enough for %s", filename);
	}
}
const char* GetBasePath(void) {
	return _systemFilePath ? _systemFilePath : getBaseExePath();
}

void ShutdownEngineSilesystem(void) {
	free(_systemFilePath);
}

char* GetContentOfFileString(const char* fullFilePath) {
	FILE* filePointer;
	filePointer = fopen(fullFilePath, "rb");
	if (!filePointer) {
		sgLogWarn("Could not open file for reading, returning null");
		return NULL;
	}
	fseek(filePointer, 0, SEEK_END);
	size_t fileSize = ftell(filePointer);
	fseek(filePointer, 0, SEEK_SET);
	char* stringBuffer = malloc(fileSize + 1);
	fread(stringBuffer, sizeof(char), fileSize, filePointer);
	stringBuffer[fileSize] = '\0';
	fclose(filePointer);
	return stringBuffer;
}
