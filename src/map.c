#include <SDL3/SDL.h>
#include <Supergoon/Graphics/shader.h>
#include <Supergoon/Graphics/texture.h>
#include <Supergoon/Primitives/Color.h>
#include <Supergoon/Primitives/rectangle.h>
#include <Supergoon/camera.h>
#include <Supergoon/filesystem.h>
#include <Supergoon/json.h>
#include <Supergoon/map.h>
#include <Supergoon/state.h>
#include <Supergoon/window.h>
#include <sgforge/unpack.h>
#include <sgtools/log.h>
#include <sgtools/tools.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_WALLS 4

Tilemap* currentMap = NULL;
Tileset** tilesets = NULL;
size_t tilesetCount = 0;
size_t tilesetSize = 0;

static void GetRectForGid(int gid, Tileset* tileset, RectangleF* rect) {
	int local = gid - tileset->FirstGid;
	int cols = tileset->ImageWidth / tileset->TileWidth;
	rect->x = (local % cols) * tileset->TileWidth;
	rect->y = (local / cols) * tileset->TileHeight;
	rect->w = tileset->TileWidth;
	rect->h = tileset->TileHeight;
}

static Tileset* GetTilesetForGID(int gid, Tilemap* map) {
	Tileset* best = NULL;
	int highest = 0;
	for (int i = 0; i < tilesetCount; i++) {
		if (gid >= tilesets[i]->FirstGid &&
			tilesets[i]->FirstGid >= highest) {
			highest = tilesets[i]->FirstGid;
			best = tilesets[i];
		}
	}
	return best;
}

static AnimatedTile* getAnimatedTileForGid(int gid, Tileset* tileset) {
	for (size_t i = 0; i < tileset->NumAnimatedTiles; i++) {
		if (tileset->AnimatedTiles[i].GID == gid)
			return &tileset->AnimatedTiles[i];
	}
	return NULL;
}

static TiledPropertyTypes getPropertyTypeForJson(json_object* value) {
	switch (jGetObjectType(value)) {
		case JsonObjectTypes_Int:
			return TiledPropertyTypeInt;
		case JsonObjectTypes_Float:
			return TiledPropertyTypeFloat;
		default:
			return TiledPropertyTypeString;
	}
}

static void createAnimatedTiles(Tileset* tileset, json_object* ts) {
	json_object* tiles = jobj(ts, "tiles");
	if (!tiles) {
		return;
	}
	tileset->NumAnimatedTiles = (unsigned int)jGetObjectArrayLength(tiles);
	tileset->AnimatedTiles = calloc(tileset->NumAnimatedTiles, sizeof(AnimatedTile));
	for (int i = 0; i < tileset->NumAnimatedTiles; i++) {
		json_object* tile = jGetObjectInObjectWithIndex(tiles, i);
		AnimatedTile* anim = &tileset->AnimatedTiles[i];
		anim->Tileset = tileset;
		anim->GID = (unsigned int)jint(tile, "id") + (unsigned int)tileset->FirstGid;
		json_object* animArr = jobj(tile, "animation");
		anim->NumFrames = (unsigned int)jGetObjectArrayLength(animArr);
		anim->TileFrames = calloc(anim->NumFrames, sizeof(TileAnimationFrame));
		for (int j = 0; j < anim->NumFrames; j++) {
			json_object* frame = jGetObjectInObjectWithIndex(animArr, j);
			TileAnimationFrame* f = &anim->TileFrames[j];
			f->MsTime = (unsigned int)jint(frame, "duration");
			f->Id = (unsigned int)jint(frame, "tileid") + (unsigned int)tileset->FirstGid;
			GetRectForGid((int)f->Id, tileset, &f->SrcRect);
		}
	}
}

static Tileset* checkForLoadedTileset(const char* name) {
	for (size_t i = 0; i < tilesetCount; ++i) {
		Tileset* ts = tilesets[i];
		if (ts && strcmp(ts->Name, name) == 0) {
			return ts;
		}
	}
	return NULL;
}

static void addTilesetToCache(Tileset* ts) {
	ArrayResizeIfNeeded((void**)&tilesets, tilesetCount + 1, &tilesetSize, sizeof(Tileset*));
	tilesets[tilesetCount++] = ts;
}

