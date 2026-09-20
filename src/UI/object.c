#include <Supergoon/Primitives/Color.h>
#include <Supergoon/UI/object.h>
#include <string.h>
#ifdef imgui
#include <Supergoon/Graphics/graphics.h>
#endif

static unsigned int currentID = 0;

UIObject* UIObjectCreate(void) {
	UIObject* o = calloc(1, sizeof(*o));
	o->Id = currentID++;
	o->Active = true;
	o->Visible = true;
	o->Dirty = true;
	o->Debug = true;
	return o;
}

void UIObjectDraw(UIObject* o, Vector2* p) {
	o->ParentOffset.X = p->X;
	o->ParentOffset.Y = p->Y;
	if (o->Active && o->Visible && o->Type && o->Type->Draw) {
		o->Type->Draw(o);
	}
	Vector2 offset = Vector2Add(*p, (Vector2){o->Rect.x, o->Rect.y});
#ifdef imgui
	if (o->Debug) {
		DrawRect(&(RectangleF){offset.X, offset.Y, o->Rect.w, o->Rect.h}, &(Color){255, 255, 255, 255}, false, false);
	}
#endif
	if (o->Child) {
		UIObjectDraw(o->Child, &offset);
	}
	if (o->Sibling) {
		UIObjectDraw(o->Sibling, p);
	}
}

void UIObjectDirty(UIObject* o) {
	if (o->Active && o->Type && o->Type->Dirty) {
		o->Type->Dirty(o);
		o->Dirty = false;
	}
	if (o->Child) {
		UIObjectDirty(o->Child);
	}
	if (o->Sibling) {
		UIObjectDirty(o->Sibling);
	}
}

// TODO this is overly complicated, reduce / make internal function
void UIObjectDestroy(UIObject* o) {
	// Remove from parent
	if (o->Parent) {
		UIObjectRemoveChild(o->Parent, o);
	}
	// Destroy child and children
	UIObject* child = o->Child;
	while (child) {
		UIObject* next = child->Sibling;
		child->Sibling = NULL;
		child->Parent = NULL;
		UIObjectDestroy(child);
		child = next;
	}
	o->Child = NULL;
	// Destroy the uiobject
	if (o->Type && o->Type->Destroy) {
		o->Type->Destroy(o);
	}
	if (o->TypeData) {
		free(o->TypeData);
	}
	if (o->Name) {
		free(o->Name);
	}
	free(o);
}

Vector2 UIObjectGetAbsolutePosition(UIObject* o) {
	Vector2 offset = Vector2Add(o->ParentOffset, (Vector2){o->Rect.x, o->Rect.y});
	return offset;
}

void UIObjectSetAbsolutePosition(UIObject* o, Vector2 pos) {
	Vector2 parentPos = UIObjectGetAbsolutePosition(o);
	o->Rect.x = pos.X - parentPos.X;
	o->Rect.y = pos.Y - parentPos.Y;
}

UIObject* GetChildByName(UIObject* o, const char* name) {
	// Check self
	if (o->Name && strcmp(o->Name, name) == 0) {
		return o;
	}
	// Check Child and his siblings
	for (UIObject* child = o->Child; child; child = child->Sibling) {
		UIObject* found = GetChildByName(child, name);
		if (found) {
			return found;
		}
	}
	return NULL;
}

void UIObjectUpdatePriority(UIObject* o, unsigned int p) {
	if (!o || o->Priority == p) {
		return;
	}
	o->Priority = p;
	if (!o->Parent) {
		return;
	}
	UIObjectRemoveChild(o->Parent, o);
	UIObjectAddChild(o->Parent, o);
}

void UIObjectAddChild(UIObject* o, UIObject* child) {
	if (!o || !child || o == child) {
		return;
	}
	if (child->Parent) {
		UIObjectRemoveChild(child->Parent, child);
	}
	child->Parent = o;
	if (!o->Child) {
		o->Child = child;
		return;
	}
	// If this is the lowest priority, add it first
	if (child->Priority < o->Child->Priority) {
		child->Sibling = o->Child;
		o->Child = child;
		return;
	}
	// We already have a child, so add sibling to the chold properly
	UIObject* insert = o->Child;
	while (insert && insert->Sibling && child->Priority >= insert->Sibling->Priority) {
		insert = insert->Sibling;
	}
	// If we have a sibling, we need to switch it around
	if (insert->Sibling) {
		child->Sibling = insert->Sibling;
	}
	insert->Sibling = child;
}

void UIObjectRemoveChild(UIObject* o, UIObject* child) {
	if (!o->Child) {
		return;
	}

	if (o->Child == child) {
		o->Child = child->Sibling;
		child->Sibling = NULL;
		child->Parent = NULL;
		return;
	}
	UIObject* next = o->Child;
	while (next && next->Sibling != child) {
		next = next->Sibling;
	}
	if (!next) {
		return;
	}
	next->Sibling = next->Sibling->Sibling;
	child->Sibling = NULL;
	child->Parent = NULL;
}
