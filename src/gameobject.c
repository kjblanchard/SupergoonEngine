#include <Supergoon/gameobject.h>
#include <Supergoon/log.h>
#include <Supergoon/map.h>
#include <SupergoonEngine/gameobject.h>
#include <SupergoonEngine/map.h>
#include <SupergoonEngine/tools.h>
#include <stdlib.h>

size_t _currentId;
GameObjectType _gameObjectTypes[MAX_GAMEOBJECT_TYPES];
GameObject* CurrentGameObject;
size_t _firstGameObjectHole = NO_HOLE;
size_t _numGameObjects;
size_t _sizeGameObjects;
GameObject** _gameObjects;

static GameObject* getFreeGameObject(void) {
	// If there are no holes, grab from the end of the array
	if (_firstGameObjectHole == NO_HOLE) {
		RESIZE_ARRAY_PTR_ALLOC(_gameObjects, _numGameObjects, _sizeGameObjects, GameObject);
		return _gameObjects[_numGameObjects++];
	}
	// Get from hole and set next hole if there.
	GameObject* returnGo = _gameObjects[_firstGameObjectHole];
	size_t nextHole = NO_HOLE;
	for (size_t i = _firstGameObjectHole + 1; i < _numGameObjects; i++) {
		if ((_gameObjects[i]->Flags & GameObjectFlagDestroyed)) {
			nextHole = i;
			break;
		}
	}
	_firstGameObjectHole = nextHole;
	return returnGo;
}

void AddGameObjectFromTiledMap(TiledObject* object) {
	// If we don't have a create function for this object type, do not create it..
	if (!_gameObjectTypes[object->ObjectType].CreateFunc) {
		return;
	}
	CurrentGameObject = getFreeGameObject();
	CurrentGameObject->Id = _currentId++;
	CurrentGameObject->Type = object->ObjectType;
	CurrentGameObject->X = CurrentGameObject->Y = CurrentGameObject->W = CurrentGameObject->H = 0;
	CurrentGameObject->Userdata = NULL;
	if (_gameObjectTypes[CurrentGameObject->Type].CreateFunc) {
		sgLogDebug("create gameobject function");
		_gameObjectTypes[CurrentGameObject->Type].CreateFunc(object, CurrentGameObject);
	}
	CurrentGameObject->Flags = 0;
	CurrentGameObject->Flags |= GameObjectFlagActive;
	CurrentGameObject->Flags |= GameObjectFlagLoaded;
}

void GameObjectSystemUpdate(void) {
	for (size_t i = 0; i < _numGameObjects; i++) {
		CurrentGameObject = _gameObjects[i];
		CurrentGameObject->pX = CurrentGameObject->X;
		CurrentGameObject->pY = CurrentGameObject->Y;
		// Gameobject is not active, continue to next.
		if (!CurrentGameObject->Flags || !(CurrentGameObject->Flags & GameObjectFlagActive) || CurrentGameObject->Flags & GameObjectFlagDestroyed) {
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
	SetGameobjectsToBeDeleted(true);
	DestroyGameObjects();
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

void LoadGameObjects(void) {
	for (size_t i = 0; i < _numGameObjects; i++) {
		CurrentGameObject = _gameObjects[i];
		if (!(CurrentGameObject->Flags & GameObjectFlagLoaded)) {
			sgLogWarn("Gameobject is not loaded from tiled, sending null is as userdata");
			if (_gameObjectTypes[CurrentGameObject->Type].CreateFunc) {
				_gameObjectTypes[CurrentGameObject->Type].CreateFunc(NULL, CurrentGameObject);
			}
			CurrentGameObject->Flags |= GameObjectFlagLoaded;
		}
	}
}

// Starts all gameobjects, should be called after Load
void StartGameObjects(void) {
	for (size_t i = 0; i < _numGameObjects; i++) {
		CurrentGameObject = _gameObjects[i];
		if (!(CurrentGameObject->Flags & GameObjectFlagStarted)) {
			if (_gameObjectTypes[CurrentGameObject->Type].StartFunc) {
				sgLogDebug("Starting gameobject function");
				_gameObjectTypes[CurrentGameObject->Type].StartFunc(CurrentGameObject);
			}
			CurrentGameObject->Flags |= GameObjectFlagStarted;
		}
	}
}

// Destroys all gameobjects that are not donot destroy, unless force is set
void DestroyGameObjects(void) {
	for (size_t i = 0; i < _numGameObjects; i++) {
		if (!(_gameObjects[i]->Flags & GameObjectFlagToBeDestroyed)) {
			continue;
		}
		if (_gameObjectTypes[_gameObjects[i]->Type].DestroyFunc) {
			_gameObjectTypes[_gameObjects[i]->Type].DestroyFunc(_gameObjects[i]);
		}
		if (_gameObjects[i]->Userdata) {
			SDL_free(_gameObjects[i]->Userdata);
			_gameObjects[i]->Userdata = NULL;
		}
		if (_firstGameObjectHole == NO_HOLE || i < _firstGameObjectHole) {
			_firstGameObjectHole = i;
		}
		_gameObjects[i]->Flags = GameObjectFlagDestroyed;  // Set flag to only be destroyed
	}
}

void SetGameobjectsToBeDeleted(int forceDestroy) {
	for (size_t i = 0; i < _numGameObjects; i++) {
		if (_gameObjects[i]->Flags & GameObjectFlagDestroyed || (_gameObjects[i]->Flags & GameObjectFlagDoNotDestroy && !forceDestroy)) {
			continue;
		}
		_gameObjects[i]->Flags |= GameObjectFlagToBeDestroyed;
	}
}
