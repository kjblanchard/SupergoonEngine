/**
 * @file map.h
 * @author Kevin Blanchard (kevin@supergoon.com)
 * @brief Tiled wrapper.  Loads maps into two BG layers, load objects, handle animated tiles.
 * @version 0.1
 * @date 2025-05-04
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once
#include <Supergoon/graphics.h>
#ifdef __cplusplus
extern "C" {
#endif

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
	char* BaseFilename;
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
	RectangleF* Solids;
	int NumSolids;
} Tilemap;

// Draws the current map bg layers and animated tiles.
void drawCurrentMap(void);
// Used when resetting or closing the game, clears out any static info like a loaded map, etc.
void shutdownMapSystem(void);

// Current map that will be drawn, if it exists.
extern Tilemap* _currentMap;

#ifdef __cplusplus
}
#endif
