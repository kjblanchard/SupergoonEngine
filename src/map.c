#include <SDL3/SDL.h>
#include <Supergoon/Graphics/shader.h>
#include <Supergoon/Graphics/texture.h>
#include <Supergoon/camera.h>
#include <Supergoon/filesystem.h>
#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <Supergoon/map.h>
#include <Supergoon/state.h>
#include <Supergoon/tools.h>
#include <Supergoon/window.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

const uint8_t NUM_WALLS = 4;

static void handleTiledLayerGroup(Tilemap *map);
static void handleTiledObjectGroup(Tilemap *map);
// Get the rect for gid, used when determining the src rect of a gid.
static void GetRectForGid(int gid, Tileset *tileset, RectangleF *rect);
Tilemap *_currentMap = NULL;
#define MAX_PREVIOUS_MAPS_CACHE 2
static Tilemap *_previousMaps[MAX_PREVIOUS_MAPS_CACHE] = {NULL};
// BG texture below the character
static Texture *_bg1Texture = NULL;
// BG texture above the character
static Texture *_bg2Texture = NULL;

static void createAnimatedTiles(Tilemap *map, Tileset *tileset) {
	LuaGetTable(_luaState, "tiles");
	tileset->NumAnimatedTiles = LuaGetTableLength(_luaState);
	tileset->AnimatedTiles =
		calloc(tileset->NumAnimatedTiles, sizeof(AnimatedTile));
	for (size_t j = 0; j < tileset->NumAnimatedTiles; j++) {
		LuaPushTableObjectToStacki(_luaState,
								   j);	// Actual animated tile is on there now
		AnimatedTile *animatedTile = &tileset->AnimatedTiles[j];
		animatedTile->GID = LuaGetInt(_luaState, "id") +
							tileset->FirstGid;	// GID is the local ID plus the first
												// gid, since it's global
		LuaGetTable(_luaState, "animation");
		animatedTile->NumFrames = LuaGetTableLength(_luaState);
		animatedTile->TileFrames =
			calloc(animatedTile->NumFrames, sizeof(TileAnimationFrame));
		for (size_t k = 0; k < animatedTile->NumFrames; k++) {
			LuaPushTableObjectToStacki(
				_luaState, k);	// Actual animated tile table is on there now
			TileAnimationFrame *frameData = &animatedTile->TileFrames[k];
			frameData->MsTime = LuaGetInt(_luaState, "duration");
			frameData->Id = LuaGetInt(_luaState, "tileid") +
							tileset->FirstGid;	// GID is the local ID plus the first
												// gid, since it's global
			GetRectForGid(frameData->Id, tileset, &frameData->SrcRect);
			LuaPopStack(_luaState, 1);	// remove animated tile frame table from stack
		}
		LuaPopStack(_luaState, 1);	// remove tileAnimation table from stack
		LuaPopStack(_luaState, 1);	// remove animated tile table from stack
	}
	LuaPopStack(_luaState, 1);	// remove tiles table from stack
}

static void createTilesets(Tilemap *map) {
	LuaGetTable(_luaState, "tilesets");
	map->NumTilesets = LuaGetTableLength(_luaState);
	map->Tilesets = calloc(map->NumTilesets, sizeof(Tileset));
	for (int i = 0; i < map->NumTilesets; i++) {
		Tileset *tileset = &map->Tilesets[i];
		LuaPushTableObjectToStacki(_luaState, i);
		tileset->Name = LuaAllocateString(_luaState, "name");
		tileset->FirstGid = LuaGetInt(_luaState, "firstgid");
		tileset->TileWidth = LuaGetInt(_luaState, "tilewidth");
		tileset->TileHeight = LuaGetInt(_luaState, "tileheight");
		tileset->Image = LuaAllocateString(_luaState, "image");
		tileset->ImageWidth = LuaGetInt(_luaState, "imagewidth");
		tileset->ImageHeight = LuaGetInt(_luaState, "imageheight");
		createAnimatedTiles(map, tileset);
		LuaPopStack(_luaState, 1);	// remove tileset table from stack
	}
	LuaPopStack(_luaState, 1);	// remove tilesets table from stack
}

