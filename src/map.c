#include <Supergoon/filesystem.h>
#include <Supergoon/graphics.h>
#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <Supergoon/map.h>
#include <Supergoon/state.h>
#include <SupergoonEngine/gameobject.h>
#include <SupergoonEngine/map.h>
#include <SupergoonEngine/tools.h>
#include <stdio.h>
#include <string.h>

// Frame data for tile animation
typedef struct TileAnimationFrame {
	unsigned int Id;
	RectangleF SrcRect;
	unsigned int MsTime;
} TileAnimationFrame;

// A animated tile from tiled. One instance of this per animated tile in tiled, and it will be updated each frame.
typedef struct AnimatedTile {
	// Global ID of the animated tile, if we get this then we know that we have to load a animated tile when creating the BG.
	unsigned int GID;
	// How many frames are in this animation
	unsigned int NumFrames;
	// All of the frames, use NumFrames for iterations
	TileAnimationFrame* TileFrames;
	// Current time on the animation
	unsigned int CurrentMSOnFrame;
	// The rectangles where to draw this animated tile, used in draw function
	RectangleF* DrawRectangles;
	unsigned int NumDrawRectangles;
	unsigned int CurrentFrame;
} AnimatedTile;

// Tiled tileset
typedef struct Tileset {
	char* Name;
	int FirstGid;
	int TileWidth;
	int TileHeight;
	char* Image;
	Texture* TilesetTexture;
	int ImageWidth;
	int ImageHeight;
	AnimatedTile* AnimatedTiles;
	unsigned int NumAnimatedTiles;
} Tileset;

// A tile layer in tiled.  Just has a bunch of ints for gids for tiles to draw, and a width and height of the layer.
typedef struct TileLayer {
	int Width;
	int Height;
	int* Data;	// Dynamically allocated array for tile IDs
} TileLayer;

// A tiled layer group, that contains a bunch of tile layers.  We use this for the map BG1 and BG1 groups.
typedef struct LayerGroup {
	char* Name;
	int NumLayers;
	TileLayer* Layers;
} LayerGroup;

// Tiled tilemap, contains the objects, Groups, etc
typedef struct Tilemap {
	int Width;
	int Height;
	int TileWidth;
	int TileHeight;
	int NumTilesets;
	Tileset* Tilesets;
	int NumObjects;
	struct TiledObject* Objects;
	int NumLayers;
	LayerGroup LayerGroups[2];
} Tilemap;

static void handleTiledLayerGroup(Tilemap* map);
static void handleTiledObjectGroup(Tilemap* map);
// Get the rect for gid, used when determining the src rect of a gid.
static void GetRectForGid(int gid, Tileset* tileset, RectangleF* rect);
// Current map that will be drawn, if it exists.
static Tilemap* _currentMap = NULL;
// BG texture below the character
static Texture* _bg1Texture = NULL;
// BG texture above the character
static Texture* _bg2Texture = NULL;
static TiledMapLoadFunction _beforeLoadFunc = NULL;
static TiledMapLoadFunction _afterLoadFunc = NULL;

static void createAnimatedTiles(Tilemap* map, Tileset* tileset) {
	LuaPushTableToStack("tiles");
	tileset->NumAnimatedTiles = LuaGetTableLength();
	tileset->AnimatedTiles = calloc(tileset->NumAnimatedTiles, sizeof(AnimatedTile));
	for (size_t j = 0; j < tileset->NumAnimatedTiles; j++) {
		LuaPushTableObjectToStacki(j);	// Actual animated tile is on there now
		AnimatedTile* animatedTile = &tileset->AnimatedTiles[j];
		animatedTile->GID = LuaGetInt("id") + tileset->FirstGid;  // GID is the local ID plus the first gid, since it's global
		LuaPushTableToStack("animation");
		animatedTile->NumFrames = LuaGetTableLength();
		animatedTile->TileFrames = calloc(animatedTile->NumFrames, sizeof(TileAnimationFrame));
		for (size_t k = 0; k < animatedTile->NumFrames; k++) {
			LuaPushTableObjectToStacki(k);	// Actual animated tile table is on there now
			TileAnimationFrame* frameData = &animatedTile->TileFrames[k];
			frameData->MsTime = LuaGetInt("duration");
			frameData->Id = LuaGetInt("tileid") + tileset->FirstGid;  // GID is the local ID plus the first gid, since it's global
			GetRectForGid(frameData->Id, tileset, &frameData->SrcRect);
			LuaPopStack(1);	 // remove animated tile frame table from stack
		}
		LuaPopStack(1);	 // remove tileAnimation table from stack
		LuaPopStack(1);	 // remove animated tile table from stack
	}
	LuaPopStack(1);	 // remove tiles table from stack
}

