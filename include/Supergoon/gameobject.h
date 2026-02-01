#pragma once
#ifdef __cplusplus
extern "C" {
#endif

typedef struct sgGameObject {
	unsigned int Id;
	unsigned int Num;
	float X;
	float Y;
} sgGameObject;

sgGameObject* GameObjectCreate(void);
void GameObjectDestroy(sgGameObject* gameobject);
void ShutdownGameObjectSystem(void);

#ifdef __cplusplus
}
#endif
