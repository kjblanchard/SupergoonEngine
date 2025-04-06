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
} GameObjectFlags;

typedef struct GameObject {
	unsigned int Id;
	unsigned int Type;
	GameObjectFlags Flags;
	float X;
	float Y;
	float W;
	float H;
	void* Userdata;
} GameObject;

extern GameObject* CurrentGameObject;
#define GetCurrentGameObject(Type) ((Type*)CurrentGameObject.Userdata)
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

#ifdef __cplusplus
}
#endif
