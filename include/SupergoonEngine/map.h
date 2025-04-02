#pragma once
#include <Supergoon/graphics.h>
#ifdef __cplusplus
extern "C" {
#endif


typedef struct Tileset {
	char name[64];
	int firstgid;
	int tilewidth;
	int tileheight;
	char image[256];
	Texture* tilesetTexture;
	int imagewidth;
	int imageheight;
} Tileset;

typedef struct TileLayer {
	int width;
	int height;
	int* data;	// Dynamically allocated array for tile IDs
} TileLayer;

typedef struct LayerGroup {
	char Name[64];
	int NumLayers;
	TileLayer* Layers;
} LayerGroup;

typedef struct Tilemap {
	int width;
	int height;
	int tilewidth;
	int tileheight;
	int tileset_count;
	Tileset* tilesets;	// Array of tilesets
	int num_objects;
	TiledObject* objects;
	int layer_count;
	LayerGroup groups[2];  // Array of tilesets
} Tilemap;

// You must free this, do not pass .lua into this and yes do it, should be called from the engine only.
Tilemap* parseTiledTilemap(const char* tiledFilename);
void createBackgroundsFromTilemap(Tilemap* map);
void freeTiledTilemap(Tilemap* map);

#ifdef __cplusplus
}
#endif