static void createTilesets(Tilemap* map) {
	LuaPushTableToStack("tilesets");
	map->NumTilesets = LuaGetTableLength();
	map->Tilesets = calloc(map->NumTilesets, sizeof(Tileset));
	for (int i = 0; i < map->NumTilesets; i++) {
		Tileset* tileset = &map->Tilesets[i];
		LuaPushTableObjectToStacki(i);
		tileset->Name = LuaAllocateString("name");
		tileset->FirstGid = LuaGetInt("firstgid");
		tileset->TileWidth = LuaGetInt("tilewidth");
		tileset->TileHeight = LuaGetInt("tileheight");
		tileset->Image = LuaAllocateString("image");
		tileset->ImageWidth = LuaGetInt("imagewidth");
		tileset->ImageHeight = LuaGetInt("imageheight");
		createAnimatedTiles(map, tileset);
		LuaPopStack(1);	 // remove tileset table from stack
	}
	LuaPopStack(1);	 // remove tilesets table from stack
}

static void createLayers(Tilemap* map) {
	LuaPushTableToStack("layers");
	map->NumLayers = LuaGetTableLength();
	for (int i = 0; i < map->NumLayers; i++) {
		LuaPushTableObjectToStacki(i);
		const char* layerType = LuaGetString("type");
		if (strcmp(layerType, "objectgroup") == 0) {
			handleTiledObjectGroup(map);
		} else if (strcmp(layerType, "group") == 0) {
			handleTiledLayerGroup(map);
		}
		LuaPopStack(1);	 // remove layer object table from stack
	}
	LuaPopStack(1);	 // layers table
}

static Tilemap* parseTiledTilemap(const char* tiledFilename) {
	char* name;
	asprintf(&name, "assets/tiled/%s.lua", tiledFilename);
	LuaPushTableFromFile(name);
	SDL_free(name);
	Tilemap* map = malloc(sizeof(*map));
	map->Width = LuaGetInt("width");
	map->Height = LuaGetInt("height");
	map->TileWidth = LuaGetInt("tilewidth");
	map->TileHeight = LuaGetInt("tileheight");
	createTilesets(map);
	createLayers(map);
	LuaPopStack(1);	  // tiled map table
	LuaClearStack();  // for good measure :)
	return map;
}

static void createTileLayer(TileLayer* layer) {
	layer->Width = LuaGetInt("width");
	layer->Height = LuaGetInt("height");
	int data_length = layer->Width * layer->Height;
	layer->Data = calloc(data_length, sizeof(int));
	LuaPushTableToStack("data");
	for (int i = 0; i < data_length; i++) {
		layer->Data[i] = LuaGetIntFromTablei(i);
	}
	LuaPopStack(1);
}

static void handleTiledLayerGroup(Tilemap* map) {
	const char* name = LuaGetString("name");
	int groupNum = (strcmp(name, "bg1") == 0) ? 0 : 1;
	LayerGroup* group = &map->LayerGroups[groupNum];
	group->Name = strdup(name);
	LuaPushTableToStack("layers");
	group->NumLayers = LuaGetTableLength();
	group->Layers = calloc(group->NumLayers, sizeof(TileLayer));
	for (size_t i = 0; i < (size_t)group->NumLayers; i++) {
		LuaPushTableObjectToStacki(i);
		createTileLayer(&group->Layers[i]);
		LuaPopStack(1);
	}
	LuaPopStack(1);
}

static void loadTilesetTextures(Tilemap* map) {
	for (size_t i = 0; i < (size_t)map->NumTilesets; i++) {
		int nameLen = strlen(map->Tilesets[i].Image);
		map->Tilesets[i].Image[nameLen - 4] = '\0';	 // Subtract .bmp (4) chars for easier loading with function below
		map->Tilesets[i].TilesetTexture = CreateTextureFromIndexedBMP(map->Tilesets[i].Image);
	}
}

static TiledPropertyTypes getPropertyTypeForStack(void) {
	if (LuaIsInt(-1)) {
		return TiledPropertyTypeInt;
	} else if (LuaIsFloat(-1)) {
		return TiledPropertyTypeFloat;
	} else {
		return TiledPropertyTypeString;
	}
}

