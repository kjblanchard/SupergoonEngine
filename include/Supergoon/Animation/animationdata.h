/**
 * @file animationdata.h
 * @brief Aseprite animation data loader
 * @author Kevin Blanchard
 * @version 0.2.0
 * @date 2026-09-17
 */
#pragma once
#include <stddef.h>
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
} FrameTag;

typedef struct {
	char* name;
	int opacity;
	char* blendMode;
} Layer;

typedef struct {
	char* image;

	FrameTag* frameTags;
	int frameTagCount;
} Meta;

typedef struct {
	Frame* frames;
	int frameCount;
	Meta meta;
} AnimationData;

AnimationData* CreateAnimationData(void);
void CreateAnimationDataFromAsepriteFile(AnimationData* animationData, const char* filename);
void CreateAnimationDataFromAsepriteBuffer(AnimationData* animationData, char* buf, size_t sz);
void DestroyAnimationData(AnimationData* data);
#ifdef __cplusplus
}
#endif