static void createLayers(Tilemap *map) {
	LuaGetTable(_luaState, "layers");
	map->NumLayers = LuaGetTableLength(_luaState);
	for (int i = 0; i < map->NumLayers; i++) {
		LuaPushTableObjectToStacki(_luaState, i);
		const char *layerType = LuaGetString(_luaState, "type");
		if (strcmp(layerType, "objectgroup") == 0) {
			handleTiledObjectGroup(map);
		} else if (strcmp(layerType, "group") == 0) {
			handleTiledLayerGroup(map);
		}
		LuaPopStack(_luaState, 1);	// remove layer object table from stack
	}
	LuaPopStack(_luaState, 1);	// layers table
}

static Tilemap *parseTiledTilemap(const char *tiledFilename) {
	char *name;
	asprintf(&name, "assets/tiled/%s.lua", tiledFilename);
	LuaPushTableFromFile(_luaState, name);
	SDL_free(name);
	Tilemap *map = calloc(1, sizeof(*map));
	map->BaseFilename = strdup(tiledFilename);
	map->Width = LuaGetInt(_luaState, "width");
	map->Height = LuaGetInt(_luaState, "height");
	map->TileWidth = LuaGetInt(_luaState, "tilewidth");
	map->TileHeight = LuaGetInt(_luaState, "tileheight");
	createTilesets(map);
	createLayers(map);
	LuaPopStack(_luaState, 1);	// tiled map table
	LuaClearStack(_luaState);	// for good measure :)
	return map;
}

static void createTileLayer(TileLayer *layer) {
	layer->Width = LuaGetInt(_luaState, "width");
	layer->Height = LuaGetInt(_luaState, "height");
	int data_length = layer->Width * layer->Height;
	layer->Data = calloc(data_length, sizeof(int));
	LuaGetTable(_luaState, "data");
	for (int i = 0; i < data_length; i++) {
		layer->Data[i] = LuaGetIntFromTablei(_luaState, i);
	}
	LuaPopStack(_luaState, 1);
}

static void handleTiledLayerGroup(Tilemap *map) {
	const char *name = LuaGetString(_luaState, "name");
	int groupNum = (strcmp(name, "bg1") == 0) ? 0 : 1;
	LayerGroup *group = &map->LayerGroups[groupNum];
	group->Name = strdup(name);
	LuaGetTable(_luaState, "layers");
	group->NumLayers = LuaGetTableLength(_luaState);
	group->Layers = calloc(group->NumLayers, sizeof(TileLayer));
	for (size_t i = 0; i < (size_t)group->NumLayers; i++) {
		LuaPushTableObjectToStacki(_luaState, i);
		createTileLayer(&group->Layers[i]);
		LuaPopStack(_luaState, 1);
	}
	LuaPopStack(_luaState, 1);
}

static void loadTilesetTextures(Tilemap *map) {
	for (size_t i = 0; i < (size_t)map->NumTilesets; i++) {
		if (map->Tilesets[i].TilesetTexture != NULL) {
			continue;
		}
		int nameLen = strlen(map->Tilesets[i].Image);
		char *imageName = map->Tilesets[i].Image;
		// remove bmp extension if needed
		if (nameLen >= 4 && strcmp(imageName + nameLen - 4, ".bmp") == 0) {
			imageName[nameLen - 4] = '\0';
		}
		map->Tilesets[i].TilesetTexture = TextureCreate();
		TextureLoadFromBmp(map->Tilesets[i].TilesetTexture, map->Tilesets[i].Image);
	}
}

static TiledPropertyTypes getPropertyTypeForStack(void) {
	if (LuaIsInt(_luaState, -1)) {
		return TiledPropertyTypeInt;
	} else if (LuaIsFloat(_luaState, -1)) {
		return TiledPropertyTypeFloat;
	} else {
		return TiledPropertyTypeString;
	}
}

