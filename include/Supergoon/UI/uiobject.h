#pragma once
#include <Supergoon/Primitives/rectangle.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef enum UIObjectTypes {
	UIObjectTypesPanel,
	UIObjectTypesImage,
	UIObjectTypesText,

} UIObjectTypes;
typedef enum UIObjectFlags {
	UIObjectFlagDirty = 1 << 0,
	UIObjectFlagVisible = 1 << 1,
	UIObjectFlagActive = 1 << 2,
	UIObjectFlagPriorityDraw = 1 << 3,
	UIObjectFlagSuperPriorityDraw = 1 << 4,
	UIObjectFlagDebugBox = 1 << 5,
} UIObjectFlags;

typedef struct UIObject {
	uint16_t Id;
	UIObjectTypes Type;
	RectangleF Location;
	float XOffset;
	float YOffset;
	char* Name;
	uint8_t Flags;
	void* Data;
	struct UIObject* Parent;
	uint8_t ChildrenCount;
	struct UIObject** Children;
} UIObject;

void UIObjectFree(UIObject* object);

#ifdef __cplusplus
}
#endif
