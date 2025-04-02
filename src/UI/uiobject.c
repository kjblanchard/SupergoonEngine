#include <Supergoon/UI/uiobject.h>

void UIObjectFree(UIObject* object) {
	// Clear all children based on type
	switch (object->Type) {
		default:
			break;
	}
	for (size_t i = 0; i < object->ChildrenCount; i++) {
		UIObjectFree(object->Children[i]);
	}
	if (object->Name) {
		SDL_free(object->Name);
		object->Name = NULL;
	}
	SDL_free(object);
	object = NULL;
}
