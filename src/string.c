#include <Supergoon/string.h>
#include <stdio.h>
#include <string.h>

#define stringScratchBufferSize 4096
static char stringScratchBuffer[stringScratchBufferSize];
static size_t stringScratchLoc = 0;

String StringConcat(const char* lhs, const char* rhs) {
	size_t l1 = strlen(lhs);
	size_t l2 = strlen(rhs);
	size_t len = l1 + l2 + 1;
	if (stringScratchLoc + len > stringScratchBufferSize) {
		stringScratchLoc = 0;
	}
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
