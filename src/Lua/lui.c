#include <Supergoon/UI/ui.h>
#include <Supergoon/UI/uibutton.h>
#include <Supergoon/UI/uiimage.h>
#include <Supergoon/UI/uilayoutgroup.h>
#include <Supergoon/UI/uiobject.h>
#include <Supergoon/UI/uirect.h>
#include <Supergoon/UI/uitext.h>
#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <SupergoonEngine/Lua/ui.h>
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

// return cUI.CreateImage(name, rect, parentPanel, filename, srcRect)
static int createImage(lua_State* state) {
	// args - name, loc table, parent userdata, filename, src rect
	if (LuaGetStackSize(state) != 5 || !LuaIsString(state, 1) || !LuaIsTable(state, 2) || !LuaIsString(state, 4) || !(LuaIsTable(state, 5) || lua_isnil(state, 5))) {
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
	AddUIObject(obj, obj->Parent);
	LuaPushLightUserdata(state, obj);
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

static void buttonPress(UIObject* obj) {
	if (obj) {
		lua_pushlightuserdata(_luaState, obj);			  // arg
		callButtonFuncByIndex(_luaState, obj->Id, 1, 1);  // index 1 = click, 0 args
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
	if (LuaGetStackSize(L) != 4 || !LuaIsString(L, 1) || !LuaIsTable(L, 2) || !LuaIsTable(L, 4)) {
		sgLogError("Could not create button from lua, bad params passed in");
		LuaPushNil(L);
		return 1;
	}
	UIObject* obj = createUIObject(L);
	obj->Type = UIObjectTypesButton;
	UIButton* buttonData = SDL_calloc(1, sizeof(*buttonData));
	obj->Data = buttonData;
	buttonData->MouseOverLastFrame = false;
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

static const luaL_Reg uiLib[] = {
	{"CreatePanel", createPanel},
	{"CreateImage", createImage},
	{"CreateText", createText},
	{"CreateRect", createRect},
	{"CreateLayoutGroup", createLayoutGroup},
	{"CreateButton", createButton},
	{"GetObjectLocation", getUIObjectLocation},
	{"GetObjectSize", getUIObjectSize},
	{"SetObjectLocation", setUIObjectLocation},
	{"DestroyUIObject", destroyObject},
	{NULL, NULL}};

void RegisterLuaUIFunctions(void) {
	initButtonFuncTable(_luaState);
	luaL_newlib(_luaState, uiLib);
	lua_setglobal(_luaState, "cUI");
}
