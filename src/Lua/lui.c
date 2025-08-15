// TODO this shouldn't be here
#include <SDL3/SDL.h>
#include <Supergoon/Animation/animator.h>
#include <Supergoon/UI/ui.h>
#include <Supergoon/UI/uiImageAnimation.h>
#include <Supergoon/UI/uibutton.h>
#include <Supergoon/UI/uiimage.h>
#include <Supergoon/UI/uilayoutgroup.h>
#include <Supergoon/UI/uiobject.h>
#include <Supergoon/UI/uirect.h>
#include <Supergoon/UI/uitext.h>
#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <Supergoon/sprite.h>
#include <SupergoonEngine/Animation/animator.h>
#include <SupergoonEngine/Lua/ui.h>
#include <SupergoonEngine/tools.h>
#include <SupergoonEngine/ui.h>
#include <lauxlib.h>
#include <lua.h>

#define LUA_BUTTON_FUNCS_KEY "GameUIButtonFuncs"

void initButtonFuncTable(lua_State* L) {
	lua_newtable(L);										   // Create a new table
	lua_setfield(L, LUA_REGISTRYINDEX, LUA_BUTTON_FUNCS_KEY);  // Store in registry with key
}

static UIObject* createUIObject(LuaState L) {
	UIObject* testObj = SDL_calloc(1, sizeof(*testObj));
	testObj->Name = strdup(LuaGetStringi(L, 1));
	testObj->XOffset = LuaGetFloatFromTableStackiKey(L, 2, "x");
	testObj->YOffset = LuaGetFloatFromTableStackiKey(L, 2, "y");
	testObj->Location.w = LuaGetFloatFromTableStackiKey(L, 2, "w");
	testObj->Location.h = LuaGetFloatFromTableStackiKey(L, 2, "h");
	testObj->Parent = LuaGetLightUserdatai(L, 3);
	testObj->Flags |= UIObjectFlagActive | UIObjectFlagVisible | UIObjectFlagDirty;
	testObj->Id = _nextObjectId;
	return testObj;
}

static int createPanel(lua_State* state) {
	// args - name, loc table, parent userdata
	if (LuaGetStackSize(state) != 3 || !LuaIsString(state, 1) || !LuaIsTable(state, 2)) {
		sgLogError("Could not create panel from lua, bad params");
		LuaPushNil(state);
		return 1;
	}
	UIObject* obj = createUIObject(state);
	obj->Type = UIObjectTypesPanel;
	LuaPushLightUserdata(state, obj);
	AddUIObject(obj, obj->Parent);
	return 1;
}

static int createImage(lua_State* state) {
	// args - name, loc table, parent userdata, filename, src rect
	if (LuaGetStackSize(state) != 6 || !LuaIsString(state, 1) || !LuaIsTable(state, 2) || !LuaIsString(state, 4) || !(LuaIsTable(state, 5) || lua_isnil(state, 5)) || !LuaIsInt(state, 6)) {
		sgLogError("Could not create image from lua, bad params");
		LuaPushNil(state);
		return 1;
	}
	UIObject* obj = createUIObject(state);
	obj->Type = UIObjectTypesImage;
	UIImageData* image = SDL_malloc(sizeof(*image));
	image->Texture = CreateTextureFromIndexedBMP(LuaGetStringi(state, 4));
	obj->Data = image;
	if (!LuaIsNili(state, 5)) {
		image->SrcRect.x = LuaGetFloatFromTableStackiKey(state, 5, "x");
		image->SrcRect.y = LuaGetFloatFromTableStackiKey(state, 5, "y");
		image->SrcRect.w = LuaGetFloatFromTableStackiKey(state, 5, "w");
		image->SrcRect.h = LuaGetFloatFromTableStackiKey(state, 5, "h");
	}
	SetTextureAlpha(image->Texture, LuaGetIntFromStacki(state, 6));
	AddUIObject(obj, obj->Parent);
	LuaPushLightUserdata(state, obj);
	return 1;
}

