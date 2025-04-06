#pragma once
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

#ifdef __cplusplus
}
#endif
