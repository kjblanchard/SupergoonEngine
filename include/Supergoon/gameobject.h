#pragma once
#include <stdlib.h>
#ifdef __cplusplus
extern "C" {
#endif
// Update function for gameobject system, if the gameobject is not loaded, it should load it at runtime, also start it at runtime.  You should likely create and start during a load phase though, so proper order happens regularly.
void GameObjectSystemUpdate(void);
void ShutdownGameObjectSystem(void);

typedef enum GameObjectFlags {
	GameObjectFlagLoaded = 1 << 0,
	GameObjectFlagStarted = 1 << 1,
	GameObjectFlagActive = 1 << 2,
	GameObjectFlagDoNotDestroy = 1 << 3,
	GameObjectFlagDestroyed = 1 << 4,
	GameObjectFlagToBeDestroyed = 1 << 5,
	GameObjectFlagDebugDraw = 1 << 6,
} GameObjectFlags;

typedef struct GameObject {
	unsigned int Id;
	unsigned int Type;
	unsigned int Flags;
	float X;
	float Y;
	float W;
	float H;
} GameObject;
extern GameObject* CurrentGameObject;
#define GetCurrentGameObjectDataCasted(Type) ((Type*)CurrentGameObject.Userdata)
// Userdata could be tiledmap, or anything really.
typedef void (*GameObjectCreateFunc)(void* userdata, GameObject* go);
typedef void (*GameObjectStartFunc)(GameObject* go);
typedef void (*GameObjectUpdateFunc)(GameObject* go);
typedef void (*GameObjectDestroyFunc)(GameObject* go);

// GameObject set functions
void ObjectSetCreateFunction(int type, GameObjectCreateFunc func);
void ObjectSetStartFunction(int type, GameObjectStartFunc func);
void ObjectSetUpdateFunction(int type, GameObjectUpdateFunc func);
void ObjectSetDestroyFunction(int type, GameObjectDestroyFunc func);

// Loads all gameObjects
void LoadGameObjects(void);
// Starts all gameobjects, should be called after Load
void StartGameObjects(void);
// Sets the "To BeDestroyed" flag on all gameobjects set to be destroyed.. so that they can be cleaned up during a loading screen.. useful to keep data like images, etc cached during loads.
void SetGameobjectsToBeDeleted(int forceDestroy);
// Destroys all gameobjects set to be destroyed
void DestroyGameObjects(void);

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

#ifdef __cplusplus
}
#endif