static int createImageAnimator(lua_State* L) {
	// args - name, loc table, parent userdata, filename, src rect
	if (LuaGetStackSize(L) != 6 || !LuaIsString(L, 1) || !LuaIsTable(L, 2) || !LuaIsString(L, 4) || !(LuaIsTable(L, 5) || lua_isnil(L, 5)) || !LuaIsInt(L, 6)) {
		sgLogError("Could not create image from lua, bad params");
		LuaPushNil(L);
		return 1;
	}
	UIObject* obj = createUIObject(L);
	obj->Type = UIObjectTypesImageAnimator;
	UIImageAnimationData* imageAnimation = SDL_malloc(sizeof(*imageAnimation));
	obj->Data = imageAnimation;
	imageAnimation->SpritePtr = NULL;
	imageAnimation->AnimatorHandle = 9999;
	Sprite* sprite = NewSprite();
	sprite->Texture = CreateTextureFromIndexedBMP(LuaGetStringi(L, 4));
	sprite->Parent = NULL;
	sprite->TextureSourceRect.x = LuaGetFloatFromTableStackiKey(L, 5, "x");
	sprite->TextureSourceRect.y = LuaGetFloatFromTableStackiKey(L, 5, "y");
	sprite->TextureSourceRect.h = LuaGetFloatFromTableStackiKey(L, 5, "h");
	sprite->TextureSourceRect.w = LuaGetFloatFromTableStackiKey(L, 5, "w");
	sprite->OffsetAndSizeRectF.x = LuaGetFloatFromTableStackiKey(L, 5, "x");
	sprite->OffsetAndSizeRectF.y = LuaGetFloatFromTableStackiKey(L, 5, "y");
	sprite->OffsetAndSizeRectF.h = LuaGetFloatFromTableStackiKey(L, 5, "h");
	sprite->OffsetAndSizeRectF.w = LuaGetFloatFromTableStackiKey(L, 5, "w");
	SET_FLAG(sprite->Flags, SpriteFlagVisible | SpriteFlagUI);
	// AnimatorHandle animHandle = CreateAnimator(LuaGetStringi(L, 4));
	imageAnimation->AnimatorHandle = CreateAnimator(LuaGetStringi(L, 4));
	Animator* anim = &_animators.Animators[imageAnimation->AnimatorHandle];
	anim->Sprite = sprite;
	anim->AnimationSpeed = 1.0f;
	imageAnimation->SpritePtr = sprite;
	SetTextureAlpha(sprite->Texture, LuaGetIntFromStacki(L, 6));
	AddUIObject(obj, obj->Parent);
	LuaPushLightUserdata(L, obj);
	return 1;
}

