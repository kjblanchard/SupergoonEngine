#pragma once
#include <Supergoon/log.h>
#include <string.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
#define SET_FLAG(v, f) ((v) |= (f))		// turn on bits
#define CLEAR_FLAG(v, f) ((v) &= ~(f))	// turn off bits
#define TOGGLE_FLAG(v, f) ((v) ^= (f))	// flip bits
// Check if bitset has any flags, use | to put them together
// (HAS_ANY_FLAGS(sprite->Flags, SpriteFlagUI | SpriteFlagHidden))
#define HAS_ANY_FLAGS(v, f) (((v) & (f)) != 0)
// Check if bitset has all flags
// if (HAS_ALL_FLAGS(sprite->Flags, SpriteFlagVisible | SpriteFlagUI))
#define HAS_ALL_FLAGS(v, f) (((v) & (f)) == (f))
// Check if bitset has no flag.
// (NO_FLAGS(sprite->Flags, SpriteFlagUI | SpriteFlagHidden))
#define NO_FLAGS(v, f) (((v) & (f)) == 0)

// Engine uses this as a "hole" in arrays
#define NO_HOLE ((size_t)(-1))
// Useful not on void ptrs, but on a ptr that size is known.
#define CLEAR_STRUCT(ptr) memset((ptr), 0, sizeof(*(ptr)))

// Pass array ptr, size of array, and type and will resize to double size, starts with 4 if it is 0
#define RESIZE_ARRAY(arr, count, size, type)                                                       \
	do {                                                                                           \
		if (count >= size) {                                                                       \
			size_t oldSize = (size);                                                               \
			size_t newSize = (size == 0) ? 4 : (size) * 2;                                         \
			type* newStorage = realloc((arr), newSize * sizeof(type));                             \
			if (newStorage) {                                                                      \
				memset(newStorage + oldSize, 0, (newSize - oldSize) * sizeof(type));               \
				(arr) = newStorage;                                                                \
				(size) = newSize;                                                                  \
			} else {                                                                               \
				sgLogError("Realloc failed when resizing array: %s, line %d", __FILE__, __LINE__); \
			}                                                                                      \
		}                                                                                          \
	} while (0)

// Resize an array if needed, and also allocate memory for each of the new items, uses calloc so new items are 0
#define RESIZE_ARRAY_PTR_ALLOC(arr, count, size, type) \
	do {                                               \
		if (count >= size) {                           \
			size_t oldSize = size;                     \
			RESIZE_ARRAY(arr, count, size, type*);     \
			for (size_t i = oldSize; i < size; i++) {  \
				arr[i] = calloc(1, sizeof(type));      \
			}                                          \
		}                                              \
	} while (0)

int sgasprintf(char** strp, const char* fmt, ...);

#ifdef _WIN32
// No native asprintf on Windows, use your own
#define asprintf sgasprintf
#define strncasecmp sgstrncasecmp
#endif

// Returns true if strings are equal
int strcmpWithSuffix(const char* lhs, const char* rhs, const char* suffix);
uint64_t getCurrentMSTicks();

void sgSleepMS(int ms);
#ifdef __cplusplus
}
#endif
