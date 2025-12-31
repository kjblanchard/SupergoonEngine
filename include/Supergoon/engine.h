/**
 * @file engine.h
 * @author Kevin B (kevin@supergoon.com)
 * @brief Defines the SDL callbackfunctions, and the functions the game should bind to
 * @version 0.1
 * @date 2025-03-14
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <Supergoon/events.h>

/**
 * @brief Set the Start Function object
 *
 * @param startFunc Function to be called once when the game is starting
 */
void SetStartFunction(void (*startFunc)(void));
/**
 * @brief Set the Handle Event Function object
 *
 * @param eventFunc The function that should be called, this function should return 0 to continue,
 * 1 for quit, 2 for error quit
 */
void SetHandleEventFunction(int (*eventFunc)(void*));
/**
 * @brief Set the Input Function object, used in the update loop before any gameobject updates, etc
 *
 * @param updateFunc
 */
void SetInputFunction(void (*updateFunc)(void));
/**
 * @brief Set the Update Function object
 *
 * @param updateFunc Function to be called every frame
 */
void SetUpdateFunction(void (*updateFunc)(void));
/**
 * @brief Set the Draw Function object
 *
 * @param drawFunc Function to be called when drawing
 */
void SetDrawFunction(void (*drawFunc)(void));
void SetQuitFunction(void (*quitFunc)(void));

void Run(void);

extern void (*_startFunc)(void);
extern void (*_updateFunc)(void);
extern void (*_drawFunc)(void);
extern void (*_quitFunc)(void);
extern void (*_inputFunc)(void);
extern int (*_handleEventFunc)(void*);
