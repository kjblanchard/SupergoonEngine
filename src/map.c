#include <SDL3/SDL.h>
#include <Supergoon/Graphics/shader.h>
#include <Supergoon/Graphics/texture.h>
#include <Supergoon/Primitives/Color.h>
#include <Supergoon/Primitives/rectangle.h>
#include <Supergoon/camera.h>
#include <Supergoon/filesystem.h>
#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <Supergoon/map.h>
#include <Supergoon/state.h>
#include <Supergoon/tools.h>
#include <Supergoon/window.h>
#include <stdio.h>
#include <string.h>

const uint8_t NUM_WALLS = 4;
static void handleTiledLayerGroup(Tilemap *map);
static void handleTiledObjectGroup(Tilemap *map);
// Get the rect for gid, used when determining the src rect of a gid.
static void GetRectForGid(int gid, Tileset *tileset, RectangleF *rect);
Tilemap *_currentMap = NULL;
#define MAX_PREVIOUS_MAPS_CACHE 3
static Tilemap *_previousMaps[MAX_PREVIOUS_MAPS_CACHE] = {NULL};

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
		map->Tilesets[i].TilesetTexture = TextureCreate(map->Tilesets[i].Image);
		TextureLoadFromBmp(map->Tilesets[i].TilesetTexture, map->Tilesets[i].Image);
	}
}

static void handleTiledObjectEntities(Tilemap *map) {
	LuaGetTable(_luaState, "objects");
	LuaSetGlobal(_luaState, map->BaseFilename);
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
	map->BackgroundTexture = TextureCreateRenderTarget(mapWidth, mapHeight);
	SetRenderTarget(map->BackgroundTexture);
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
				DrawTextureToTexture(map->BackgroundTexture, srcTexture, GetDefaultShader(), &dstRect, &srcRect, 1.0f);
			}
		}
	}
	SetRenderTarget(NULL);
}

static void freeTiledTilemap(Tilemap *map) {
	sgLogWarn("chearing tiled map with name %s ", map->BaseFilename);
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
	LuaUnsetGlobal(_luaState, map->BaseFilename);
	SDL_free(map->BaseFilename);
	TextureDestroy(map->BackgroundTexture);
	SDL_free(map);
	map = NULL;
}

static void drawAnimatedTiles(void) {
	// Update the animated tiles
	for (size_t i = 0; i < _currentMap->NumTilesets; i++) {
		Tileset *tileset = &_currentMap->Tilesets[i];
		for (size_t j = 0; j < tileset->NumAnimatedTiles; j++) {
			AnimatedTile *animatedTile = &tileset->AnimatedTiles[j];
			animatedTile->CurrentMSOnFrame += DeltaTimeMilliseconds;
			while (true) {
				TileAnimationFrame *currentFrame =
					&animatedTile->TileFrames[animatedTile->CurrentFrame];
				if (animatedTile->CurrentMSOnFrame >= currentFrame->MsTime) {
					animatedTile->CurrentMSOnFrame -= currentFrame->MsTime;
					animatedTile->CurrentFrame =
						animatedTile->CurrentFrame + 1 >= animatedTile->NumFrames
							? 0
							: animatedTile->CurrentFrame + 1;
				} else {
					break;
				}
			}
		}
	}
	// Draw the animated tiles
	for (size_t i = 0; i < _currentMap->NumTilesets; i++) {
		Tileset *tileset = &_currentMap->Tilesets[i];
		for (size_t j = 0; j < tileset->NumAnimatedTiles; j++) {
			AnimatedTile *animatedTile = &tileset->AnimatedTiles[j];
			for (size_t k = 0; k < animatedTile->NumDrawRectangles; k++) {
				RectangleF dst = animatedTile->DrawRectangles[k];
				DrawTexture(tileset->TilesetTexture, GetDefaultShader(), &dst, &animatedTile->TileFrames[animatedTile->CurrentFrame].SrcRect, true, 1.0f, false, &(Color){255, 255, 255, 255});
			}
		}
	}
}

