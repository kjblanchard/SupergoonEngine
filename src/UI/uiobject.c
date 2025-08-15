#include <Supergoon/UI/uiImageAnimation.h>
#include <Supergoon/UI/uiimage.h>
#include <Supergoon/UI/uiobject.h>
#include <Supergoon/UI/uitext.h>

static void removeObjectFromParent(UIObject* object) {
	UIObject* parent = object->Parent;
	int childToRemove = -1;
	for (size_t i = 0; i < parent->ChildrenCount; i++) {
		UIObject* child = parent->Children[i];
		if (child != object) {
			continue;
		}
		childToRemove = i;
		break;
	}
	if (childToRemove == -1) {
		return;
	}
	for (size_t i = childToRemove; i < parent->ChildrenCount - 1; i++) {
		parent->Children[i] = parent->Children[i + 1];
	}
	--parent->ChildrenCount;
}

void UIObjectFree(UIObject* object) {
	// Clear all children based on type
	switch (object->Type) {
		default:
			break;
		case UIObjectTypesText:
			DestroyUIText(object);
			break;
		case UIObjectTypesImage:
			DestroyUIImage(object);
			break;
		case UIObjectTypesImageAnimator:
			DestroyUIImageAnimation(object);
			break;
	}
	if (object->Data) {
		SDL_free(object->Data);
		object->Data = NULL;
	}
	if (object->ChildrenCount > 0) {
		for (size_t i = 0; i < object->ChildrenCount; i++) {
			UIObjectFree(object->Children[i]);
		}
		SDL_free(object->Children);
		object->Children = NULL;
	}
	if (object->Name) {
		SDL_free(object->Name);
		object->Name = NULL;
	}
	if (object->Parent) {
		removeObjectFromParent(object);
	}
	SDL_free(object);
	object = NULL;
}