static void createTilesets(Tilemap* map, json_object* root) {
	json_object* tilesetsArrayJson = jobj(root, "tilesets");
	int numTilesets = jGetObjectArrayLength(tilesetsArrayJson);
	for (int i = 0; i < numTilesets; i++) {
		// Check if we already have cached the tileset
		json_object* tilesetJson = jGetObjectInObjectWithIndex(tilesetsArrayJson, i);
		// Tileset* tileset = &map->Tilesets[i];
		const char* name = jstr(tilesetJson, "name");
		if (!name) {
			sgLogError("For some reason can not get name from tileset number %d; %s", i, map->BaseFilename);
			continue;
		}
		Tileset* tileset = checkForLoadedTileset(name);
		if (tileset) {
			continue;
		}
		tileset = malloc(sizeof(*tileset));
		tileset->Name = strdup(jstr(tilesetJson, "name"));
		tileset->FirstGid = jint(tilesetJson, "firstgid");
		tileset->TileWidth = jint(tilesetJson, "tilewidth");
		tileset->TileHeight = jint(tilesetJson, "tileheight");
		tileset->Image = strdup(jstr(tilesetJson, "image"));
		tileset->ImageWidth = jint(tilesetJson, "imagewidth");
		tileset->ImageHeight = jint(tilesetJson, "imageheight");
		addTilesetToCache(tileset);
		createAnimatedTiles(tileset, tilesetJson);
	}
}

static void createTileLayer(TileLayer* layer, json_object* layerObj) {
	layer->Width = jint(layerObj, "width");
	layer->Height = jint(layerObj, "height");
	int count = layer->Width * layer->Height;
	layer->Data = calloc(count, sizeof(int));
	json_object* data = jobj(layerObj, "data");
	for (int i = 0; i < count; i++) {
		layer->Data[i] = jintIndex(data, i);
	}
}

static void handleTiledObjectEntities(Tilemap* map, json_object* layer) {
	json_object* objects = jobj(layer, "objects");
	if (!objects)
		return;
	map->NumObjects = jGetObjectArrayLength(objects);
	map->Objects = calloc(map->NumObjects, sizeof(TiledObject));
	for (size_t i = 0; i < (size_t)map->NumObjects; i++) {
		json_object* obj = jGetObjectInObjectWithIndex(objects, i);
		TiledObject* object = &map->Objects[i];
		object->Id = jint(obj, "id");
		object->ObjectType = atoi(jstr(obj, "type"));
		object->X = jfloat(obj, "x");
		object->Y = jfloat(obj, "y");
		object->Width = jfloat(obj, "width");
		object->Height = jfloat(obj, "height");
		json_object* props = jobj(obj, "properties");
		if (!props) {
			object->NumProperties = 0;
			continue;
		}
		object->NumProperties = jGetObjectArrayLength(props);
		if (object->NumProperties == 0)
			continue;
		object->Properties = calloc(object->NumProperties, sizeof(TiledProperty));
		for (size_t j = 0; j < (size_t)object->NumProperties; j++) {
			json_object* prop = jGetObjectInObjectWithIndex(props, j);
			TiledProperty* property = &object->Properties[j];
			property->Name = strdup(jstr(prop, "name"));
			json_object* value = jobj(prop, "value");
			property->PropertyType = getPropertyTypeForJson(value);
			switch (property->PropertyType) {
				case TiledPropertyTypeInt:
					property->Data.IntData = jint(prop, "value");
					break;
				case TiledPropertyTypeFloat:
					property->Data.FloatData =
						(float)jfloat(prop, "value");
					break;
				case TiledPropertyTypeString:
				default:
					property->Data.StringData = strdup(jstr(prop, "value"));
					break;
			}
		}
	}
}

static void handleLayerGroup(Tilemap* map, json_object* group) {
	const char* name = jstr(group, "name");
	int idx = (strcmp(name, "bg1") == 0) ? 0 : 1;
	LayerGroup* g = &map->LayerGroups[idx];
	g->Name = strdup(name);
	json_object* layers = jobj(group, "layers");
	g->NumLayers = jGetObjectArrayLength(layers);
	g->Layers = calloc(g->NumLayers, sizeof(TileLayer));
	for (size_t i = 0; i < g->NumLayers; i++) {
		createTileLayer(&g->Layers[i], jGetObjectInObjectWithIndex(layers, i));
	}
}

static void handleSolidObjects(Tilemap* map, json_object* layer) {
	json_object* objects = jobj(layer, "objects");
	map->NumSolids = jGetObjectArrayLength(objects);
	map->Solids = calloc(map->NumSolids + NUM_WALLS, sizeof(RectangleF));
	for (size_t i = 0; i < map->NumSolids; i++) {
		json_object* o = jGetObjectInObjectWithIndex(objects, i);
		RectangleF* r = &map->Solids[i];
		r->x = jfloat(o, "x");
		r->y = jfloat(o, "y");
		r->w = jfloat(o, "width");
		r->h = jfloat(o, "height");
	}
	int mw = map->Width * map->TileWidth;
	int mh = map->Height * map->TileHeight;
	int bs = 16;
	RectangleF walls[NUM_WALLS] = {
		{0, -bs, mw, bs},
		{mw, 0, bs, mh},
		{0, mh, mw, bs},
		{-bs, 0, bs, mh}};
	memcpy(&map->Solids[map->NumSolids], walls, sizeof(walls));
	map->NumSolids += NUM_WALLS;
}

