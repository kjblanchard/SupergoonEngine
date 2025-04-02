#pragma once
#ifdef __cplusplus
extern "C" {
#endif

typedef enum TiledPropertyTypes {
	TiledPropertyTypeInt,
	TiledPropertyTypeFloat,
	TiledPropertyTypeString,
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

void LoadMap(const char* map);
void LoadObjectsFromMap(void);
#ifdef __cplusplus
}
#endif
