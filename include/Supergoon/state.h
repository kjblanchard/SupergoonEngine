/**
 * @file state.h
 * @author Kevin (kevin@supergoon.com)
 * @brief Shared values that the engine uses for things, and could be useful to the end user as well
 * @version 0.1
 * @date 2025-03-17
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <stdint.h>
#pragma once

#ifdef __cplusplus
extern "C" {
#endif
extern float DeltaTimeSeconds;
extern float DeltaTimeMilliseconds;
extern uint32_t Ticks;
#ifdef __cplusplus
}
#endif