static void handleTiledObjectEntities(Tilemap *map) {
	LuaGetTable(_luaState, "objects");
	map->NumObjects = LuaGetTableLength(_luaState);
	map->Objects = calloc(map->NumObjects, sizeof(TiledObject));
	for (size_t i = 0; i < (size_t)map->NumObjects; i++) {
		TiledObject *object = &map->Objects[i];
		LuaPushTableObjectToStacki(_luaState, i);
		object->Id = LuaGetInt(_luaState, "id");
		object->ObjectType = atoi(LuaGetString(_luaState, "type"));
		object->X = LuaGetFloat(_luaState, "x");
		object->Y = LuaGetFloat(_luaState, "y");
		object->Width = LuaGetFloat(_luaState, "width");
		object->Height = LuaGetFloat(_luaState, "height");
		LuaGetTable(_luaState, "properties");
		object->NumProperties = LuaGetTableLengthMap(_luaState);
		if (object->NumProperties == 0) {
			LuaPopStack(_luaState, 2);
			continue;
		}
		object->Properties = calloc(object->NumProperties, sizeof(TiledProperty));
		LuaStartTableKeyValueIteration(_luaState);
		for (size_t j = 0; j < (size_t)object->NumProperties; j++) {
			if (!LuaNextTableKeyValueIterate(_luaState)) {
				break;
			}
			TiledProperty *property = &object->Properties[j];
			property->Name = LuaAllocateStringStack(_luaState, -2);
			property->PropertyType = getPropertyTypeForStack();
			if (property->PropertyType == TiledPropertyTypeInt) {
				property->Data.IntData = LuaGetIntFromStack(_luaState);
			} else if (property->PropertyType == TiledPropertyTypeFloat) {
				property->Data.FloatData = LuaGetFloatFromStack(_luaState);
			} else {
				property->Data.StringData = LuaAllocateStringStack(_luaState, -1);
			}
			LuaPopStack(_luaState, 1);
		}
		LuaEndTableKeyValueIteration(_luaState);
		LuaPopStack(_luaState, 2);
	}
	LuaPopStack(_luaState, 1);
}
static void handleTiledSolidObjects(Tilemap *map) {
	LuaGetTable(_luaState, "objects");
	map->NumSolids = LuaGetTableLength(_luaState);
	// We are also adding solids for the walls of the map
	map->Solids = calloc(map->NumSolids + NUM_WALLS, sizeof(RectangleF));
	for (size_t i = 0; i < (size_t)map->NumSolids; i++) {
		RectangleF *rect = &map->Solids[i];
		LuaPushTableObjectToStacki(_luaState, i);
		rect->x = LuaGetFloat(_luaState, "x");
		rect->y = LuaGetFloat(_luaState, "y");
		rect->w = LuaGetFloat(_luaState, "width");
		rect->h = LuaGetFloat(_luaState, "height");
		LuaPopStack(_luaState, 1);
	}
	LuaPopStack(_luaState, 1);
	const int boxSize = 16;
	int mapSizeX = map->Width * map->TileWidth;
	int mapSizeY = map->Height * map->TileHeight;
	RectangleF walls[] = {
		{0, -boxSize, mapSizeX, boxSize},
		{mapSizeX, 0, boxSize, mapSizeY},
		{0, mapSizeY, mapSizeX, boxSize},
		{-boxSize, 0, boxSize, mapSizeY},
	};
	memcpy(&map->Solids[map->NumSolids], walls, sizeof(walls));
	map->NumSolids += NUM_WALLS;
}

static void handleTiledObjectGroup(Tilemap *map) {
	const char *name = LuaGetString(_luaState, "name");
	if (strcmp(name, "entities") == 0) {
		handleTiledObjectEntities(map);
	} else if (strcmp(name, "solid") == 0) {
		handleTiledSolidObjects(map);
	}
}

static Tileset *GetTilesetForGID(int gid, Tilemap *map) {
	int highestGID = 0;
	Tileset *highestGIDTileset = NULL;
	for (size_t i = 0; i < (size_t)map->NumTilesets; i++) {
		if (gid >= map->Tilesets[i].FirstGid &&
			map->Tilesets[i].FirstGid >= highestGID) {
			highestGID = map->Tilesets[i].FirstGid;
			highestGIDTileset = &map->Tilesets[i];
		}
	}
	return highestGIDTileset;
}

static void GetRectForGid(int gid, Tileset *tileset, RectangleF *rect) {
	int tilemapGid = gid - tileset->FirstGid;
	int x = (tilemapGid % (tileset->ImageWidth / tileset->TileWidth)) *
			tileset->TileWidth;
	int y = (tilemapGid / (tileset->ImageWidth / tileset->TileWidth)) *
			tileset->TileHeight;
	rect->x = x;
	rect->y = y;
	rect->w = tileset->TileWidth;
	rect->h = tileset->TileHeight;
}

