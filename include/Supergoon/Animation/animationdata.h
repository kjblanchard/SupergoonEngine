#pragma once
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int x;
	int y;
	int w;
	int h;
} Rect;

typedef struct {
	char* filename;
	Rect frame;
	int rotated;
	// int trimmed;
	// Rect spriteSourceSize;
	struct {
		int w;
		int h;
	} sourceSize;
	int duration;
} Frame;

typedef enum AnimationDataDirections {
	AnimationDataDirectionsDefault,
	AnimationDataDirectionsPingPong,
	AnimationDataDirectionsForward,
} AnimationDataDirections;

typedef struct {
	char* name;
	int from;
	int to;
	AnimationDataDirections direction;
	// char* color;
} FrameTag;

typedef struct {
	char* name;
	int opacity;
	char* blendMode;
} Layer;

typedef struct {
	char* image;
	// char* name;
	// struct {
	// 	int w;
	// 	int h;
	// } size;

	FrameTag* frameTags;
	int frameTagCount;
	// Layer* layers;
	// int layerCount;
} Meta;

typedef struct {
	Frame* frames;
	int frameCount;
	Meta meta;
} AnimationData;
AnimationData* CreateAnimationData();
void CreateAnimationDataFromAsepriteFile(AnimationData* animationData, const char* filename);
#ifdef __cplusplus
}
#endif