void DrawCurrentMap(void) {
	if (!_currentMap)
		return;
	float camWidth = CameraGetWidth();
	float camHeight = CameraGetHeight();
	camWidth = camWidth > WindowWidth() ? WindowWidth() : camWidth;
	camHeight = camHeight > WindowHeight() ? WindowHeight() : camHeight;
	RectangleF src = {CameraGetX(), CameraGetY(), camWidth, camHeight};
	RectangleF dst = {0, 0, camWidth, camHeight};
	/* RectangleF dst = {0, 0, WindowWidth(), WindowHeight()}; */
	Color color = {255, 255, 255, 255};
	DrawTexture(_currentMap->BackgroundTexture, GetDefaultShader(), &dst, &src, false, 1.0f, false, &color);
	if (_currentMap) {
		drawAnimatedTiles();
	}
}
static void moveToFront(Tilemap **arr, int idx) {
	if (idx <= 0) return;

	Tilemap *tmp = arr[idx];

	for (int i = idx; i > 0; --i) {
		arr[i] = arr[i - 1];
	}

	arr[0] = tmp;
}

static void insertAtFront(Tilemap **arr, Tilemap *map) {
	// find how many valid entries we have
	int count = 0;
	for (int i = 0; i < MAX_PREVIOUS_MAPS_CACHE; ++i) {
		if (arr[i])
			count++;
		else
			break;	// guaranteed no gaps
	}
	if (count == MAX_PREVIOUS_MAPS_CACHE) {
		// full → destroy last map
		freeTiledTilemap(arr[MAX_PREVIOUS_MAPS_CACHE - 1]);
		count--;  // effectively frees one slot
	}
	// shift everything right by one, up to count
	for (int i = count; i > 0; --i) {
		arr[i] = arr[i - 1];
	}
	arr[0] = map;
}

// We should keep a small cache of maps so that we don't always reload
// Order them by the last loaded one, and clear that one when needed, and always keep them sorted.
// _previousMaps[0] should always be the map that is loaded, then the most recently second loaded, and we should replace the last one
static Tilemap *checkCache(const char *mapName) {
	for (int i = 0; i < MAX_PREVIOUS_MAPS_CACHE; ++i) {
		Tilemap *t = _previousMaps[i];
		if (!t) break;	// no more valid entries

		if (strcmp(t->BaseFilename, mapName) == 0) {
			moveToFront(_previousMaps, i);
			return _previousMaps[0];
		}
	}
	return NULL;
}

void LoadMap(const char *mapName) {
	Tilemap *map = checkCache(mapName);
	if (!map) {
		// Not found — parse new map
		map = parseTiledTilemap(mapName);
		createBackgroundsFromTilemap(map);
		insertAtFront(_previousMaps, map);
	}
	_currentMap = map;
	SetCameraBounds(_currentMap->Width * _currentMap->TileWidth, _currentMap->Height * _currentMap->TileHeight);
	SetCameraSize(_currentMap->Width * _currentMap->TileWidth, _currentMap->Height * _currentMap->TileHeight);
}

void ShutdownMapSystem(void) {
	for (size_t i = 0; i < MAX_PREVIOUS_MAPS_CACHE; i++) {
		if (_previousMaps[i] == NULL) {
			break;
		}
		freeTiledTilemap(_previousMaps[i]);
		_previousMaps[i] = NULL;
	}
}

void CheckRectForCollisionWithSolids(RectangleF *rect) {
	for (int i = 0; i < _currentMap->NumSolids; i++) {
		RectangleF *solid = &_currentMap->Solids[i];
		RectangleF solidRect = {solid->x, solid->y, solid->w, solid->h};
		if (RectIsCollision(rect, &solidRect)) {
			RectResolveCollision(rect, &solidRect);
		}
	}
}
