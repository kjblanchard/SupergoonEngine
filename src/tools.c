#include <Supergoon/tools.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef tui
#include <time.h>
#include <unistd.h>
#else
#include <SDL3/SDL.h>
#endif

void sgSleepMS(int ms) {
#ifdef tui
	usleep(ms * 1000);
#else
	SDL_Delay(ms);
#endif
}

uint64_t getCurrentMSTicks(void) {
#ifdef tui
	return clock() / (CLOCKS_PER_SEC / 1000);
#else
	return SDL_GetTicks();
#endif
}

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

// This was causing issues when there was "player1" and "player1Battler" in animations
// int strcmpWithSuffix(const char *lhs, const char *rhs, const char *suffix) {
// 	if (!lhs || !rhs) {
// 		sgLogInfo("Passed a null string into compare.. returning false");
// 		return false;
// 	}
// 	return strncmp(lhs, rhs, strlen(suffix)) == 0;
// }

// TODO this was vibe coded .. prolly make this better.
int strcmpWithSuffix(const char *lhs, const char *rhs, const char *suffix) {
	if (!lhs || !rhs) {
		sgLogInfo("Passed a null string into compare.. returning false");
		return 0;  // false
	}

	size_t lhsLen = strlen(lhs);
	size_t rhsLen = strlen(rhs);
	size_t suffixLen = strlen(suffix);

	// check suffix first
	if (lhsLen < suffixLen || rhsLen < suffixLen) return 0;

	if (strcmp(lhs + lhsLen - suffixLen, suffix) != 0) {
		return 0;
	}

	// compare strings without the suffix
	if (lhsLen != rhsLen) return 0;

	return strncmp(lhs, rhs, lhsLen - suffixLen) == 0;
}
