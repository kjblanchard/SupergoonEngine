/**
 * @file map.h
 * @author Kevin Blanchard (kevin@supergoon.com)
 * @brief Loading Tiled maps and TiledObjects C API
 * @version 0.1
 * @date 2025-05-04
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once
#ifdef __cplusplus
extern "C" {
#endif

struct GameObject;
#include <Supergoon/Primitives/rectangle.h>

// Tiled properties can be of many types, use this before accessing the union on the property.
typedef enum TiledPropertyTypes {
	TiledPropertyTypeInt,
	TiledPropertyTypeFloat,
	TiledPropertyTypeString,
} TiledPropertyTypes;

// Tiled property, probably use this in your gameobject load functions
typedef struct TiledProperty {
	TiledPropertyTypes PropertyType;
	char* Name;
	union {
		int IntData;
		float FloatData;
		char* StringData;
	} Data;
} TiledProperty;

// Tiled Object, this should be used in your gameobject load functions most likely.
typedef struct TiledObject {
	char* Name;
	int Id;
	int Height;
	int Width;
	int X;
	int Y;
	int ObjectType;
	int NumProperties;
	TiledProperty* Properties;
} TiledObject;

// Loads a tiled map and prepares it to be drawn. You can set a before and after load function to be called.
void LoadMap(const char* map);
// Load the objects from the map into the gameobjects. All gameobjects will be cleared unless they are marked donotdestroy when a map changes.
void LoadObjectsFromMap(void);
void CheckGameobjectForCollisionWithSolids(struct GameObject* gameobject);
void CheckRectForCollisionWithSolids(RectangleF* rect);
#ifdef __cplusplus
}
#endif
