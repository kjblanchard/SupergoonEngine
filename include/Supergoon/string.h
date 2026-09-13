/**
 * @file string.h
 * @brief Simple string funcs
 * @author Kevin Blanchard
 * @version 0.1.0
 * @date 2026-09-12
 */
#pragma once
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Strings are always short lived, and use a scratch buffer, so they could be overwritten at any time.
 * Use a string copy func to get the char* if you need it somewhere.
 */
typedef struct String {
	char* Data;
	size_t Len;
} String;
/**
 * @brief Simple scratch sprintf
 * @param fmt format string
 * @param ... args for format string
 * @return  scratch string for use somewhere
 */
String StringSprintf(const char* fmt, ...);
String StringConcat(const char* lhs, const char* rhs);
String StringSConcat(String lhs, const char* rhs);

#ifdef __cplusplus
}
#endif
