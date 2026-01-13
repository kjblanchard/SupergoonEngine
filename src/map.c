#include <SDL3/SDL.h>
#include <Supergoon/Graphics/shader.h>
#include <Supergoon/Graphics/texture.h>
#include <Supergoon/Primitives/Color.h>
#include <Supergoon/Primitives/rectangle.h>
#include <Supergoon/camera.h>
#include <Supergoon/filesystem.h>
#include <Supergoon/json.h>
#include <Supergoon/log.h>
#include <Supergoon/map.h>
#include <Supergoon/state.h>
#include <Supergoon/tools.h>
#include <Supergoon/window.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_WALLS 4
#define MAX_PREVIOUS_MAPS_CACHE 3

Tilemap* _currentMap = NULL;
static Tilemap* _previousMaps[MAX_PREVIOUS_MAPS_CACHE] = {NULL};

//
// ------------------------------------------------------------
// Tileset helpers
// ------------------------------------------------------------
//

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

	for (int i = 0; i < map->NumTilesets; i++) {
		if (gid >= map->Tilesets[i].FirstGid &&
			map->Tilesets[i].FirstGid >= highest) {
			highest = map->Tilesets[i].FirstGid;
			best = &map->Tilesets[i];
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
	if (!tiles)
		return;

	tileset->NumAnimatedTiles = jGetObjectArrayLength(tiles);
	tileset->AnimatedTiles =
		calloc(tileset->NumAnimatedTiles, sizeof(AnimatedTile));

	for (size_t i = 0; i < tileset->NumAnimatedTiles; i++) {
		json_object* tile = jGetObjectInObjectWithIndex(tiles, i);
		AnimatedTile* anim = &tileset->AnimatedTiles[i];
		anim->GID = jint(tile, "id") + tileset->FirstGid;
		json_object* animArr = jobj(tile, "animation");
		anim->NumFrames = jGetObjectArrayLength(animArr);
		anim->TileFrames =
			calloc(anim->NumFrames, sizeof(TileAnimationFrame));

		for (size_t j = 0; j < anim->NumFrames; j++) {
			json_object* frame = jGetObjectInObjectWithIndex(animArr, j);
			TileAnimationFrame* f = &anim->TileFrames[j];

			f->MsTime = jint(frame, "duration");
			f->Id = jint(frame, "tileid") + tileset->FirstGid;
			GetRectForGid(f->Id, tileset, &f->SrcRect);
		}
	}
}

static void createTilesets(Tilemap* map, json_object* root) {
	json_object* tilesets = jobj(root, "tilesets");
	map->NumTilesets = jGetObjectArrayLength(tilesets);
	map->Tilesets = calloc(map->NumTilesets, sizeof(Tileset));

	for (int i = 0; i < map->NumTilesets; i++) {
		json_object* ts = jGetObjectInObjectWithIndex(tilesets, i);
		Tileset* tileset = &map->Tilesets[i];
		/* tileset->Name = strdup(jstr(ts, "name")); */
		const char* name = jstr(ts, "name");
		if (!name) {
			sgLogCritical("For some reason can not get name from tileset number %d; %s", i, map->BaseFilename);
			continue;
		}
		tileset->Name = strdup(jstr(ts, "name"));
		tileset->FirstGid = jint(ts, "firstgid");
		tileset->TileWidth = jint(ts, "tilewidth");
		tileset->TileHeight = jint(ts, "tileheight");
		tileset->Image = strdup(jstr(ts, "image"));
		tileset->ImageWidth = jint(ts, "imagewidth");
		tileset->ImageHeight = jint(ts, "imageheight");

		createAnimatedTiles(tileset, ts);
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

		object->Properties =
			calloc(object->NumProperties, sizeof(TiledProperty));

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
					property->Data.StringData =
						strdup(jstr(prop, "value"));
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
	map->Solids =
		calloc(map->NumSolids + NUM_WALLS, sizeof(RectangleF));

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

static void loadTilesetTextures(Tilemap* map) {
	for (size_t i = 0; i < (size_t)map->NumTilesets; i++) {
		if (map->Tilesets[i].TilesetTexture) {
			sgLogDebug("Tileset is already loaded, skiping");
			continue;
		}
		if (!map->Tilesets[i].Image) {
			sgLogWarn("No Image to load for tileset");
			continue;
		}

		int len = strlen(map->Tilesets[i].Image);
		if (len > 4 &&
			strcmp(map->Tilesets[i].Image + len - 4, ".png") == 0)
			map->Tilesets[i].Image[len - 4] = '\0';

		map->Tilesets[i].TilesetTexture = TextureCreate(map->Tilesets[i].Image);
		TextureLoadFromPng(map->Tilesets[i].TilesetTexture,
						   map->Tilesets[i].Image);
	}
}

static void createBackgroundsFromTilemap(Tilemap* map) {
	int w = map->Width * map->TileWidth;
	int h = map->Height * map->TileHeight;

	map->BackgroundTexture =
		TextureCreateRenderTarget(w, h);

	SetRenderTarget(map->BackgroundTexture);
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
					at->DrawRectangles =
						realloc(at->DrawRectangles,
								sizeof(RectangleF) *
									(++at->NumDrawRectangles));
					at->DrawRectangles[at->NumDrawRectangles - 1] = dst;
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
	for (size_t i = 0; i < _currentMap->NumTilesets; i++) {
		Tileset* ts = &_currentMap->Tilesets[i];
		for (size_t j = 0; j < ts->NumAnimatedTiles; j++) {
			AnimatedTile* a = &ts->AnimatedTiles[j];
			a->CurrentMSOnFrame += DeltaTimeMilliseconds;

			while (a->CurrentMSOnFrame >=
				   a->TileFrames[a->CurrentFrame].MsTime) {
				a->CurrentMSOnFrame -=
					a->TileFrames[a->CurrentFrame].MsTime;
				a->CurrentFrame =
					(a->CurrentFrame + 1) % a->NumFrames;
			}
		}
	}

	for (size_t i = 0; i < _currentMap->NumTilesets; i++) {
		Tileset* ts = &_currentMap->Tilesets[i];
		for (size_t j = 0; j < ts->NumAnimatedTiles; j++) {
			AnimatedTile* a = &ts->AnimatedTiles[j];
			for (size_t k = 0; k < a->NumDrawRectangles; k++) {
				DrawTexture(ts->TilesetTexture,
							GetDefaultShader(),
							&a->DrawRectangles[k],
							&a->TileFrames[a->CurrentFrame].SrcRect,
							true, 1.0f, false,
							&(Color){255, 255, 255, 255});
			}
		}
	}
}

void DrawCurrentMap(void) {
	if (!_currentMap) return;

	int w = TextureGetWidth(_currentMap->BackgroundTexture);
	int h = TextureGetHeight(_currentMap->BackgroundTexture);

	RectangleF src = {0, 0, w, h};
	RectangleF dst = {-CameraGetX(), -CameraGetY(), w, h};

	DrawTexture(_currentMap->BackgroundTexture,
				GetDefaultShader(), &dst, &src,
				false, 1.0f, false,
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

	for (int i = 0; i < map->NumTilesets; i++) {
		Tileset* ts = &map->Tilesets[i];
		for (size_t j = 0; j < ts->NumAnimatedTiles; j++) {
			SDL_free(ts->AnimatedTiles[j].TileFrames);
			SDL_free(ts->AnimatedTiles[j].DrawRectangles);
		}
		SDL_free(ts->AnimatedTiles);
		SDL_free(ts->Name);
		SDL_free(ts->Image);
		TextureDestroy(ts->TilesetTexture);
	}

	SDL_free(map->Tilesets);
	SDL_free(map->Solids);
	TextureDestroy(map->BackgroundTexture);
	SDL_free(map->BaseFilename);
	SDL_free(map);
}

static void moveToFront(Tilemap** arr, int idx) {
	Tilemap* tmp = arr[idx];
	for (int i = idx; i > 0; i--)
		arr[i] = arr[i - 1];
	arr[0] = tmp;
}

static Tilemap* checkCache(const char* name) {
	for (int i = 0; i < MAX_PREVIOUS_MAPS_CACHE; i++) {
		if (!_previousMaps[i]) break;
		if (strcmp(_previousMaps[i]->BaseFilename, name) == 0) {
			moveToFront(_previousMaps, i);
			return _previousMaps[0];
		}
	}
	return NULL;
}

void LoadMap(const char* name) {
	Tilemap* map = checkCache(name);
	if (!map) {
		char path[256];
		snprintf(path, sizeof(path), "assets/tiled/%s.tmj", name);

		json_object* root = jGetObjectFromFile(path);
		if (!root) return;

		map = calloc(1, sizeof(Tilemap));
		map->BaseFilename = strdup(name);
		map->Width = jint(root, "width");
		map->Height = jint(root, "height");
		map->TileWidth = jint(root, "tilewidth");
		map->TileHeight = jint(root, "tileheight");

		createTilesets(map, root);
		createLayers(map, root);
		createBackgroundsFromTilemap(map);

		jReleaseObjectFromFile(root);

		for (int i = MAX_PREVIOUS_MAPS_CACHE - 1; i > 0; i--)
			_previousMaps[i] = _previousMaps[i - 1];
		_previousMaps[0] = map;
	}

	_currentMap = map;
	SetCameraBounds(map->Width * map->TileWidth,
					map->Height * map->TileHeight);
	SetCameraSize(map->Width * map->TileWidth,
				  map->Height * map->TileHeight);
}

void ShutdownMapSystem(void) {
	for (int i = 0; i < MAX_PREVIOUS_MAPS_CACHE; i++) {
		if (_previousMaps[i])
			freeTiledTilemap(_previousMaps[i]);
		_previousMaps[i] = NULL;
	}
}

void CheckRectForCollisionWithSolids(RectangleF* rect) {
	for (int i = 0; i < _currentMap->NumSolids; i++) {
		if (RectIsCollision(rect, &_currentMap->Solids[i]))
			RectResolveCollision(rect, &_currentMap->Solids[i]);
	}
}
