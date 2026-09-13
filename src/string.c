#include <Supergoon/string.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define stringScratchBufferSize 4096
static char stringScratchBuffer[stringScratchBufferSize];
static size_t stringScratchLoc = 0;

static void checkLen(size_t l) {
	if (stringScratchLoc + l > stringScratchBufferSize) {
		stringScratchLoc = 0;
	}
}

__attribute__((format(printf, 1, 2)))
String StringSprintf(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	size_t available = stringScratchBufferSize - stringScratchLoc;
	char* dest = stringScratchBuffer + stringScratchLoc;
	va_list copy;
	va_copy(copy, args);
	int len = vsnprintf(dest, available, fmt, copy);
	va_end(copy);
	if (len < 0) {
		va_end(args);
		return (String){"<format error>\n", 15};
	}
	if ((size_t)len + 1 > available) {
		// Try again at the beginning of the scratch buffer.
		if ((size_t)len + 1 > stringScratchBufferSize) {
			va_end(args);
			return (String){"<string too long>\n", 18};
		}
		stringScratchLoc = 0;
		dest = stringScratchBuffer;
		vsnprintf(dest, stringScratchBufferSize, fmt, args);
	}
	va_end(args);
	String s = {dest, (size_t)len};
	stringScratchLoc += (size_t)len + 1;
	return s;
}

String StringConcat(const char* lhs, const char* rhs) {
	size_t l1 = strlen(lhs);
	size_t l2 = strlen(rhs);
	size_t len = l1 + l2 + 1;
	checkLen(len);
	char* loc = stringScratchBuffer + stringScratchLoc;
	memcpy(loc, lhs, l1);
	memcpy(loc + l1, rhs, l2);
	loc[l1 + l2] = '\0';
	stringScratchLoc += len;
	return (String){loc, len - 1};
}

String StringSConcat(String lhs, const char* rhs) {
	return StringConcat(lhs.Data, rhs);
}
