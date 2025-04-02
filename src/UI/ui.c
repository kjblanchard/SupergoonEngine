#include <Supergoon/UI/ui.h>
#include <Supergoon/UI/uiobject.h>
#include <Supergoon/graphics.h>
#include <SupergoonEngine/ui.h>
#include <assert.h>

// used in debug ui panel
UIObject* _rootUIObject = NULL;
//  defined in window, used for initial size of the root panel.
extern int _logicalWidth;
extern int _logicalHeight;

static void drawUIObject(UIObject* object) {
	if (!object || !(object->Flags & UIObjectFlagVisible)) {
		return;
	}
	switch (object->Type) {
		default:
// Draw debug box in imgui
#ifdef imgui
			if (object->Flags & UIObjectFlagDebugBox) {
				DrawRect(&object->Location);
			}
#endif
			break;
	}
	for (size_t i = 0; i < object->ChildrenCount; i++) {
		drawUIObject(object->Children[i]);
	}
}

// Get the parents x and y, this could differ due to VLG, HLG, and anything that controls layout
static void getParentXY(float* x, float* y, UIObject* parent) {
	switch (parent->Type) {
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
	// Give values to both a and b
	int priorityA = (objA->Flags & UIObjectFlagSuperPriorityDraw) ? 2 : (objA->Flags & UIObjectFlagPriorityDraw) ? 1
																												 : 0;
	int priorityB = (objB->Flags & UIObjectFlagSuperPriorityDraw) ? 2 : (objB->Flags & UIObjectFlagPriorityDraw) ? 1
																												 : 0;

	return priorityA - priorityB;
}

// Internal dirty function, which calls into the proper ui objects dirty update.
static void onDirtyUIObject(UIObject* object) {
	float xFromParent = 0, yFromParent = 0;
	if (object->Parent) {
		getParentXY(&xFromParent, &yFromParent, object->Parent);
	}
	object->Location.x = xFromParent + object->XOffset;
	object->Location.y = yFromParent + object->YOffset;
	qsort(object->Children, object->ChildrenCount, sizeof(UIObject*), compareUIObjectsLayer);
	switch (object->Type) {
		default:
			break;
	}
	for (size_t i = 0; i < object->ChildrenCount; i++) {
		onDirtyUIObject(object->Children[i]);
	}
	object->Flags &= ~UIObjectFlagDirty;
}

static void updateUIObject(UIObject* object) {
	if (!object || !(object->Flags & UIObjectFlagActive)) {
		return;
	}
	if (object->Flags & UIObjectFlagDirty) {
		onDirtyUIObject(object);
	}
	switch (object->Type) {
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
}

void ShutdownUISystem(void) {
	UIObjectFree(_rootUIObject);
	_rootUIObject = NULL;
}
