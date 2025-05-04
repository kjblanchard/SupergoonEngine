/**
 * @file map.h
 * @author Kevin Blanchard (kevin@supergoon.com)
 * @brief Tiled wrapper.  Loads maps into two BG layers, load objects, handle animated tiles.
 * @version 0.1
 * @date 2025-05-04
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once
#include <Supergoon/graphics.h>
#ifdef __cplusplus
extern "C" {
#endif

// Draws the current map bg layers and animated tiles.
void drawCurrentMap(void);
// Used when resetting or closing the game, clears out any static info like a loaded map, etc.
void shutdownMapSystem(void);

#ifdef __cplusplus
}
#endif