static int create9SliceImage(lua_State* L) {
	// args - name, loc table, parent userdata, filename, alpha
	if (!LuaCheckFunctionCallParamsAndTypes(L, 5, LuaFunctionParameterTypeString, LuaFunctionParameterTypeTable, LuaFunctionParameterTypePass, LuaFunctionParameterTypeString, LuaFunctionParameterTypeTable)) {
		LuaPushNil(L);
		return 1;
	}
	UIObject* obj = createUIObject(L);
	obj->Type = UIObjectTypesImage;
	UIImageData* renderTargetImageData = SDL_malloc(sizeof(*renderTargetImageData));
	Texture* renderTargetTexture = CreateRenderTargetTexture(obj->Location.w, obj->Location.h, (sgColor){255, 255, 255, 255});
	Texture* nineSliceImageTexture = CreateTextureFromIndexedBMP(LuaGetStringi(L, 4));
	renderTargetImageData->Texture = renderTargetTexture;
	obj->Data = renderTargetImageData;
	float nineSliceImageW;
	float nineSliceImageH;
	int r;
	int g;
	int b;
	int a;
	r = LuaGetFloatFromTableStackiKey(L, 5, "r");
	g = LuaGetFloatFromTableStackiKey(L, 5, "g");
	b = LuaGetFloatFromTableStackiKey(L, 5, "b");
	a = LuaGetFloatFromTableStackiKey(L, 5, "a");
	TextureSize(nineSliceImageTexture, &nineSliceImageW, &nineSliceImageH);
	SetTextureAlpha(renderTargetTexture, a);
	ClearRenderTargetTexture(renderTargetTexture, &(sgColor){r, g, b, a});
	renderTargetImageData->SrcRect = (RectangleF){0, 0, obj->Location.w, obj->Location.h};
	// renderTargetImageData->SrcRect = (RectangleF){0, 0, nineSliceImageW, nineSliceImageH};
	// ClearRenderTargetTexture likely not needed
	// TODO this should be param, as this is the size of the 9 slices
	float sizeX = 8;
	float sizeY = 9;
	// / Draw the corners
	// tl
	RectangleF srcRect = {0, 0, sizeX, sizeY};
	RectangleF dstRect = {0, 0, sizeX, sizeY};
	DrawTextureToRenderTargetTexture(renderTargetTexture, nineSliceImageTexture, &dstRect, &srcRect);
	// tr
	srcRect = (RectangleF){nineSliceImageW - sizeX, 0, sizeX, sizeY};
	dstRect = (RectangleF){obj->Location.w - sizeX, 0, sizeX, sizeY};
	// dstRect = (RectangleF){64 - sizeX, 0, sizeX, sizeY};
	DrawTextureToRenderTargetTexture(renderTargetTexture, nineSliceImageTexture, &dstRect, &srcRect);
	// // bl
	srcRect = (RectangleF){0, nineSliceImageH - sizeY, sizeX, sizeY};
	dstRect = (RectangleF){0, obj->Location.h - sizeY, sizeX, sizeY};
	DrawTextureToRenderTargetTexture(renderTargetTexture, nineSliceImageTexture, &dstRect, &srcRect);
	// br
	srcRect = (RectangleF){nineSliceImageW - sizeX, nineSliceImageH - sizeY, sizeX, sizeY};
	dstRect = (RectangleF){obj->Location.w - sizeX, obj->Location.h - sizeY, sizeX, sizeY};
	DrawTextureToRenderTargetTexture(renderTargetTexture, nineSliceImageTexture, &dstRect, &srcRect);
	// draw the bars
	int length = obj->Location.w - (sizeX);
	int height = obj->Location.h - (sizeY);
	// top
	srcRect = (RectangleF){1 + sizeX, 0, 1, sizeY};
	for (size_t i = sizeX; i < length; i++) {
		dstRect = (RectangleF){(float)i, 0, 1, sizeY};
		DrawTextureToRenderTargetTexture(renderTargetTexture, nineSliceImageTexture, &dstRect, &srcRect);
	}
	// bottom
	for (size_t i = sizeX; i < length; i++) {
		dstRect = (RectangleF){(float)i, obj->Location.h - sizeY + 4, 1, sizeY};
		DrawTextureToRenderTargetTexture(renderTargetTexture, nineSliceImageTexture, &dstRect, &srcRect);
	}
	// left
	srcRect = (RectangleF){0, sizeY + 1, sizeX, 1};
	for (size_t i = sizeY; i < height; i++) {
		dstRect = (RectangleF){0, (float)i, sizeX, 1};
		DrawTextureToRenderTargetTexture(renderTargetTexture, nineSliceImageTexture, &dstRect, &srcRect);
	}
	// right
	for (size_t i = sizeY; i < height; i++) {
		dstRect = (RectangleF){obj->Location.w - sizeX + 3, (float)i, sizeX, 1};
		DrawTextureToRenderTargetTexture(renderTargetTexture, nineSliceImageTexture, &dstRect, &srcRect);
	}
	UnloadTexture(nineSliceImageTexture);
	AddUIObject(obj, obj->Parent);
	LuaPushLightUserdata(L, obj);
	return 1;
}

