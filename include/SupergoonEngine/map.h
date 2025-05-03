#pragma once
#include <Supergoon/graphics.h>
#ifdef __cplusplus
extern "C" {
#endif

// typedef struct AnimatedTileInstance {
// 	// Where to draw this,
// 	RectangleF dstRect;
// } AnimatedTileInstance;

// Frame data for tile animation
typedef struct TileAnimationFrameData {
	unsigned int tileId;
	RectangleF srcRect;
	unsigned int msTime;
} TileAnimationFrameData;

// The animated tile, it contains all of the tile
typedef struct AnimatedTile {
	// ID of the animated tile, if we get this then we know that we have to load a animated tile when creating the BG.
	unsigned int gid;
	// How many frames are in this animation
	unsigned int numTilesInAnimation;
	// The frame animation info
	TileAnimationFrameData* animatedTileFrameData;
	// Current timeon the animation
	unsigned int currentMSTime;
	// This is created when we are creating the map, so that we know where to draw these.
	RectangleF* DrawRectangles;
	unsigned int currentNumTileInstances;
	unsigned int currentAnimationFrame;
} AnimatedTile;

typedef struct Tileset {
	char name[64];
	int firstgid;
	int tilewidth;
	int tileheight;
	char image[256];
	Texture* tilesetTexture;
	int imagewidth;
	int imageheight;
	AnimatedTile* animatedTiles;
	unsigned int numAnimatedTiles;
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
	struct TiledObject* objects;
	int layer_count;
	LayerGroup groups[2];  // Array of tilesets
} Tilemap;

// You must free this, do not pass .lua into this and yes do it, should be called from the engine only.
Tilemap* parseTiledTilemap(const char* tiledFilename);
void drawCurrentMap(void);
void createBackgroundsFromTilemap(Tilemap* map);
void freeTiledTilemap(Tilemap* map);

#ifdef __cplusplus
}
#endif
