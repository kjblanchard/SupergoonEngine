/**
 * @file string.h
 * @author Kevin B (kevin@supergoon.com)
 * @brief simple string handlers
 * @version 0.1
 * @date 2026-9-10
 *
 * @copyright Copyright (c) 2026
 *
 */

#pragma once
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

// Strings are always short lived, and use a scratch buffer, so they could be overwritten at any time.
// Use a string copy func to get the char* if you need it somewhere.
typedef struct String {
	char* Data;
	size_t Len;
} String;

String StringConcat(const char* lhs, const char* rhs);
String StringSConcat(String lhs, const char* rhs);

#ifdef __cplusplus
}
#endif
