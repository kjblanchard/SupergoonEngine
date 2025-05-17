#pragma once
#include <Supergoon/gameobject.h>
#ifdef __cplusplus
extern "C" {
#endif
#define MAX_GAMEOBJECT_TYPES 64
struct TiledObject;

/**
 * @brief Holds all of the functions for the current types.
 *
 */
typedef struct GameObjectType {
	GameObjectCreateFunc CreateFunc;
	GameObjectStartFunc StartFunc;
	GameObjectUpdateFunc UpdateFunc;
	GameObjectDestroyFunc DestroyFunc;
} GameObjectType;
/**
 * @brief Initializes the gameobject System
 *
 */
void InitializeGameObjectSystem(void);
// Runs the create function on the gameobject, uses the tiled object as the userdata to create func
void AddGameObjectFromTiledMap(struct TiledObject* object);
// Update function for gameobject system, if the gameobject is not loaded, it should load it at runtime, also start it at runtime.  You should likely create and start during a load phase though, so proper order happens regularly.
void GameObjectSystemUpdate(void);
void ShutdownGameObjectSystem(void);
#ifdef __cplusplus
}
#endif