static int createText(lua_State* L) {
	// args - name, loc table, parent userdata, text string,
	// text fontName, int font size, bool centerX, centerY, wordwrap
	if (LuaGetStackSize(L) != 10 || !LuaIsString(L, 1) || !LuaIsTable(L, 2) ||
		!LuaIsString(L, 4) || !LuaIsString(L, 5) || !LuaIsInt(L, 6) || !LuaIsBool(L, 7) ||
		!LuaIsBool(L, 8) || !LuaIsBool(L, 9) || !LuaIsTable(L, 10)) {
		sgLogError("Could not create text from lua, bad params");
		LuaPushNil(L);
		return 1;
	}
	UIObject* obj = createUIObject(L);
	obj->Type = UIObjectTypesText;
	SetFont(LuaGetStringi(L, 5), LuaGetIntFromStacki(L, 6));
	UIText* textData = SDL_calloc(1, sizeof(*textData));
	obj->Data = textData;
	textData->CenteredX = LuaGetBooli(L, 7);
	textData->CenteredY = LuaGetBooli(L, 8);
	textData->WordWrap = LuaGetBooli(L, 9);
	textData->FontSize = LuaGetIntFromStacki(L, 6);
	textData->NumLettersToDraw = UINT_MAX;
	textData->Text = LuaAllocateStringStack(L, 4);
	AddUIObject(obj, obj->Parent);
	textData->Color.R = (uint8_t)LuaGetFloatFromTableStackiKey(L, 10, "r");
	textData->Color.G = (uint8_t)LuaGetFloatFromTableStackiKey(L, 10, "g");
	textData->Color.B = (uint8_t)LuaGetFloatFromTableStackiKey(L, 10, "b");
	textData->Color.A = (uint8_t)LuaGetFloatFromTableStackiKey(L, 10, "a");
	LuaPushLightUserdata(L, obj);
	return 1;
}
static int createRect(lua_State* L) {
	// args - name, loc table, parent userdata, table color
	if (LuaGetStackSize(L) != 4 || !LuaIsString(L, 1) || !LuaIsTable(L, 2) ||
		!LuaIsTable(L, 4)) {
		sgLogError("Could not create rect from lua, bad params");
		LuaPushNil(L);
		return 1;
	}
	UIObject* obj = createUIObject(L);
	obj->Type = UIObjectTypesRect;
	UIRect* rectData = SDL_calloc(1, sizeof(*rectData));
	rectData->Color.R = (uint8_t)LuaGetFloatFromTableStackiKey(L, 4, "r");
	rectData->Color.G = (uint8_t)LuaGetFloatFromTableStackiKey(L, 4, "g");
	rectData->Color.B = (uint8_t)LuaGetFloatFromTableStackiKey(L, 4, "b");
	rectData->Color.A = (uint8_t)LuaGetFloatFromTableStackiKey(L, 4, "a");
	obj->Data = rectData;
	AddUIObject(obj, obj->Parent);
	LuaPushLightUserdata(L, obj);
	return 1;
}

static int createLayoutGroup(lua_State* L) {
	// args - name, loc table, parent userdata, spacing int, bool IsHorizontal
	if (LuaGetStackSize(L) != 5 || !LuaIsString(L, 1) || !LuaIsTable(L, 2) || !LuaIsInt(L, 4) || !LuaIsBool(L, 5)) {
		sgLogError("Could not create layoutGroup from lua, bad params");
		LuaPushNil(L);
		return 1;
	}
	UIObject* obj = createUIObject(L);
	obj->Type = UIObjectTypesLayoutGroup;
	UILayoutGroup* layoutGroupData = SDL_calloc(1, sizeof(*layoutGroupData));
	obj->Data = layoutGroupData;
	layoutGroupData->Spacing = LuaGetIntFromStacki(L, 4);
	layoutGroupData->IsHorizontal = LuaGetBooli(L, 5);
	AddUIObject(obj, obj->Parent);
	LuaPushLightUserdata(L, obj);
	return 1;
}

static void callButtonFuncByIndex(LuaState L, int objId, int index, int argCount) {
	LuaRegistryGetSubTableEntry(L, LUA_BUTTON_FUNCS_KEY, objId);  // Push the function table from the lua registry
	if (!LuaIsTable(L, -1)) {
		sgLogWarn("There is no table on the stack, returning");
		return;
	}
	LuaGetLuaFuncAtIndex(L, index);	 // Push the function to the top of the stack
	LuaRemoveIndex(L, -2);			 // Remove the table with the funcs
	int pushLoc = LuaGetStackSize(L) - argCount;
	LuaMoveStackTipToIndex(L, pushLoc);
	RunLuaFunctionOnStack(L, argCount);
}

