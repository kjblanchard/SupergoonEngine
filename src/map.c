#include <Supergoon/filesystem.h>
#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <SupergoonEngine/map.h>
#include <stdio.h>
#include <string.h>

// Reads through the tiled layer group and assigns properly
static void handleTiledLayerGroup(Tilemap* map);
static void handleTiledObjectGroup(Tilemap* map);

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
	map->tilesets = malloc(map->tileset_count * sizeof(Tileset));
	for (int i = 0; i < map->tileset_count; i++) {
		LuaPushTableObjectToStacki(i);
		LuaCopyString("name", map->tilesets[i].name, sizeof(map->tilesets[i].name));
		map->tilesets[i].firstgid = LuaGetInt("firstgid");
		map->tilesets[i].tilewidth = LuaGetInt("tilewidth");
		map->tilesets[i].tileheight = LuaGetInt("tileheight");
		LuaCopyString("image", map->tilesets[i].image, sizeof(map->tilesets[i].image));
		map->tilesets[i].imagewidth = LuaGetInt("imagewidth");
		map->tilesets[i].imageheight = LuaGetInt("imageheight");
		LuaPopStack(1);
	}
	LuaPopStack(1);
	LuaPushTableToStack("layers");
	map->layer_count = LuaGetTableLength();
	for (int i = 0; i < map->layer_count; i++) {
		LuaPushTableObjectToStacki(i);
		const char* layerType = LuaGetString("type");
		if (strcmp(layerType, "objectgroup") == 0) {
			handleTiledObjectGroup(map);
			sgLogWarn("This is a object group!");
		} else if (strcmp(layerType, "group") == 0) {
			handleTiledLayerGroup(map);
		}
		LuaPopStack(1);
	}
	LuaPopStack(1);
	LuaClearStack();
	return map;
}

static void createTileLayer(TileLayer* layer) {
	layer->width = LuaGetInt("width");
	layer->height = LuaGetInt("height");
	int data_length = layer->width * layer->height;
	layer->data = malloc(data_length * sizeof(int));
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
	int numLayers = LuaGetTableLength();
	group->Layers = calloc(numLayers, sizeof(TileLayer));
	for (size_t i = 0; i < (size_t)numLayers; i++) {
		LuaPushTableObjectToStacki(i);
		createTileLayer(&group->Layers[i]);
		LuaPopStack(1);
	}
	LuaPopStack(1);
}

static void handleTiledObjectGroup(Tilemap* map) {
}

void createBackgroundsFromTilemap(Tilemap map) {
}
