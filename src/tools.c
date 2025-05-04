#include <SupergoonEngine/tools.h>

int sgasprintf(char **strp, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);

	// Copy args for size calculation
	va_list args_copy;
	va_copy(args_copy, args);

	// Get length of formatted string (excluding null terminator)
	int len = vsnprintf(NULL, 0, fmt, args_copy);
	va_end(args_copy);

	if (len < 0) {
		va_end(args);
		return -1;
	}

	// Allocate space (+1 for null terminator)
	char *buf = (char *)malloc(len + 1);
	if (!buf) {
		va_end(args);
		return -1;
	}

	// Write the formatted string
	vsnprintf(buf, len + 1, fmt, args);
	va_end(args);

	*strp = buf;
	return len;
}
