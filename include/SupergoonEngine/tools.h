#pragma once
#include <Supergoon/log.h>
#ifdef __cplusplus
extern "C" {
#endif

#define RESIZE_ARRAY(arr, count, type)                                                         \
	do {                                                                                       \
		size_t newSize = (count) * 2;                                                          \
		type* newStorage = realloc((arr), newSize * sizeof(type));                             \
		if (newStorage) {                                                                      \
			(arr) = newStorage;                                                                \
			(count) = newSize;                                                                 \
		} else {                                                                               \
			sgLogError("Realloc failed when resizing array: %s, line %d", __FILE__, __LINE__); \
		}                                                                                      \
	} while (0)

int sgasprintf(char** strp, const char* fmt, ...);

#ifdef _WIN32
// No native asprintf on Windows, use your own
#define asprintf sgasprintf
#endif

#ifdef __cplusplus
}
#endif