static void handleTiledObjectEntities(Tilemap* map) {
	LuaPushTableToStack("objects");
	map->NumObjects = LuaGetTableLength();
	map->Objects = calloc(map->NumObjects, sizeof(TiledObject));
	for (size_t i = 0; i < (size_t)map->NumObjects; i++) {
		TiledObject* object = &map->Objects[i];
		LuaPushTableObjectToStacki(i);
		object->Id = LuaGetInt("id");
		object->Name = LuaAllocateString("name");
		object->ObjectType = atoi(LuaGetString("type"));
		object->X = LuaGetFloat("x");
		object->Y = LuaGetFloat("y");
		object->Width = LuaGetFloat("width");
		object->Height = LuaGetFloat("height");
		LuaPushTableToStack("properties");
		object->NumProperties = LuaGetTableLengthMap();
		if (object->NumProperties == 0) {
			LuaPopStack(2);
			continue;
		}
		object->Properties = calloc(object->NumProperties, sizeof(TiledProperty));
		LuaStartTableKeyValueIteration();
		for (size_t j = 0; j < (size_t)object->NumProperties; j++) {
			if (!LuaNextTableKeyValueIterate()) {
				break;
			}
			TiledProperty* property = &object->Properties[j];
			property->Name = LuaAllocateStringStack(-2);
			property->PropertyType = getPropertyTypeForStack();
			if (property->PropertyType == TiledPropertyTypeInt) {
				property->Data.IntData = LuaGetIntFromStack();
			} else if (property->PropertyType == TiledPropertyTypeFloat) {
				property->Data.FloatData = LuaGetFloatFromStack();
			} else {
				property->Data.StringData = LuaAllocateStringStack(-1);
			}
			LuaPopStack(1);
		}
		LuaEndTableKeyValueIteration();
		LuaPopStack(2);
	}
	LuaPopStack(1);
}

static void handleTiledObjectGroup(Tilemap* map) {
	const char* name = LuaGetString("name");
	if (strcmp(name, "entities") != 0) {
		return;
	}
	handleTiledObjectEntities(map);
}

static Tileset* GetTilesetForGID(int gid, Tilemap* map) {
	int highestGID = 0;
	Tileset* highestGIDTileset = NULL;
	for (size_t i = 0; i < (size_t)map->NumTilesets; i++) {
		if (gid >= map->Tilesets[i].FirstGid && map->Tilesets[i].FirstGid >= highestGID) {
			highestGID = map->Tilesets[i].FirstGid;
			highestGIDTileset = &map->Tilesets[i];
		}
	}
	return highestGIDTileset;
}

static void GetRectForGid(int gid, Tileset* tileset, RectangleF* rect) {
	int tilemapGid = gid - tileset->FirstGid;
	int x = (tilemapGid % (tileset->ImageWidth / tileset->TileWidth)) * tileset->TileWidth;
	int y = (tilemapGid / (tileset->ImageWidth / tileset->TileWidth)) * tileset->TileHeight;
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
static AnimatedTile* getAnimatedTileForGid(unsigned int gid, Tileset* tileset) {
	for (size_t i = 0; i < tileset->NumAnimatedTiles; i++) {
		if (gid == tileset->AnimatedTiles[i].GID) {
			return &tileset->AnimatedTiles[i];
		}
	}
	return NULL;
}

// Animated tiles do not get drawn to the map, instead they get added to the animated tile draw rectangles array.
static void handleAnimatedTile(Tilemap* map, Tileset* srcTileset, RectangleF* dstRect, AnimatedTile* animatedTile) {
	++animatedTile->NumDrawRectangles;
	RectangleF* newSpace = realloc(animatedTile->DrawRectangles, sizeof(RectangleF) * animatedTile->NumDrawRectangles);
	if (!newSpace) {
		sgLogError("Could not realloc, what in the world probably broken?");
		return;
	}
	animatedTile->DrawRectangles = newSpace;
	RectangleF* dstRectPtr = &animatedTile->DrawRectangles[animatedTile->NumDrawRectangles - 1];
	dstRectPtr->x = dstRect->x;
	dstRectPtr->y = dstRect->y;
	dstRectPtr->w = dstRect->w;
	dstRectPtr->h = dstRect->h;
}

static void createBackgroundsFromTilemap(Tilemap* map) {
	int mapWidth = map->Width * map->TileWidth;
	int mapHeight = map->Height * map->TileHeight;
	_bg1Texture = CreateRenderTargetTexture(mapWidth, mapHeight, (sgColor){255, 255, 255, 255});
	loadTilesetTextures(map);
	LayerGroup* bg1LayerGroup = &map->LayerGroups[0];
	RectangleF dstRect = {0, 0, map->TileWidth, map->TileHeight};
	RectangleF srcRect = {0, 0, 0, 0};
	for (size_t i = 0; i < (size_t)bg1LayerGroup->NumLayers; i++) {
		TileLayer* layer = &bg1LayerGroup->Layers[i];
		for (int y = 0; y < layer->Height; ++y) {
			for (int x = 0; x < layer->Width; ++x) {
				int index = (y * layer->Width) + x;
				int tileGid = layer->Data[index];
				if (tileGid == 0)
					continue;
				Tileset* srcTileset = GetTilesetForGID(tileGid, map);
				dstRect.x = x * map->TileWidth;
				dstRect.y = y * map->TileHeight;
				AnimatedTile* animatedTile = getAnimatedTileForGid(tileGid, srcTileset);
				if (animatedTile) {
					handleAnimatedTile(map, srcTileset, &dstRect, animatedTile);
					continue;
				}
				GetRectForGid(tileGid, srcTileset, &srcRect);
				Texture* srcTexture = srcTileset->TilesetTexture;
				DrawTextureToRenderTargetTexture(_bg1Texture, srcTexture, &dstRect, &srcRect);
			}
		}
	}
}

static void freeTiledTilemap(Tilemap* map) {
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
			}
			SDL_free(map->Objects[i].Name);
			SDL_free(map->Objects[i].Properties);
		}
		SDL_free(map->Objects);
	}
	// each tileset has a list of animated tiles, and also each of those has a list of frames
	for (size_t i = 0; i < map->NumTilesets; i++) {
		Tileset* tileset = &map->Tilesets[i];
		for (size_t j = 0; j < tileset->NumAnimatedTiles; j++) {
			AnimatedTile* animatedTile = &tileset->AnimatedTiles[j];
			SDL_free(animatedTile->TileFrames);
			SDL_free(animatedTile->DrawRectangles);
		}
		SDL_free(tileset->Name);
		SDL_free(tileset->Image);
		SDL_free(tileset->AnimatedTiles);
		UnloadTexture(tileset->TilesetTexture);
	}
	SDL_free(map->Tilesets);
	SDL_free(map);
}

