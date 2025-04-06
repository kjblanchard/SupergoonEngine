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
void AddGameObjectFromTiledMap(struct TiledObject* object);
void GameObjectSystemUpdate(void);
void ShutdownGameObjectSystem(void);
#ifdef __cplusplus
}
#endif
