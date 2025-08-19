#include <Supergoon/UI/ui.h>
#include <Supergoon/UI/uiImageAnimation.h>
#include <Supergoon/UI/uiProgressBar.h>
#include <Supergoon/UI/uibutton.h>
#include <Supergoon/UI/uiimage.h>
#include <Supergoon/UI/uilayoutgroup.h>
#include <Supergoon/UI/uiobject.h>
#include <Supergoon/UI/uirect.h>
#include <Supergoon/UI/uitext.h>
#include <Supergoon/graphics.h>
#include <Supergoon/log.h>
#include <SupergoonEngine/ui.h>
#include <assert.h>
#include <stdlib.h>

// used in debug ui panel
UIObject* _rootUIObject = NULL;
//  defined in window, used for initial size of the root panel.
extern int _logicalWidth;
extern int _logicalHeight;
unsigned int _nextObjectId = 0;

static void drawUIObject(UIObject* object) {
	if (!object || !(object->Flags & UIObjectFlagVisible)) {
		return;
	}
	switch (object->Type) {
		case UIObjectTypesImage:
			DrawUIImage(object);
			break;
		case UIObjectTypesImageAnimator:
			DrawUIImageAnimation(object);
			break;
		case UIObjectTypesText:
			UITextDraw(object);
		case UIObjectTypesRect:
			UIRectDraw(object);
		case UIObjectTypesProgressBar:
			DrawProgressBar(object);
			break;
		default:
			break;
	}
// Draw debug box in imgui
#ifdef imgui
	if (object->Flags & UIObjectFlagDebugBox) {
		static sgColor debugColor = {255, 255, 255, 255};
		DrawRect(&object->Location, &debugColor, false);
	}
#endif
	for (size_t i = 0; i < object->ChildrenCount; i++) {
		drawUIObject(object->Children[i]);
	}
}

// Get the parents x and y, this could differ due to VLG, HLG, and anything that controls layout
static void getParentXY(float* x, float* y, UIObject* parent, UIObject* child) {
	switch (parent->Type) {
		case UIObjectTypesLayoutGroup: {
			UILayoutGroup* layoutData = (UILayoutGroup*)parent->Data;
			assert(layoutData && "No layout group data..");
			unsigned int childNum = 0;
			for (size_t i = 0; i < parent->ChildrenCount; i++) {
				if (parent->Children[i] == child) {
					childNum = i;
					break;
				}
			}
			*x = layoutData->IsHorizontal ? parent->Location.x + (layoutData->Spacing * childNum) : parent->Location.x;
			*y = layoutData->IsHorizontal ? parent->Location.y : parent->Location.y + (layoutData->Spacing * childNum);
			break;
		}
		default:
			*x = parent->Location.x;
			*y = parent->Location.y;
			break;
	}
}

static int compareUIObjectsLayer(const void* a, const void* b) {
	// Only dereference once to get the object.. qsort will populate both as whatever type the "array" is
	UIObject* objA = *(UIObject**)a;
	UIObject* objB = *(UIObject**)b;
	int priorityA = ((objA->Flags & UIObjectFlagSuperPriorityDraw) ? 2 : 0) |
					((objA->Flags & UIObjectFlagPriorityDraw) ? 1 : 0);

	int priorityB = ((objB->Flags & UIObjectFlagSuperPriorityDraw) ? 2 : 0) |
					((objB->Flags & UIObjectFlagPriorityDraw) ? 1 : 0);

	return priorityA - priorityB;
}

// Internal dirty function, which calls into the proper ui objects dirty update.
static void onDirtyUIObject(UIObject* object) {
	float xFromParent = 0, yFromParent = 0;
	if (object->Parent) {
		// We need to resort the parents children on dirty too, in the case of a layer change on the child, etc
		qsort(object->Parent->Children, object->Parent->ChildrenCount, sizeof(UIObject*), compareUIObjectsLayer);
		getParentXY(&xFromParent, &yFromParent, object->Parent, object);
	}
	object->Location.x = xFromParent + object->XOffset;
	object->Location.y = yFromParent + object->YOffset;
	qsort(object->Children, object->ChildrenCount, sizeof(UIObject*), compareUIObjectsLayer);
	switch (object->Type) {
		case UIObjectTypesText:
			UITextOnDirty(object);
			break;

		default:
			break;
	}
	for (size_t i = 0; i < object->ChildrenCount; i++) {
		onDirtyUIObject(object->Children[i]);
	}
	object->Flags &= ~UIObjectFlagDirty;
}

static void updateUIInputObject(UIObject* object) {
	if (!object || !(object->Flags & UIObjectFlagActive)) {
		return;
	}
	switch (object->Type) {
		case UIObjectTypesButton:
			UpdateUIButton(object);
			break;
		default:
			break;
	}
	for (size_t i = 0; i < object->ChildrenCount; i++) {
		updateUIInputObject(object->Children[i]);
	}
}

static void updateUIObject(UIObject* object) {
	if (!object || !(object->Flags & UIObjectFlagActive)) {
		return;
	}
	if (object->Flags & UIObjectFlagDirty) {
		onDirtyUIObject(object);
	}
	switch (object->Type) {
		// case UIObjectTypesButton:
		// 	UpdateUIButton(object);
		default:
			break;
	}
	for (size_t i = 0; i < object->ChildrenCount; i++) {
		updateUIObject(object->Children[i]);
	}
}

void InitializeUISystem(void) {
	_rootUIObject = calloc(1, sizeof(*_rootUIObject));
	_rootUIObject->Location.w = _logicalWidth;
	_rootUIObject->Location.h = _logicalHeight;
	_rootUIObject->Name = strdup("Root Panel");
	_rootUIObject->Flags |= UIObjectFlagActive | UIObjectFlagVisible | UIObjectFlagDirty;
	InitializeUITextSystem();
}

void UpdateUIInputSystem(void) {
	assert(_rootUIObject && "No root man");
	updateUIInputObject(_rootUIObject);
}

void UpdateUISystem(void) {
	assert(_rootUIObject && "No root man");
	updateUIObject(_rootUIObject);
}

void DrawUISystem(void) {
	assert(_rootUIObject && "No root man");
	drawUIObject(_rootUIObject);
}

void AddUIObject(UIObject* child, UIObject* parent) {
	// TODO should we store the size of array so we don't have to realloc every time?
	if (!child) return;
	parent = parent ? parent : _rootUIObject;
	UIObject** newChildren = realloc(parent->Children, (parent->ChildrenCount + 1) * sizeof(UIObject*));
	assert(newChildren && "Broken man with realloc");
	parent->Children = newChildren;
	parent->Children[parent->ChildrenCount++] = child;
	child->Parent = parent;
	// Load based on type
	switch (child->Type) {
		case UIObjectTypesText:
			UITextLoad(child);
			break;

		default:
			break;
	}
	++_nextObjectId;
}

void ShutdownUISystem(void) {
	UIObjectFree(_rootUIObject);
	_rootUIObject = NULL;
}
