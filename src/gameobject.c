#include <Supergoon/gameobject.h>
#include <Supergoon/log.h>
#include <Supergoon/map.h>
#include <Supergoon/tools.h>
#include <stdlib.h>
#include <string.h>

static size_t _currentId = 1;
static size_t _firstGameObjectHole = NO_HOLE;
static size_t _numGameObjects;
static size_t _sizeGameObjects;
static GameObject** _gameObjects;

static GameObject* getFreeGameObject(void) {
	// If there are no holes, grab from the end of the array
	if (_firstGameObjectHole == NO_HOLE) {
		RESIZE_ARRAY_PTR_ALLOC(_gameObjects, _numGameObjects, _sizeGameObjects, GameObject);
		GameObject* go = _gameObjects[_numGameObjects];
		go->Num = _numGameObjects;
		++_numGameObjects;
		return go;
	}
	GameObject* returnGo = _gameObjects[_firstGameObjectHole];
	returnGo->Num = _firstGameObjectHole;
	size_t holeIndex = _firstGameObjectHole;
	if (holeIndex == _numGameObjects - 1) {
		++_firstGameObjectHole;
		++_numGameObjects;
		if (_numGameObjects > _sizeGameObjects) {
			_firstGameObjectHole = NO_HOLE;
		}
		return returnGo;
	} else {
		size_t nextHole = NO_HOLE;
		for (size_t i = _firstGameObjectHole + 1; i < _numGameObjects; i++) {
			if (_gameObjects[i]->Id == 0) {
				nextHole = i;
				break;
			}
		}
		_firstGameObjectHole = nextHole;
		return returnGo;
	}
}

GameObject* GameObjectCreate(void) {
	GameObject* newGameObject = getFreeGameObject();
	newGameObject->Id = _currentId++;
	newGameObject->X = newGameObject->Y = 0;
	return newGameObject;
}

void GameObjectDestroy(GameObject* gameobject) {
	gameobject->Id = 0;
	gameobject->X = 0;
	gameobject->Y = 0;
	if (_firstGameObjectHole == NO_HOLE || gameobject->Num < _firstGameObjectHole) {
		_firstGameObjectHole = gameobject->Num;
	}
}

void ShutdownGameObjectSystem(void) {
	free(_gameObjects);
}
