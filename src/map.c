#include <Supergoon/filesystem.h>
#include <Supergoon/graphics.h>
#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <Supergoon/map.h>
#include <Supergoon/state.h>
#include <SupergoonEngine/gameobject.h>
#include <SupergoonEngine/map.h>
#include <stdio.h>
#include <string.h>

static void handleTiledLayerGroup(Tilemap* map);
static void handleTiledObjectGroup(Tilemap* map);
// Get the rect for gid, used when determining the src rect of a gid.
static void GetRectForGid(int gid, Tileset* tileset, RectangleF* rect);
// Current map that will be drawn, if it exists.
static Tilemap* _currentMap = NULL;
// BG texture below the character
static Texture* bg1Texture = NULL;
// BG texture above the character
static Texture* bg2Texture = NULL;
// // Animated tiles in the tilemap
// static AnimatedTile* _currentMapAnimatedTiles = NULL;
// static size_t _currentNumAnimatedTiles = 0;

Tilemap* parseTiledTilemap(const char* tiledFilename) {
	char name[50];
	snprintf(name, sizeof(name), "assets/tiled/%s.lua", tiledFilename);
	LuaPushTableFromFile(name);
	Tilemap* map = malloc(sizeof(*map));
	map->width = LuaGetInt("width");
	map->height = LuaGetInt("height");
	map->tilewidth = LuaGetInt("tilewidth");
	map->tileheight = LuaGetInt("tileheight");
	LuaPushTableToStack("tilesets");
	map->tileset_count = LuaGetTableLength();
	map->tilesets = calloc(map->tileset_count, sizeof(Tileset));
	for (int i = 0; i < map->tileset_count; i++) {
		Tileset* tileset = &map->tilesets[i];
		LuaPushTableObjectToStacki(i);
		LuaCopyString("name", map->tilesets[i].name, sizeof(map->tilesets[i].name));
		map->tilesets[i].firstgid = LuaGetInt("firstgid");
		map->tilesets[i].tilewidth = LuaGetInt("tilewidth");
		map->tilesets[i].tileheight = LuaGetInt("tileheight");
		LuaCopyString("image", map->tilesets[i].image, sizeof(map->tilesets[i].image));
		map->tilesets[i].imagewidth = LuaGetInt("imagewidth");
		map->tilesets[i].imageheight = LuaGetInt("imageheight");
		// Create all of the animated tiles.
		LuaPushTableToStack("tiles");
		tileset->numAnimatedTiles = LuaGetTableLength();
		tileset->animatedTiles = calloc(tileset->numAnimatedTiles, sizeof(AnimatedTile));
		for (size_t j = 0; j < tileset->numAnimatedTiles; j++) {
			LuaPushTableObjectToStacki(j);	// Actual animated tile is on there now
			AnimatedTile* animatedTile = &tileset->animatedTiles[j];
			animatedTile->gid = LuaGetInt("id") + tileset->firstgid;
			// // For some reason, gid on tiled is one higher than gid in tileset?
			// animatedTile->gid++;
			LuaPushTableToStack("animation");
			animatedTile->numTilesInAnimation = LuaGetTableLength();
			animatedTile->animatedTileFrameData = calloc(tileset->animatedTiles->numTilesInAnimation, sizeof(TileAnimationFrameData));
			for (size_t k = 0; k < tileset->animatedTiles->numTilesInAnimation; k++) {
				LuaPushTableObjectToStacki(k);	// Actual animated tile table is on there now
				TileAnimationFrameData* frameData = &animatedTile->animatedTileFrameData[k];
				frameData->msTime = LuaGetInt("duration");
				frameData->tileId = LuaGetInt("tileid");
				GetRectForGid(frameData->tileId, tileset, &frameData->srcRect);
				LuaPopStack(1);	 // remove animated tile frame table from stack
			}
			LuaPopStack(1);	 // remove tileAnimation table from stack
			LuaPopStack(1);	 // remove animated tile table from stack
		}

		LuaPopStack(1);	 // remove tiles table from stack
		LuaPopStack(1);	 // remove tileset table from stack
	}
	LuaPopStack(1);
	LuaPushTableToStack("layers");
	map->layer_count = LuaGetTableLength();
	for (int i = 0; i < map->layer_count; i++) {
		LuaPushTableObjectToStacki(i);
		const char* layerType = LuaGetString("type");
		if (strcmp(layerType, "objectgroup") == 0) {
			handleTiledObjectGroup(map);
		} else if (strcmp(layerType, "group") == 0) {
			handleTiledLayerGroup(map);
		}
		LuaPopStack(1);
	}
	LuaPopStack(2);	  // Layers and the actual table
	LuaClearStack();  // for good measure :)
	return map;
}