static void buttonPress(UIObject* obj, int justPressed) {
	if (obj) {
		lua_pushlightuserdata(_luaState, obj);			  // arg
		lua_pushboolean(_luaState, justPressed);		  // arg
		callButtonFuncByIndex(_luaState, obj->Id, 1, 2);  // index 1 = click, 0 args
	}
}

static void buttonHover(UIObject* obj, int justHovered) {
	if (obj) {
		lua_pushlightuserdata(_luaState, obj);			  // arg
		lua_pushboolean(_luaState, justHovered);		  // arg
		callButtonFuncByIndex(_luaState, obj->Id, 2, 2);  // index 2 = hover, 1 arg
	}
}

static int createButton(lua_State* L) {
	// args - name, loc table, parent userdata, funcsTable
	if (LuaGetStackSize(L) != 5 || !LuaIsString(L, 1) || !LuaIsTable(L, 2) || !LuaIsTable(L, 4) || !LuaIsBool(L, 5)) {
		sgLogError("Could not create button from lua, bad params passed in");
		LuaPushNil(L);
		return 1;
	}
	UIObject* obj = createUIObject(L);
	obj->Type = UIObjectTypesButton;
	UIButton* buttonData = SDL_calloc(1, sizeof(*buttonData));
	obj->Data = buttonData;
	buttonData->MouseOverLastFrame = false;
	buttonData->ClickOnRelease = LuaGetBooli(L, 5);
	LuaRegistrySetSubTableEntry(L, LUA_BUTTON_FUNCS_KEY, obj->Id, 4);
	buttonData->ButtonClickEvent = buttonPress;
	buttonData->ButtonHoverEvent = buttonHover;
	AddUIObject(obj, obj->Parent);
	LuaPushLightUserdata(L, obj);
	return 1;
}

static int getUIObjectLocation(lua_State* L) {
	UIObject* object = LuaGetLightUserdatai(L, 1);
	if (object == NULL) {
		LuaPushNil(L);
		sgLogWarn("Trying to get a bad object from uiobjectlocation");
		return 1;
	}
	LuaPushFloat(L, object->Location.x);
	LuaPushFloat(L, object->Location.y);
	return 2;
}

static int getUIObjectSize(lua_State* L) {
	UIObject* object = LuaGetLightUserdatai(L, 1);
	if (object == NULL) {
		LuaPushNil(L);
		sgLogWarn("Trying to get a bad object from uiobjectlocation");
		return 1;
	}
	LuaPushFloat(L, object->Location.w);
	LuaPushFloat(L, object->Location.h);
	return 2;
}

static int playUIAnimation(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 2, LuaFUnctionParameterTypeUserdata, LuaFunctionParameterTypeString)) {
		sgLogWarn("Bad params sent to play animation");
	}
	UIObject* object = LuaGetLightUserdatai(L, 1);
	if (!object || object->Type != UIObjectTypesImageAnimator) {
		sgLogWarn("Bad object sent in to play ui animation");
		return 0;
	}
	UIImageAnimationData* animData = (UIImageAnimationData*)object->Data;
	if (!animData) {
		sgLogWarn("Bad anim data in uiobject");
		return 0;
	}
	PlayAnimation(animData->AnimatorHandle, LuaGetStringi(L, 2));
	return 0;
}

static int setUIObjectLocation(lua_State* L) {
	if (LuaGetStackSize(L) != 3 || !LuaIsFloat(L, 2) || !LuaIsFloat(L, 3)) {
		sgLogError("Could not set object location");
		return 0;
	}
	UIObject* object = LuaGetLightUserdatai(L, 1);
	if (object == NULL) {
		sgLogWarn("Trying to get a bad object from uiobjectlocation");
		return 0;
	}
	// object->XOffset = object->Parent->Location.x - LuaGetFloati(2);
	// object->YOffset = object->Parent->Location.y - LuaGetFloati(3);
	object->XOffset = LuaGetFloati(L, 2) - object->Parent->Location.x;
	object->YOffset = LuaGetFloati(L, 3) - object->Parent->Location.y;
	object->Flags |= UIObjectFlagDirty;
	return 0;
}

