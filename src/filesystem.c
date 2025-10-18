// #include <SDL3/SDL_filesystem.h>
#include <Supergoon/filesystem.h>
#include <Supergoon/log.h>
#include <stdio.h>
#include <string.h>

#ifdef tui
#include <libgen.h>
#include <mach-o/dyld.h>
#include <unistd.h>
#endif

static char *_systemFilePath = NULL;

void GetFilenameWithPrefPathFilepath(char *buffer, size_t bufferSize, const char *filename) {
	snprintf(buffer, bufferSize, "./%s", filename);
}

static const char *getBaseExePath(void) {
#ifdef __APPLE__
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
	char *dir = dirname(path);
	_systemFilePath = strdup(dir);
	// OPTIONAL: ensure it ends with '/'
	if (_systemFilePath[strlen(_systemFilePath) - 1] != '/') {
		char *withSlash;
		asprintf(&withSlash, "%s/", _systemFilePath);
		free(_systemFilePath);
		_systemFilePath = withSlash;
	}
	return _systemFilePath;
}

void GetFilenameWithExeFilepath(char *buffer, size_t bufferSize, const char *filename) {
#endif
	const char *path = _systemFilePath ? _systemFilePath : getBaseExePath();
	if (snprintf(buffer, bufferSize, "%s%s", path, filename) >= (int)bufferSize) {
		sgLogWarn("Path is likely incorrect, buffer passed is not log enough for %s", filename);
	}
}
const char *GetBasePath(void) {
	return _systemFilePath ? _systemFilePath : getBaseExePath();
}

void shutdownEngineFilesystem(void) {
	free(_systemFilePath);
}
