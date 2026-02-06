#include <SDL3/SDL_timer.h>
#include <Supergoon/log.h>
#include <Supergoon/tools.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void sgSleepMS(int ms) {
	SDL_Delay(ms);
}


uint64_t getCurrentNSTicks(){
	return SDL_GetTicksNS();
}

uint64_t getCurrentMSTicks(void) {
	return SDL_GetTicks();
}

int sgasprintf(char** strp, const char* fmt, ...) {
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
	char* buf = (char*)malloc(len + 1);
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

int sgstrncasecmp(const char* s1, const char* s2, size_t n) {
	if (n == 0) return 0;

	while (n-- != 0 && tolower(*s1) == tolower(*s2)) {
		if (n == 0 || *s1 == '\0' || *s2 == '\0')
			break;
		s1++;
		s2++;
	}

	return tolower(*(const unsigned char*)s1) - tolower(*(const unsigned char*)s2);
}

int strcmpWithSuffix(const char* lhs, const char* rhs, const char* suffix) {
	if (!lhs || !rhs || !suffix) return 0;

	size_t rhsLen = strlen(rhs);
	size_t suffixLen = strlen(suffix);

	// rhs must end with suffix
	if (rhsLen < suffixLen) return 0;
	if (strcmp(rhs + rhsLen - suffixLen, suffix) != 0) return 0;

	// find last '/'
	const char* base = strrchr(rhs, '/');
	base = base ? base + 1 : rhs;  // if no slash, whole string is basename

	// compute basename length without suffix
	size_t baseLen = strlen(base) - suffixLen;

	// lhs must match the basename (minus suffix)
	if (strlen(lhs) != baseLen) return 0;

	return strncmp(lhs, base, baseLen) == 0;
}
