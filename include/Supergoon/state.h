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
#pragma once

struct Directory;
struct UIObject;

#ifdef __cplusplus
extern "C" {
#endif
// Seconds in this frame
extern float DeltaTimeSeconds;
// MS in this frame
extern float DeltaTimeMilliseconds;
// Directory used for loading from buffers
extern struct Directory* AssetDirectory;
#ifdef imgui
// used to draw the imgui root ui if it is set.
extern struct UIObject* RootUIObject;
#endif
#ifdef __cplusplus
}
#endif
