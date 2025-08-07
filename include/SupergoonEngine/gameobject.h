#pragma once
#include <Supergoon/gameobject.h>
#include <stdlib.h>
#ifdef __cplusplus
extern "C" {
#endif
extern size_t _currentId;
extern size_t _numGameObjects;
extern size_t _sizeGameObjects;
extern GameObject** _gameObjects;
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

// Runs the create function on the gameobject, uses the tiled object as the userdata to create func
void AddGameObjectFromTiledMap(struct TiledObject* object);
// For use with manual additions, creation outside of the tiled map.  Considers it loaded, but returns the actual gameobject that you can load yourself.
GameObject* AddGameObject(void);
// Update function for gameobject system, if the gameobject is not loaded, it should load it at runtime, also start it at runtime.  You should likely create and start during a load phase though, so proper order happens regularly.
void GameObjectSystemUpdate(void);
void ShutdownGameObjectSystem(void);
#ifdef __cplusplus
}
#endif
