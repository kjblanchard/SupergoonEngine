#pragma once
/**
 * @file debug.h
 * @author Kevin Blanchard (kevin@supergoon.com)
 * @brief Logging functions used for logging at specific levels, log to a file, etc.
 * @version 0.2
 * @date 2023-07-15
 * @copyright Copyright (c) 2023
 */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief The level that we should show debug events at.
 *
 */
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
int InitializeLogSystem(void);
/**
 * @brief Closes the open file for logging.
 *
 * @return
 */
int ShutdownLogSystem(void);
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
/**
 * @brief Sets a log function that will be called on every log function, usually used for outputting the logs somewhere else as well
 * @param Time string, Message string, Log level
 */
void sgSetDebugFunction(void (*)(const char *, const char *, int));
/**
 * @brief Sets the log level that we should use throughout the program.  If a log is this level or higher, it will be shown, defaults to Debug
 * @param newLevel
 */
void sgSetLogLevel(int newLevel);
#ifdef __cplusplus
}
#endif