/**
 * @brief Get the Animated Tile For Gid object
 * @param gid gid to search for
 * @param tileset tileset to search in for animated tiles gid
 * @return AnimatedTile* the tile, or null if it isn't found
 */
static AnimatedTile *getAnimatedTileForGid(unsigned int gid, Tileset *tileset) {
	for (size_t i = 0; i < tileset->NumAnimatedTiles; i++) {
		if (gid == tileset->AnimatedTiles[i].GID) {
			return &tileset->AnimatedTiles[i];
		}
	}
	return NULL;
}

// Animated tiles do not get drawn to the map, instead they get added to the
// animated tile draw rectangles array.
static void handleAnimatedTile(Tilemap *map, Tileset *srcTileset,
							   RectangleF *dstRect,
							   AnimatedTile *animatedTile) {
	++animatedTile->NumDrawRectangles;
	RectangleF *newSpace =
		realloc(animatedTile->DrawRectangles,
				sizeof(RectangleF) * animatedTile->NumDrawRectangles);
	if (!newSpace) {
		sgLogError("Could not realloc, what in the world probably broken?");
		return;
	}
	animatedTile->DrawRectangles = newSpace;
	RectangleF *dstRectPtr =
		&animatedTile->DrawRectangles[animatedTile->NumDrawRectangles - 1];
	dstRectPtr->x = dstRect->x;
	dstRectPtr->y = dstRect->y;
	dstRectPtr->w = dstRect->w;
	dstRectPtr->h = dstRect->h;
}

// Called every time we load a map, as the bg1 and bg2 textures must be redrawn.
static void createBackgroundsFromTilemap(Tilemap *map) {
	int mapWidth = map->Width * map->TileWidth;
	int mapHeight = map->Height * map->TileHeight;
	if (_bg1Texture) {
		TextureDestroy(_bg1Texture);
	}
	//_bg1Texture = TextureCreate();
	_bg1Texture = TextureCreateRenderTarget(mapWidth, mapHeight);
	SetRenderTarget(_bg1Texture);
	loadTilesetTextures(map);
	LayerGroup *bg1LayerGroup = &map->LayerGroups[0];
	RectangleF dstRect = {0, 0, map->TileWidth, map->TileHeight};
	RectangleF srcRect = {0, 0, 0, 0};
	for (size_t i = 0; i < (size_t)bg1LayerGroup->NumLayers; i++) {
		TileLayer *layer = &bg1LayerGroup->Layers[i];
		for (int y = 0; y < layer->Height; ++y) {
			for (int x = 0; x < layer->Width; ++x) {
				int index = (y * layer->Width) + x;
				int tileGid = layer->Data[index];
				if (tileGid == 0)
					continue;
				Tileset *srcTileset = GetTilesetForGID(tileGid, map);
				dstRect.x = x * map->TileWidth;
				dstRect.y = y * map->TileHeight;
				AnimatedTile *animatedTile = getAnimatedTileForGid(tileGid, srcTileset);
				if (animatedTile) {
					handleAnimatedTile(map, srcTileset, &dstRect, animatedTile);
					continue;
				}
				GetRectForGid(tileGid, srcTileset, &srcRect);
				Texture *srcTexture = srcTileset->TilesetTexture;
				DrawTextureToTexture(_bg1Texture, srcTexture, GetDefaultShader(), &dstRect, &srcRect, 1.0f);
			}
		}
	}
	SetRenderTarget(NULL);
}

