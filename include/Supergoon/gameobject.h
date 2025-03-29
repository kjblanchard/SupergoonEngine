#pragma once
#ifdef __cplusplus
extern "C" {
#endif
typedef void (*GameObjectCreateFunc)(void);
typedef void (*GameObjectStartFunc)(void);
typedef void (*GameObjectUpdateFunc)(void);
typedef void (*GameObjectDestroyFunc)(void);

typedef enum GameObjectFlags {
	GameObjectFlagStarted = 1 << 0,
	GameObjectFlagDoNotDestroy = 1 << 1,

} GameObjectFlags;

typedef struct GameObject {
	int Type;
	GameObjectFlags Flags;
	float X;
	float Y;
	float W;
	float H;
	void* Userdata;
} GameObject;

// GameObject set functions
void ObjectSetCreateFunction(int type, GameObjectCreateFunc func);
void ObjectSetStartFunction(int type, GameObjectStartFunc func);
void ObjectSetUpdateFunction(int type, GameObjectUpdateFunc func);
void ObjectSetDestroyFunction(int type, GameObjectDestroyFunc func);

#ifdef __cplusplus
}
#endif
