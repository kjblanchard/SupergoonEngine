#pragma once
#include <Supergoon/log.h>
#ifdef __cplusplus
extern "C" {
#endif

// Pass array ptr, size of array, and type and will resize.
#define RESIZE_ARRAY(arr, size, type)                                                          \
	do {                                                                                       \
		size_t newSize = (size == 0) ? 4 : (size) * 2;                                         \
		type* newStorage = realloc((arr), newSize * sizeof(type));                             \
		if (newStorage) {                                                                      \
			(arr) = newStorage;                                                                \
			(size) = newSize;                                                                  \
		} else {                                                                               \
			sgLogError("Realloc failed when resizing array: %s, line %d", __FILE__, __LINE__); \
		}                                                                                      \
	} while (0)

// Resize array if capacity if count is over size
#define RESIZE_ARRAY_FULL(arr, count, size, type) \
	do {                                          \
		if (count >= size) {                      \
			RESIZE_ARRAY(arr, size, type);        \
		}                                         \
	} while (0)

int sgasprintf(char** strp, const char* fmt, ...);

#ifdef _WIN32
// No native asprintf on Windows, use your own
#define asprintf sgasprintf
#endif

// Returns true if strings are equal
int strcmpWithSuffix(const char* lhs, const char* rhs, const char* suffix);

#ifdef __cplusplus
}
#endif
