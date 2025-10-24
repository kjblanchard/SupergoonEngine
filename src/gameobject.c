#include <Supergoon/gameobject.h>
#include <Supergoon/log.h>
#include <Supergoon/map.h>
#include <Supergoon/tools.h>
#include <stdlib.h>

static size_t _currentId = 0;
static size_t _numGameObjects = 0;
static size_t _sizeGameObjects = 0;
static GameObject* _nextFreeGameObject = NULL;
static GameObject** _gameObjects = NULL;

static GameObject* getFreeGameObject() {
	// If there is no free gameobject, we need to grow the array properly.
	if (_nextFreeGameObject == NULL) {
		RESIZE_ARRAY(_gameObjects, _numGameObjects, _sizeGameObjects, GameObject*);
	}

	GameObject* returnGameobject = _nextFreeGameObject;

	return returnGameobject;
}

static GameObject* GameObjectCreate(void) {
	GameObject* newGameObject = getFreeGameObject();
	newGameObject->Id = _currentId++;
	newGameObject->X = newGameObject->Y = 0;
	return newGameObject;
}

static void ShutdownGameObjectSystem(void) {
	free(_gameObjects);
}