static void drawAnimatedTiles(void) {
	// Update the animated tiles
	for (size_t i = 0; i < _currentMap->NumTilesets; i++) {
		Tileset* tileset = &_currentMap->Tilesets[i];
		for (size_t j = 0; j < tileset->NumAnimatedTiles; j++) {
			AnimatedTile* animatedTile = &tileset->AnimatedTiles[j];
			animatedTile->CurrentMSOnFrame += DeltaTimeMilliseconds;
			while (true) {
				TileAnimationFrame* currentFrame = &animatedTile->TileFrames[animatedTile->CurrentFrame];
				if (animatedTile->CurrentMSOnFrame >= currentFrame->MsTime) {
					animatedTile->CurrentMSOnFrame -= currentFrame->MsTime;
					animatedTile->CurrentFrame = animatedTile->CurrentFrame + 1 >= animatedTile->NumFrames ? 0 : animatedTile->CurrentFrame + 1;
				} else {
					break;
				}
			}
		}
	}
	// Draw the animated tiles
	for (size_t i = 0; i < _currentMap->NumTilesets; i++) {
		Tileset* tileset = &_currentMap->Tilesets[i];
		for (size_t j = 0; j < tileset->NumAnimatedTiles; j++) {
			AnimatedTile* animatedTile = &tileset->AnimatedTiles[j];
			for (size_t k = 0; k < animatedTile->NumDrawRectangles; k++) {
				DrawTexture(tileset->TilesetTexture, &animatedTile->DrawRectangles[k], &animatedTile->TileFrames[animatedTile->CurrentFrame].SrcRect);
			}
		}
	}
}

void drawCurrentMap(void) {
	// TODO, main camera? First we draw the background, use the main camera offset, currently set it to 0/0 and screensize.
	if (_bg1Texture) {
		RectangleF src = {0, 0, 480, 270};
		DrawTexture(_bg1Texture, &src, &src);
	}
	drawAnimatedTiles();
	if (_bg2Texture) {
		RectangleF src = {0, 0, 480, 270};
		DrawTexture(_bg1Texture, &src, &src);
	}
}

void LoadMap(const char* mapName) {
	// TODO Maybe we cache a few of these?  currently we reload every map change.
	if (_currentMap) {
		freeTiledTilemap(_currentMap);
		_currentMap = NULL;
	}
	_currentMap = parseTiledTilemap(mapName);
	createBackgroundsFromTilemap(_currentMap);
}

void LoadObjectsFromMap(void) {
	for (size_t i = 0; i < (size_t)_currentMap->NumObjects; i++) {
		AddGameObjectFromTiledMap(&_currentMap->Objects[i]);
	}
}

void SetTiledBeforeLoadFunction(TiledMapLoadFunction func) {
	if (!func) {
		sgLogWarn("Setting a bad load func for before load");
		return;
	}
	_beforeLoadFunc = func;
}

void SetTiledAfterLoadFunction(TiledMapLoadFunction func) {
	if (!func) {
		sgLogWarn("Setting a bad load func for after load");
		return;
	}
	_afterLoadFunc = func;
}

void shutdownMapSystem(void) {
	if (_bg1Texture) {
		UnloadTexture(_bg1Texture);
	}
	if (_bg2Texture) {
		UnloadTexture(_bg2Texture);
	}
	if (_currentMap) {
		freeTiledTilemap(_currentMap);
	}
}