static void createTileLayer(TileLayer* layer) {
	layer->width = LuaGetInt("width");
	layer->height = LuaGetInt("height");
	int data_length = layer->width * layer->height;
	layer->data = calloc(data_length, sizeof(int));
	LuaPushTableToStack("data");
	for (int i = 0; i < data_length; i++) {
		layer->data[i] = LuaGetIntFromTablei(i);
	}
	LuaPopStack(1);
}

static void handleTiledLayerGroup(Tilemap* map) {
	const char* name = LuaGetString("name");
	int groupNum = (strcmp(name, "bg1") == 0) ? 0 : 1;
	LayerGroup* group = &map->groups[groupNum];
	strncpy(group->Name, name, sizeof(group->Name));
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
	for (size_t i = 0; i < (size_t)map->tileset_count; i++) {
		// Remove the .bmp from the name for easier loading with function
		int nameLen = strlen(map->tilesets[i].image);
		// Subtract .bmp (4) chars
		map->tilesets[i].image[nameLen - 4] = '\0';
		map->tilesets[i].tilesetTexture = CreateTextureFromIndexedBMP(map->tilesets[i].image);
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
	map->num_objects = LuaGetTableLength();
	map->objects = calloc(map->num_objects, sizeof(TiledObject));
	for (size_t i = 0; i < (size_t)map->num_objects; i++) {
		TiledObject* object = &map->objects[i];
		LuaPushTableObjectToStacki(i);
		object->Id = LuaGetInt("id");
		LuaCopyString("name", object->Name, sizeof(object->Name));
		object->ObjectType = atoi(LuaGetString("type"));
		object->X = LuaGetFloat("x");
		object->Y = LuaGetFloat("y");
		object->Width = LuaGetFloat("width");
		object->Height = LuaGetFloat("height");
		LuaPushTableToStack("properties");
		object->PropertyNum = LuaGetTableLengthMap();
		if (object->PropertyNum == 0) {
			LuaPopStack(2);
			continue;
		}
		object->Properties = calloc(object->PropertyNum, sizeof(TiledProperty));
		LuaStartTableKeyValueIteration();
		for (size_t j = 0; j < (size_t)object->PropertyNum; j++) {
			if (!LuaNextTableKeyValueIterate()) {
				break;
			}
			TiledProperty* property = &object->Properties[j];
			LuaCopyStringStack(-2, property->Name, sizeof(property->Name));
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
	for (size_t i = 0; i < (size_t)map->tileset_count; i++) {
		if (gid >= map->tilesets[i].firstgid && map->tilesets[i].firstgid >= highestGID) {
			highestGID = map->tilesets[i].firstgid;
			highestGIDTileset = &map->tilesets[i];
		}
	}
	return highestGIDTileset;
}

static void GetRectForGid(int gid, Tileset* tileset, RectangleF* rect) {
	int tilemapGid = gid - tileset->firstgid;
	int x = (tilemapGid % (tileset->imagewidth / tileset->tilewidth)) * tileset->tilewidth;
	int y = (tilemapGid / (tileset->imagewidth / tileset->tilewidth)) * tileset->tileheight;
	rect->x = x;
	rect->y = y;
	rect->w = tileset->tilewidth;
	rect->h = tileset->tileheight;
}

void createBackgroundsFromTilemap(Tilemap* map) {
	int mapWidth = map->width * map->tilewidth;
	int mapHeight = map->height * map->tileheight;
	bg1Texture = CreateRenderTargetTexture(mapWidth, mapHeight, (sgColor){255, 255, 255, 255});
	// Load the tileset textures so that we can use them to draw with.
	loadTilesetTextures(map);
	LayerGroup* bg1LayerGroup = &map->groups[0];
	RectangleF dstRect = {0, 0, map->tilewidth, map->tileheight};
	RectangleF srcRect = {0, 0, 0, 0};
	for (size_t i = 0; i < (size_t)bg1LayerGroup->NumLayers; i++) {
		TileLayer* layer = &bg1LayerGroup->Layers[i];
		for (int y = 0; y < layer->height; ++y) {
			for (int x = 0; x < layer->width; ++x) {
				int index = (y * layer->width) + x;
				int tileGid = layer->data[index];
				if (tileGid == 0)
					continue;
				Tileset* srcTileset = GetTilesetForGID(tileGid, map);
				// Handle animated tiles?
				bool isAnimatedTile = false;
				for (size_t k = 0; k < srcTileset->numAnimatedTiles; k++) {
					if (tileGid == srcTileset->animatedTiles[k].gid) {
						AnimatedTile* animatedTile = &srcTileset->animatedTiles[k];
						++animatedTile->currentNumTileInstances;
						void* newSpace = realloc(animatedTile->DrawRectangles, sizeof(RectangleF) * animatedTile->currentNumTileInstances);
						if (!newSpace) {
							sgLogError("Could not realloc, what in the world probably broken?");
							continue;
						}
						animatedTile->DrawRectangles = newSpace;
						RectangleF* dstRectPtr = &animatedTile->DrawRectangles[animatedTile->currentNumTileInstances - 1];
						dstRectPtr->x = x * map->tilewidth;
						dstRectPtr->y = y * map->tileheight;
						dstRectPtr->w = dstRect.w;
						dstRectPtr->h = dstRect.h;
						isAnimatedTile = true;
						break;
					}
				}
				if (isAnimatedTile) {
					continue;
				}
				GetRectForGid(tileGid, srcTileset, &srcRect);
				Texture* srcTexture = srcTileset->tilesetTexture;
				dstRect.x = x * map->tilewidth;
				dstRect.y = y * map->tileheight;
				DrawTextureToRenderTargetTexture(bg1Texture, srcTexture, &dstRect, &srcRect);
			}
		}
	}
}

void freeTiledTilemap(Tilemap* map) {
	for (size_t i = 0; i < 2; i++) {
		for (size_t j = 0; j < (size_t)map->groups[i].NumLayers; j++) {
			SDL_free(map->groups[i].Layers[j].data);
		}
		SDL_free(map->groups[i].Layers);
	}

	if (map->num_objects > 0) {
		for (size_t i = 0; i < (size_t)map->num_objects; i++) {
			SDL_free(map->objects[i].Properties);
		}
		SDL_free(map->objects);
	}
	// each tileset has a list of animated tiles, and also each of those has a list of frames
	for (size_t i = 0; i < map->tileset_count; i++) {
		Tileset* tileset = &map->tilesets[i];
		for (size_t j = 0; j < tileset->numAnimatedTiles; j++) {
			AnimatedTile* animatedTile = &tileset->animatedTiles[j];
			for (size_t k = 0; k < animatedTile->numTilesInAnimation; k++) {
				SDL_free(animatedTile->animatedTileFrameData);
			}
		}
		SDL_free(tileset->animatedTiles);
	}
	SDL_free(map->tilesets);
	SDL_free(map);
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
	for (size_t i = 0; i < (size_t)_currentMap->num_objects; i++) {
		AddGameObjectFromTiledMap(&_currentMap->objects[i]);
	}
}

static void drawAnimatedTiles(void) {
	// for (size_t i = 0; i < _currentNumAnimatedTiles; i++) {
	// 	AnimatedTile* tile = &_currentMapAnimatedTiles[i];
	// 	// tile->CurrentTime += DeltaTimeMilliseconds;
	// 	bool complete = false;
	// 	while (!complete) {
	// 		complete = true;
	// 	}
	// }
}

void drawCurrentMap(void) {
	// TODO, main camera? First we draw the background, use the main camera offset, currently set it to 0/0 and screensize.
	if (bg1Texture) {
		RectangleF src = {0, 0, 512, 288};
		DrawTexture(bg1Texture, &src, &src);
	}
	drawAnimatedTiles();
	if (bg2Texture) {
		RectangleF src = {0, 0, 512, 288};
		DrawTexture(bg1Texture, &src, &src);
	}
}
