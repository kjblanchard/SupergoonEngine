#pragma once
/**
 * @file debug.h
 * @author Kevin Blanchard (kevin@supergoon.com)
 * @brief
 * @version 0.1
 * @date 2023-07-15
 *
 * @copyright Copyright (c) 2023
 *
 */

/**
 * @brief The level that we should show debug events at.
 *
 */
#ifdef __cplusplus
extern "C" {
#endif
typedef enum sgLogLevel {
	Log_LDefault = 0,
	Log_LDebug = 1,
	Log_LInfo = 2,
	Log_LWarn = 3,
	Log_LError = 4,
	Log_LCritical = 5,
} sgLogLevel;
/**
 * @brief Opens and/or creates a file for debug logging.
 *
 * @return 1 if successful, 0 if failed.
 */
int sgInitializeDebugLogFile(void);
/**
 * @brief Closes the open file for logging.
 *
 * @return
 */
int sgCloseDebugLogFile(void);
/**
 * @brief Log a Debug log in a printf format
 *
 * @param format The printf styped text
 * @param ... The variables for the printf
 */
void sgLogDebug(const char *format, ...);
/**
 * @brief Log a warning log in a printf format
 * @param format The printf styped text
 * @param ... The variables for the printf
 */
void sgLogInfo(const char *format, ...);
/**
 * @brief Log a warning log in a printf format
 * @param format The printf styped text
 * @param ... The variables for the printf
 */
void sgLogWarn(const char *format, ...);
/**
 * @brief Log a error log in a printf format
 * @param format The printf styped text
 * @param ... The variables for the printf
 */
void sgLogError(const char *format, ...);
/**
 * @brief Log a critical log in a printf format, and then exit
 * @param format The printf styped text
 * @param ... The variables for the printf
 */
void sgLogCritical(const char *fmt, ...);

void sgSetDebugFunction(void (*)(const char *, const char *, int));

void sgSetLogLevel(int newLevel);

#ifdef __cplusplus
}
#endif