static void createLayers(Tilemap* map, json_object* root) {
	json_object* layers = jobj(root, "layers");
	for (size_t i = 0; i < jGetObjectArrayLength(layers); i++) {
		json_object* layer = jGetObjectInObjectWithIndex(layers, i);
		const char* type = jstr(layer, "type");
		if (strcmp(type, "group") == 0) {
			handleLayerGroup(map, layer);
		} else if (strcmp(type, "objectgroup") == 0) {
			const char* name = jstr(layer, "name");
			if (strcmp(name, "solid") == 0)
				handleSolidObjects(map, layer);
			else if (strcmp(name, "entities") == 0)
				handleTiledObjectEntities(map, layer);
		}
	}
}

static void loadTilesetTextures() {
	for (size_t i = 0; i < (size_t)tilesetCount; i++) {
		Tileset* ts = tilesets[i];
		assert(ts);
		if (ts->TilesetTexture) {
			sgLogDebug("Tileset is already loaded, skiping");
			continue;
		}
		if (!ts->Image) {
			sgLogWarn("No Image to load for tileset");
			continue;
		}
		if (!AssetDirectory) {
			sgLogCritical("No asset directory to load from, exiting!");
		}
		char* lastSlash = strrchr(ts->Image, '/');
		if (lastSlash) {
			++lastSlash;
		}
		char* findName = lastSlash ? lastSlash : ts->Image;
		ts->TilesetTexture = TextureCreate(findName);
		char* buf;
		size_t sz;
		int result = GetDataFromDirectory(findName, &buf, &sz, AssetDirectory);
		if (!result) continue;
		TextureLoadFromPngBuffer(ts->TilesetTexture, findName, buf, sz);
	}
}

static void createBackgroundsFromTilemap(Tilemap* map) {
	int w = map->Width * map->TileWidth;
	int h = map->Height * map->TileHeight;
	map->BackgroundTexture = TextureCreateRenderTarget(w, h);
	SetRenderTarget(map->BackgroundTexture);
	TextureClearRenderTarget(map->BackgroundTexture, 0.1f, 0.1f, 0.1f, 255);
	loadTilesetTextures(map);
	LayerGroup* bg = &map->LayerGroups[0];
	RectangleF dst = {0, 0, map->TileWidth, map->TileHeight};
	RectangleF src = {0, 0, 0, 0};
	for (size_t i = 0; i < bg->NumLayers; i++) {
		TileLayer* layer = &bg->Layers[i];
		for (int y = 0; y < layer->Height; y++) {
			for (int x = 0; x < layer->Width; x++) {
				int gid = layer->Data[y * layer->Width + x];
				if (!gid) continue;
				Tileset* ts = GetTilesetForGID(gid, map);
				dst.x = x * map->TileWidth;
				dst.y = y * map->TileHeight;
				AnimatedTile* at = getAnimatedTileForGid(gid, ts);
				if (at) {
					map->AnimatedDrawRectangles = realloc(map->AnimatedDrawRectangles, sizeof(RectangleF) * (++map->AnimatedNumDrawRectangles));
					map->AnimatedGIDList = realloc(map->AnimatedGIDList, sizeof(AnimatedTile*) * map->AnimatedNumDrawRectangles);
					map->AnimatedDrawRectangles[map->AnimatedNumDrawRectangles - 1] = dst;
					map->AnimatedGIDList[map->AnimatedNumDrawRectangles - 1] = at;
					continue;
				}
				GetRectForGid(gid, ts, &src);
				DrawTextureToTexture(map->BackgroundTexture,
									 ts->TilesetTexture,
									 GetDefaultShader(),
									 &dst, &src, 1.0f);
			}
		}
	}
	SetRenderTarget(NULL);
}

static void drawAnimatedTiles(void) {
	for (int i = 0; i < currentMap->AnimatedNumDrawRectangles; ++i) {
		AnimatedTile* at = currentMap->AnimatedGIDList[i];
		RectangleF* dst = &currentMap->AnimatedDrawRectangles[i];
		DrawTexture(at->Tileset->TilesetTexture,
					GetDefaultShader(),
					dst,
					&at->TileFrames[at->CurrentFrame].SrcRect,
					true, 1.0f, false,
					&(Color){255, 255, 255, 255});
	}
}

