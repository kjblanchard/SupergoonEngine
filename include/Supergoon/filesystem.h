/**
 * @file filesystem.h
 * @author Kevin B (kevin@supergoon.com)
 * @brief Helpful tools for filesystem functions
 * @version 0.1
 * @date 2025-03-15
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include <stdbool.h>
#include <stdlib.h>

/**
 * @brief Fills buffer with the full bath of the binary ran.
 *
 * @param buffer
 * @param bufferSize
 * @param filename
 */
void GetFilenameWithExeFilepath(char* buffer, size_t bufferSize, const char* filename);
void GetFilenameWithPrefPathFilepath(char* buffer, size_t bufferSize, const char* filename);
const char* GetBasePath(void);
void shutdownEngineFilesystem(void);
