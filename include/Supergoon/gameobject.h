#pragma once
#include <stdlib.h>
#ifdef __cplusplus
extern "C" {
#endif
void ShutdownGameObjectSystem(void);

typedef struct GameObject {
	unsigned int Id;
	float X;
	float Y;
} GameObject;

extern size_t _currentId;
extern size_t _numGameObjects;
extern size_t _sizeGameObjects;
extern GameObject** _gameObjects;

GameObject* GameObjectCreate(void);

#ifdef __cplusplus
}
#endif