void UpdateCurrentMap(void) {
	for (int i = 0; i < tilesetCount; ++i) {
		Tileset* ts = tilesets[i];
		for (int j = 0; j < ts->NumAnimatedTiles; ++j) {
			AnimatedTile* at = &ts->AnimatedTiles[j];
			at->CurrentMSOnFrame += (unsigned int)DeltaTimeMilliseconds;
			while (at->CurrentMSOnFrame >= at->TileFrames[at->CurrentFrame].MsTime) {
				at->CurrentMSOnFrame -= at->TileFrames[at->CurrentFrame].MsTime;
				at->CurrentFrame = (at->CurrentFrame + 1) % at->NumFrames;
			}
		}
	}
}

void DrawCurrentMap(void) {
	if (!currentMap) return;
	float texW = (float)TextureGetWidth(currentMap->BackgroundTexture);
	float texH = (float)TextureGetHeight(currentMap->BackgroundTexture);
	RectangleF src = {0, 0, texW, texH};
	RectangleF dst = {0, 0, texW, texH};
	DrawTexture(currentMap->BackgroundTexture,
				GetDefaultShader(), &dst, &src,
				true, 1.0f, false,
				&(Color){255, 255, 255, 255});
	drawAnimatedTiles();
}

static void freeTiledTilemap(Tilemap* map) {
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < map->LayerGroups[i].NumLayers; j++)
			SDL_free(map->LayerGroups[i].Layers[j].Data);
		SDL_free(map->LayerGroups[i].Layers);
		SDL_free(map->LayerGroups[i].Name);
	}
	// for (int i = 0; i < map->NumTilesets; i++) {
	// 	Tileset* ts = &map->Tilesets[i];
	// 	for (size_t j = 0; j < ts->NumAnimatedTiles; j++) {
	// 		SDL_free(ts->AnimatedTiles[j].TileFrames);
	// 	}
	// 	SDL_free(ts->AnimatedTiles);
	// 	SDL_free(ts->Name);
	// 	SDL_free(ts->Image);
	// 	TextureDestroy(ts->TilesetTexture);
	// }
	SDL_free(map->AnimatedDrawRectangles);
	// SDL_free(map->Tilesets);
	SDL_free(map->Solids);
	for (int i = 0; i < map->NumObjects; ++i) {
		TiledObject* object = &map->Objects[i];
		for (int j = 0; j < object->NumProperties; ++j) {
			if (object->Properties[j].PropertyType == TiledPropertyTypeString) {
				SDL_free(object->Properties[j].Data.StringData);
			}
			SDL_free(object->Properties[j].Name);
		}
		SDL_free(object->Properties);
	}
	SDL_free(map->Objects);
	TextureDestroy(map->BackgroundTexture);
	SDL_free(map->BaseFilename);
	SDL_free(map);
}

static void loadMapInternal(const char* name, Tilemap* map, json_object* root) {
	map->BaseFilename = strdup(name);
	map->Width = jint(root, "width");
	map->Height = jint(root, "height");
	map->TileWidth = jint(root, "tilewidth");
	map->TileHeight = jint(root, "tileheight");
	createTilesets(map, root);
	createLayers(map, root);
	createBackgroundsFromTilemap(map);
	jReleaseObjectFromFile(root);
}

static Tilemap* cacheMapFromBuffer(const char* name, char* buf, size_t sz) {
	Tilemap* map = NULL;
	if (!map) {
		map = calloc(1, sizeof(Tilemap));
		json_object* root = jGetObjectFromBuffer(buf, sz);
		if (!root) return NULL;
		loadMapInternal(name, map, root);
	}
	return map;
}

Tilemap* LoadMapFromBuffer(const char* name, char* buf, size_t sz) {
	Tilemap* map = (Tilemap*)cacheMapFromBuffer(name, buf, sz);
	if (!map) sgLogCritical("Could not get map from cache, why for %s?", name);
	currentMap = map;
	SetCameraBounds(map->Width * map->TileWidth, map->Height * map->TileHeight);
	SetCameraSize(map->Width * map->TileWidth, map->Height * map->TileHeight);
	return map;
}

void LoadMap(Tilemap* m) {
	if (!m) {
		sgLogError("tried to load a bad map!?");
		return;
	}
	currentMap = m;
	SetCameraBounds(m->Width * m->TileWidth, m->Height * m->TileHeight);
	SetCameraSize(m->Width * m->TileWidth, m->Height * m->TileHeight);
}

void ShutdownMapSystem(void) {}

void CheckRectForCollisionWithSolids(RectangleF* rect) {
	for (int i = 0; i < currentMap->NumSolids; i++) {
		if (RectIsCollision(rect, &currentMap->Solids[i]))
			RectResolveCollision(rect, &currentMap->Solids[i]);
	}
}