static void freeTiledTilemap(Tilemap *map) {
	for (size_t i = 0; i < 2; i++) {
		for (size_t j = 0; j < (size_t)map->LayerGroups[i].NumLayers; j++) {
			SDL_free(map->LayerGroups[i].Layers[j].Data);
		}
		SDL_free(map->LayerGroups[i].Name);
		SDL_free(map->LayerGroups[i].Layers);
	}
	if (map->NumObjects > 0) {
		for (size_t i = 0; i < (size_t)map->NumObjects; i++) {
			for (size_t j = 0; j < map->Objects[i].NumProperties; j++) {
				SDL_free(map->Objects[i].Properties[j].Name);
				if (map->Objects[i].Properties[j].PropertyType ==
					TiledPropertyTypeString) {
					SDL_free(map->Objects[i].Properties[j].Data.StringData);
				}
			}
			SDL_free(map->Objects[i].Name);
			SDL_free(map->Objects[i].Properties);
		}
		SDL_free(map->Objects);
	}
	// each tileset has a list of animated tiles, and also each of those has a
	// list of frames
	for (size_t i = 0; i < map->NumTilesets; i++) {
		Tileset *tileset = &map->Tilesets[i];
		for (size_t j = 0; j < tileset->NumAnimatedTiles; j++) {
			AnimatedTile *animatedTile = &tileset->AnimatedTiles[j];
			SDL_free(animatedTile->TileFrames);
			SDL_free(animatedTile->DrawRectangles);
		}
		SDL_free(tileset->Name);
		SDL_free(tileset->Image);
		SDL_free(tileset->AnimatedTiles);
		TextureDestroy(tileset->TilesetTexture);
	}
	if (map->Solids) {
		SDL_free(map->Solids);
	}
	SDL_free(map->Tilesets);
	SDL_free(map->BaseFilename);
	SDL_free(map);
	map = NULL;
}

/* static void drawAnimatedTiles(void) { */
/* 	// Update the animated tiles */
/* 	for (size_t i = 0; i < _currentMap->NumTilesets; i++) { */
/* 		Tileset *tileset = &_currentMap->Tilesets[i]; */
/* 		for (size_t j = 0; j < tileset->NumAnimatedTiles; j++) { */
/* 			AnimatedTile *animatedTile = &tileset->AnimatedTiles[j]; */
/* 			animatedTile->CurrentMSOnFrame += DeltaTimeMilliseconds; */
/* 			while (true) { */
/* 				TileAnimationFrame *currentFrame = */
/* 					&animatedTile->TileFrames[animatedTile->CurrentFrame]; */
/* 				if (animatedTile->CurrentMSOnFrame >= currentFrame->MsTime) { */
/* 					animatedTile->CurrentMSOnFrame -= currentFrame->MsTime; */
/* 					animatedTile->CurrentFrame = */
/* 						animatedTile->CurrentFrame + 1 >= animatedTile->NumFrames */
/* 							? 0 */
/* 							: animatedTile->CurrentFrame + 1; */
/* 				} else { */
/* 					break; */
/* 				} */
/* 			} */
/* 		} */
/* 	} */
/* 	// Draw the animated tiles */
/* 	for (size_t i = 0; i < _currentMap->NumTilesets; i++) { */
/* 		Tileset *tileset = &_currentMap->Tilesets[i]; */
/* 		for (size_t j = 0; j < tileset->NumAnimatedTiles; j++) { */
/* 			AnimatedTile *animatedTile = &tileset->AnimatedTiles[j]; */
/* 			for (size_t k = 0; k < animatedTile->NumDrawRectangles; k++) { */
/* 				RectangleF dst = animatedTile->DrawRectangles[k]; */
/* 				dst.x -= CameraX; */
/* 				dst.y -= CameraY; */
/* 				// DrawTexture( */
/* 				//     tileset->TilesetTexture, &dst, */
/* 				//     &animatedTile->TileFrames[animatedTile->CurrentFrame].SrcRect); */
/* 			} */
/* 		} */
/* 	} */
/* } */

