#include <Supergoon/gameobject.h>
#include <Supergoon/log.h>
#include <Supergoon/map.h>
#include <SupergoonEngine/gameobject.h>
#include <SupergoonEngine/map.h>

size_t _firstGameObjectHole = 0;
size_t _currentId = 0;
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

static GameObject* getFreeGameObject(void) {
	if (!_firstGameObjectHole) {
		return &_gameObjects[_numGameObjects];
	}
	GameObject* returnGo = &_gameObjects[_firstGameObjectHole];
	size_t nextHole = 0;
	for (size_t i = _firstGameObjectHole + 1; i < _numGameObjects; i++) {
		if (!(_gameObjects[i].Flags & GameObjectFlagDestroyed)) {
			nextHole = i;
			break;
		}
	}
	_firstGameObjectHole = nextHole;
	return returnGo;
}

void AddGameObjectFromTiledMap(TiledObject* object) {
	if (_numGameObjects + 1 > _sizeGameObjects / 2) {
		resizeGameObjectArray();
	}
	CurrentGameObject = getFreeGameObject();
	CurrentGameObject->Id = _currentId++;
	CurrentGameObject->Type = object->ObjectType;
	CurrentGameObject->X = CurrentGameObject->Y = CurrentGameObject->W = CurrentGameObject->H = 0;
	CurrentGameObject->Userdata = NULL;
	sgLogWarn("Load GameObject from map");
	if (_gameObjectTypes[CurrentGameObject->Type].CreateFunc) {
		_gameObjectTypes[CurrentGameObject->Type].CreateFunc(object, CurrentGameObject);
	}
	CurrentGameObject->Flags |= GameObjectFlagActive;
	CurrentGameObject->Flags |= GameObjectFlagLoaded;
	++_numGameObjects;
}

void InitializeGameObjectSystem(void) {
	_gameObjects = calloc(_sizeGameObjects, sizeof(GameObject));
	GameObjectType _gameObjectTypes[MAX_GAMEOBJECT_TYPES];
	memset(_gameObjectTypes, 0, sizeof(_gameObjectTypes));
}

void GameObjectSystemUpdate(void) {
	for (size_t i = 0; i < _numGameObjects; i++) {
		CurrentGameObject = &_gameObjects[i];
		// Gameobject is not active, continue to next.
		if (!CurrentGameObject->Flags || !(CurrentGameObject->Flags & GameObjectFlagActive)) {
			continue;
		}
		if (!(CurrentGameObject->Flags & GameObjectFlagLoaded)) {
			sgLogWarn("Gameobject is not loaded from tiled, sending null is as userdata");
			if (_gameObjectTypes[CurrentGameObject->Type].CreateFunc) {
				_gameObjectTypes[CurrentGameObject->Type].CreateFunc(NULL, CurrentGameObject);
			}
			CurrentGameObject->Flags |= GameObjectFlagLoaded;
		}
		if (!(CurrentGameObject->Flags & GameObjectFlagStarted)) {
			if (_gameObjectTypes[CurrentGameObject->Type].StartFunc) {
				sgLogDebug("Starting gameobject function");
				_gameObjectTypes[CurrentGameObject->Type].StartFunc(CurrentGameObject);
			}
			CurrentGameObject->Flags |= GameObjectFlagStarted;
		}
		if (_gameObjectTypes[CurrentGameObject->Type].UpdateFunc) {
			_gameObjectTypes[CurrentGameObject->Type].UpdateFunc(CurrentGameObject);
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
