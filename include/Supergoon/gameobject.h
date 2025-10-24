#pragma once
#include <stdlib.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct GameObject {
	unsigned int Id;
	unsigned int Num;
	float X;
	float Y;
} GameObject;

GameObject* GameObjectCreate(void);
void GameObjectDestroy(GameObject* gameobject);
void ShutdownGameObjectSystem(void);

#ifdef __cplusplus
}
#endif