void DrawCurrentMap(void) {
	if (!_currentMap)
		return;
	// RectangleF src = {CameraX, CameraY, _logicalWidth, _logicalHeight};
	// Draw the full screen size for now.
	/* int textureWidth = TextureGetWidth(_bg1Texture); */
	/* int textureHeight = TextureGetHeight(_bg1Texture); */
	// we should use the Cameras offset for the src rect here.
	RectangleF src = {CameraGetX(), CameraGetY(), WindowWidth(), WindowHeight()};
	RectangleF dst = {0, 0, WindowWidth(), WindowHeight()};
	if (_bg1Texture) {
		DrawTexture(_bg1Texture, GetDefaultShader(), &dst, &src, false, 1.0f, false);
	}
	if (_currentMap) {
		// drawAnimatedTiles();
	}
	if (_bg2Texture) {
		// DrawTexture(_bg2Texture, &dst, &src);
	}
}
// TODO this was ai.. probably need to check this and fix.
static Tilemap *checkCache(const char *mapName) {
	if (!_currentMap) {
		return NULL;
	}

	Tilemap *returnMap = NULL;
	int foundIndex = -1;
	unsigned int cacheSize = 0;

	// First pass: look for cache hit
	for (size_t i = 0; i < MAX_PREVIOUS_MAPS_CACHE; i++) {
		if (_previousMaps[i] == NULL)
			break;

		if (strcmp(mapName, _previousMaps[i]->BaseFilename) == 0) {
			returnMap = _previousMaps[i];
			foundIndex = i;
			break;
		}
	}

	// Second pass: determine actual cache size
	for (cacheSize = 0;
		 cacheSize < MAX_PREVIOUS_MAPS_CACHE && _previousMaps[cacheSize] != NULL;
		 ++cacheSize);

	// If cache hit, remove it from cache (shift others left)
	if (foundIndex >= 0) {
		for (size_t i = foundIndex; i < cacheSize - 1; i++) {
			_previousMaps[i] = _previousMaps[i + 1];
		}
		_previousMaps[cacheSize - 1] = NULL;
		// Caller is expected to assign this to _currentMap
		return returnMap;
	}

	// Cache current map if it is different from the map being loaded
	if (_currentMap && strcmp(mapName, _currentMap->BaseFilename) != 0) {
		// If cache is full, free the last entry
		if (cacheSize == MAX_PREVIOUS_MAPS_CACHE) {
			Tilemap *map = _previousMaps[cacheSize - 1];
			if (map) {
				freeTiledTilemap(map);
			}
			cacheSize--;  // Reduce cacheSize to make room
		}

		// Shift existing entries down to make space at index 0
		for (size_t i = cacheSize; i > 0; i--) {
			_previousMaps[i] = _previousMaps[i - 1];
		}
		_previousMaps[0] = _currentMap;
	} else {
		// If current map is already the one being loaded, just return it
		return _currentMap;
	}

	return returnMap;
}

void LoadMap(const char *mapName) {
	Tilemap *nextMap = checkCache(mapName);
	if (_currentMap && nextMap == _currentMap) {
		return;
	}
	if (!nextMap) {
		nextMap = parseTiledTilemap(mapName);
	}
	_currentMap = nextMap;
	createBackgroundsFromTilemap(_currentMap);
	SetCameraBounds(_currentMap->Width * _currentMap->TileWidth, _currentMap->Height * _currentMap->TileHeight);
}

void ShutdownMapSystem(void) {
	if (_bg1Texture) {
		TextureDestroy(_bg1Texture);
	}
	if (_bg2Texture) {
		TextureDestroy(_bg2Texture);
	}
	if (_currentMap) {
		freeTiledTilemap(_currentMap);
		_currentMap = NULL;
	}
	for (size_t i = 0; i < MAX_PREVIOUS_MAPS_CACHE; i++) {
		if (_previousMaps[i] == NULL) {
			break;
		}
		freeTiledTilemap(_previousMaps[i]);
		_previousMaps[i] = NULL;
	}
}

// void CheckGameobjectForCollisionWithSolids(GameObject *gameobject) {
//   SDL_FRect playerRect = {gameobject->X, gameobject->Y, gameobject->W,
//                           gameobject->H};
//   CheckRectForCollisionWithSolids(&playerRect);
// }
//
// void CheckRectForCollisionWithSolids(RectangleF *rect) {
//   for (int i = 0; i < _currentMap->NumSolids; i++) {
//     SDL_FRect solidRect = {_currentMap->Solids[i].x,
//     _currentMap->Solids[i].y,
//                            _currentMap->Solids[i].w,
//                            _currentMap->Solids[i].h};
//
//     if (SDL_HasRectIntersectionFloat(rect, &solidRect)) {
//       // Handle the collision: adjust player position
//       // This is just a simple axis-aligned response
//       RectResolveCollision(rect, &solidRect);
//     }
//   }
// }