static int destroyObject(lua_State* L) {
	if (LuaGetStackSize(L) != 1) {
		sgLogWarn("Bad destroy panel params");
		return 0;
	}
	UIObject* freeThing = LuaGetLightUserdatai(L, 1);
	if (freeThing) {
		UIObjectFree(freeThing);
	}
	return 0;
}

static int updateText(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 2, LuaFunctionParameterTypePass, LuaFunctionParameterTypeString)) {
		goto end;
	}
	UIObject* freeThing = LuaGetLightUserdatai(L, 1);
	if (!freeThing || freeThing->Type != UIObjectTypesText || !freeThing->Name) {
		sgLogWarn("Pased bad ui object to update text");
		goto end;
	}
	UIText* text = (freeThing->Data);
	if (!text) {
		sgLogWarn("Bad text in uiobject");
		goto end;
	}
	if (text->Text) {
		SDL_free(text->Text);
	}
	text->Text = LuaAllocateStringStack(L, 2);
	SDL_DestroyTexture(text->Texture);
	freeThing->Flags |= UIObjectFlagDirty;

end:
	LuaPushNil(L);
	return 1;
}

static int textSetNumLetters(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 2, LuaFunctionParameterTypePass, LuaFunctionParameterTypeInt)) {
		goto end;
	}
	UIObject* freeThing = LuaGetLightUserdatai(L, 1);
	if (!freeThing || freeThing->Type != UIObjectTypesText) {
		sgLogWarn("Pased bad ui object to update text");
		goto end;
	}
	UIText* text = (freeThing->Data);
	text->NumLettersToDraw = LuaGetIntFromStacki(L, 2);
	if (!text) {
		sgLogWarn("Bad text in uiobject");
		goto end;
	}
	text->NumLettersToDraw = LuaGetIntFromStacki(L, 2);
	freeThing->Flags |= UIObjectFlagDirty;

end:
	LuaPushNil(L);
	return 1;
}

static int textSetSize(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 2, LuaFunctionParameterTypePass, LuaFunctionParameterTypeInt)) {
		goto end;
	}
	UIObject* uiObject = LuaGetLightUserdatai(L, 1);
	if (!uiObject || uiObject->Type != UIObjectTypesText) {
		sgLogWarn("Pased bad ui object to update text size");
		goto end;
	}
	SetTextSize(uiObject, LuaGetIntFromStacki(L, 2));
end:
	LuaPushNil(L);
	return 1;
}

static int setUIObjectVisible(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 2, LuaFunctionParameterTypePass, LuaFunctionParameterTypeBoolean)) {
		goto end;
	}
	UIObject* freeThing = LuaGetLightUserdatai(L, 1);
	if (!freeThing) {
		sgLogWarn("Pased bad ui object to update text");
		goto end;
	}
	if (LuaGetBooli(L, 2)) {
		freeThing->Flags |= UIObjectFlagVisible;

	} else {
		freeThing->Flags &= ~UIObjectFlagVisible;
	}

end:
	LuaPushNil(L);
	return 1;
}

static const luaL_Reg uiLib[] = {
	{"CreatePanel", createPanel},
	{"CreateImage", createImage},
	{"CreateImageAnimator", createImageAnimator},
	{"Create9SliceImage", create9SliceImage},
	{"CreateText", createText},
	{"CreateRect", createRect},
	{"CreateLayoutGroup", createLayoutGroup},
	{"CreateButton", createButton},
	{"GetObjectLocation", getUIObjectLocation},
	{"GetObjectSize", getUIObjectSize},
	{"SetObjectLocation", setUIObjectLocation},
	{"UpdateText", updateText},
	{"PlayAnimation", playUIAnimation},
	{"TextSetNumLetters", textSetNumLetters},
	{"SetTextSize", textSetSize},
	{"SetObjectVisible", setUIObjectVisible},
	{"DestroyUIObject", destroyObject},
	{NULL, NULL}};

void RegisterLuaUIFunctions(void) {
	initButtonFuncTable(_luaState);
	luaL_newlib(_luaState, uiLib);
	lua_setglobal(_luaState, "cUI");
}
