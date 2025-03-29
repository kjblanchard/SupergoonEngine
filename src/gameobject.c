#include <Supergoon/gameobject.h>
#include <Supergoon/log.h>
#include <SupergoonEngine/gameobject.h>

size_t _numGameObjects = 0;
size_t _sizeGameObjects = 8;
GameObject* _gameObjects;
GameObjectType _gameObjectTypes[MAX_GAMEOBJECT_TYPES];
GameObject* CurrentGameObject = NULL;

static void resizeGameObjectArray(void) {
	size_t currentSize = _sizeGameObjects;
	size_t goalSize = currentSize * 2;
	GameObject* newStorage = realloc(_gameObjects, goalSize * sizeof(GameObject));
	if (newStorage) {
		_gameObjects = newStorage;
	} else {
		sgLogError("Somehow realloc failed when resizing objects array!");
		return;
	}
	_sizeGameObjects = goalSize;
}

void InitializeGameObjectSystem(void) {
	_gameObjects = calloc(_sizeGameObjects, sizeof(GameObject));
	GameObjectType _gameObjectTypes[MAX_GAMEOBJECT_TYPES];
	memset(_gameObjectTypes, 0, sizeof(_gameObjectTypes));
}

void GameObjectSystemUpdate(void) {
	for (size_t i = 0; i < _numGameObjects; i++) {
		if (_gameObjectTypes[_gameObjects->Type].UpdateFunc) {
			CurrentGameObject = &_gameObjects[i];
			_gameObjectTypes[_gameObjects->Type].UpdateFunc();
		}
	}
}

void ShutdownGameObjectSystem(void) {
	for (size_t i = 0; i < _sizeGameObjects; i++) {
		if (_gameObjects[i].Userdata) {
			SDL_free(_gameObjects[i].Userdata);
		}
	}
	SDL_free(_gameObjects);
}

void ObjectSetCreateFunction(int type, GameObjectCreateFunc func) {
	_gameObjectTypes[type].CreateFunc = func;
}

void ObjectSetStartFunction(int type, GameObjectStartFunc func) {
	_gameObjectTypes[type].StartFunc = func;
}

void ObjectSetUpdateFunction(int type, GameObjectUpdateFunc func) {
	_gameObjectTypes[type].UpdateFunc = func;
}
void ObjectSetDestroyFunction(int type, GameObjectDestroyFunc func) {
	_gameObjectTypes[type].DestroyFunc = func;
}
