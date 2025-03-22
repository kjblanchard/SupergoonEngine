#pragma once
#ifdef __cplusplus
extern "C" {
#endif

typedef enum TiledPropertyTypes {
	Int,
	Float,
	String,
} TiledPropertyTypes;

typedef struct TiledProperty {
	TiledPropertyTypes PropertyType;
	char Name[64];
	union {
		int IntData;
		float FloatData;
		char* StringData;
	} Data;
} TiledProperty;

typedef struct TiledObject {
	char Name[64];
	int Id;
	int Height;
	int Width;
	int X;
	int Y;
	int ObjectType;
	int PropertyNum;
	TiledProperty* Properties;
} TiledObject;
typedef struct Tileset {
	char name[64];
	int firstgid;
	int tilewidth;
	int tileheight;
	char image[256];
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
	TileLayer* Layers;
} LayerGroup;

typedef struct Tilemap {
	int width;
	int height;
	int tilewidth;
	int tileheight;
	int tileset_count;
	Tileset* tilesets;	// Array of tilesets
	TiledObject* objects;
	int layer_count;
	LayerGroup groups[2];  // Array of tilesets
} Tilemap;

// You must free this, do not pass .lua into this and yes do it, should be called from the engine only.
Tilemap* parseTiledTilemap(const char* tiledFilename);
void createBackgroundsFromTilemap(Tilemap map);

#ifdef __cplusplus
}
#endif
