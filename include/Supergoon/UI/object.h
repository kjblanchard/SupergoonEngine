/**
 * @file object.h
 * @brief UI object using singly linked list
 * @author Kevin Blanchard
 * @version 0.1.0
 * @date 2026-09-18
 */
#pragma once
#include <Supergoon/Primitives/Vector2.h>
#include <Supergoon/Primitives/rectangle.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UIObject UIObject;
typedef struct UIType UIType;

struct UIObject {
	unsigned int Id;
	char* Name;
	RectangleF Rect;
	Vector2 ParentOffset;
	unsigned int Active : 1;
	unsigned int Visible : 1;
	unsigned int Dirty : 1;
	unsigned int Debug : 1;
	unsigned int Unused : 3;
	unsigned int Priority;
	UIType* Type;
	void* TypeData;
	UIObject* Parent;
	UIObject* Child;
	UIObject* Sibling;
};

struct UIType {
	const char* Name;
	void (*Dirty)(UIObject* o);
	void (*Draw)(UIObject* o);
	void (*Destroy)(UIObject* o);
};

UIObject* UIObjectCreate(void);
void UIObjectDraw(UIObject* o, Vector2* p);
void UIObjectDirty(UIObject* o);
void UIObjectDestroy(UIObject* o);
Vector2 UIObjectGetAbsolutePosition(UIObject* o);
void UIObjectSetAbsolutePosition(UIObject* o, Vector2 pos);
void UIObjectUpdatePriority(UIObject* o, unsigned int p);
// Recursive walk through each, can be expensive so cache result
UIObject* GetChildByName(UIObject* o, const char* name);
void UIObjectAddChild(UIObject* o, UIObject* child);
void UIObjectRemoveChild(UIObject* o, UIObject* child);

#ifdef __cplusplus
}
#endif
