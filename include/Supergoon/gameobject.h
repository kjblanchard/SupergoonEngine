#pragma once
#ifdef __cplusplus
extern "C" {
#endif

typedef enum GameObjectFlags {
	GameObjectFlagLoaded = 1 << 0,
	GameObjectFlagStarted = 1 << 1,
	GameObjectFlagActive = 1 << 2,
	GameObjectFlagDoNotDestroy = 1 << 3,
	GameObjectFlagDestroyed = 1 << 4,
	GameObjectFlagToBeDestroyed = 1 << 4,
} GameObjectFlags;

typedef struct GameObject {
	unsigned int Id;
	unsigned int Type;
	GameObjectFlags Flags;
	float pX;
	float pY;
	float X;
	float Y;
	float W;
	float H;
	void* Userdata;
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

#ifdef __cplusplus
}
#endif
