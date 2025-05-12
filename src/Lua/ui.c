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

static UIObject* createUIObject(void) {
	UIObject* testObj = SDL_calloc(1, sizeof(*testObj));
	testObj->Name = strdup(LuaGetStringi(1));
	testObj->XOffset = LuaGetFloatFromTableStacki(2, "x");
	testObj->YOffset = LuaGetFloatFromTableStacki(2, "y");
	testObj->Location.w = LuaGetFloatFromTableStacki(2, "w");
	testObj->Location.h = LuaGetFloatFromTableStacki(2, "h");
	testObj->Parent = LuaGetLightUserdatai(3);
	testObj->Flags |= UIObjectFlagActive | UIObjectFlagVisible | UIObjectFlagDirty;
	return testObj;
}

static int createPanel(lua_State* state) {
	// args - name, loc table, parent userdata
	if (LuaGetStackSize() != 3 || !LuaIsString(1) || !LuaIsTable(2)) {
		sgLogError("Could not create panel from lua, bad params");
		LuaPushNil();
		return 1;
	}
	UIObject* obj = createUIObject();
	obj->Type = UIObjectTypesPanel;
	LuaPushLightUserdata(obj);
	AddUIObject(obj, obj->Parent);
	return 1;
}

// return cUI.CreateImage(name, rect, parentPanel, filename, srcRect)
static int createImage(lua_State* state) {
	// args - name, loc table, parent userdata, filename, src rect
	if (LuaGetStackSize() != 5 || !LuaIsString(1) || !LuaIsTable(2) || !LuaIsString(4) || !(LuaIsTable(5) || lua_isnil(_luaState, 5))) {
		sgLogError("Could not create image from lua, bad params");
		LuaPushNil();
		return 1;
	}
	UIObject* obj = createUIObject();
	obj->Type = UIObjectTypesImage;
	UIImageData* image = SDL_malloc(sizeof(*image));
	image->Texture = CreateTextureFromIndexedBMP(LuaGetStringi(4));
	obj->Data = image;
	if (!LuaIsNili(5)) {
		image->SrcRect.x = LuaGetFloatFromTableStacki(5, "x");
		image->SrcRect.y = LuaGetFloatFromTableStacki(5, "y");
		image->SrcRect.w = LuaGetFloatFromTableStacki(5, "w");
		image->SrcRect.h = LuaGetFloatFromTableStacki(5, "h");
	}
	AddUIObject(obj, obj->Parent);
	LuaPushLightUserdata(obj);
	return 1;
}

static int createText(lua_State* state) {
	// args - name, loc table, parent userdata, text string,
	// text fontName, int font size, bool centerX, centerY, wordwrap
	if (LuaGetStackSize() != 10 || !LuaIsString(1) || !LuaIsTable(2) ||
		!LuaIsString(4) || !LuaIsString(5) || !LuaIsInt(6) || !LuaIsBool(7) ||
		!LuaIsBool(8) || !LuaIsBool(9) || !LuaIsTable(10)) {
		sgLogError("Could not create text from lua, bad params");
		LuaPushNil();
		return 1;
	}
	UIObject* obj = createUIObject();
	obj->Type = UIObjectTypesText;
	SetFont(LuaGetStringi(5), LuaGetIntFromStacki(6));
	UIText* textData = SDL_calloc(1, sizeof(*textData));
	obj->Data = textData;
	textData->CenteredX = LuaGetBooli(7);
	textData->CenteredY = LuaGetBooli(8);
	textData->WordWrap = LuaGetBooli(9);
	textData->FontSize = LuaGetIntFromStacki(6);
	textData->NumLettersToDraw = UINT_MAX;
	textData->Text = LuaAllocateStringStack(4);
	AddUIObject(obj, obj->Parent);
	textData->Color.R = (uint8_t)LuaGetFloatFromTableStacki(10, "r");
	textData->Color.G = (uint8_t)LuaGetFloatFromTableStacki(10, "g");
	textData->Color.B = (uint8_t)LuaGetFloatFromTableStacki(10, "b");
	textData->Color.A = (uint8_t)LuaGetFloatFromTableStacki(10, "a");
	LuaPushLightUserdata(obj);
	return 1;
}
static int createRect(lua_State* state) {
	// args - name, loc table, parent userdata, table color
	if (LuaGetStackSize() != 4 || !LuaIsString(1) || !LuaIsTable(2) ||
		!LuaIsTable(4)) {
		sgLogError("Could not create rect from lua, bad params");
		LuaPushNil();
		return 1;
	}
	UIObject* obj = createUIObject();
	obj->Type = UIObjectTypesRect;
	UIRect* rectData = SDL_calloc(1, sizeof(*rectData));
	rectData->Color.R = (uint8_t)LuaGetFloatFromTableStacki(4, "r");
	rectData->Color.G = (uint8_t)LuaGetFloatFromTableStacki(4, "g");
	rectData->Color.B = (uint8_t)LuaGetFloatFromTableStacki(4, "b");
	rectData->Color.A = (uint8_t)LuaGetFloatFromTableStacki(4, "a");
	obj->Data = rectData;
	AddUIObject(obj, obj->Parent);
	LuaPushLightUserdata(obj);
	return 1;
}

static int createLayoutGroup(lua_State* state) {
	// args - name, loc table, parent userdata, spacing int, bool IsHorizontal
	if (LuaGetStackSize() != 5 || !LuaIsString(1) || !LuaIsTable(2) || !LuaIsInt(4) || !LuaIsBool(5)) {
		sgLogError("Could not create layoutGroup from lua, bad params");
		LuaPushNil();
		return 1;
	}
	UIObject* obj = createUIObject();
	obj->Type = UIObjectTypesLayoutGroup;
	UILayoutGroup* layoutGroupData = SDL_calloc(1, sizeof(*layoutGroupData));
	obj->Data = layoutGroupData;
	layoutGroupData->Spacing = LuaGetIntFromStacki(4);
	layoutGroupData->IsHorizontal = LuaGetBooli(5);
	AddUIObject(obj, obj->Parent);
	LuaPushLightUserdata(obj);
	return 1;
}

static void callButtonFuncByIndex(int objId, int index, int argCount) {
	lua_getfield(_luaState, LUA_REGISTRYINDEX, LUA_BUTTON_FUNCS_KEY);  // funcsTable
	lua_pushinteger(_luaState, objId);
	lua_gettable(_luaState, -2);  // funcsTable[objId]

	if (!lua_istable(_luaState, -1)) {
		lua_pop(_luaState, 2);
		return;
	}

	lua_rawgeti(_luaState, -1, index);	// get function at index (1 = click, 2 = hover)
	if (!lua_isfunction(_luaState, -1)) {
		lua_pop(_luaState, 3);
		return;
	}

	// Push args (if any already on the stack before calling this)
	if (lua_pcall(_luaState, argCount, 0, 0) != LUA_OK) {
		const char* err = lua_tostring(_luaState, -1);
		fprintf(stderr, "Button func error: %s\n", err);
		lua_pop(_luaState, 1);
	}

	lua_pop(_luaState, 2);	// pop funcsTable[objId] and funcsTable
}

static void buttonPress(UIObject* obj) {
	if (obj) {
		lua_pushlightuserdata(_luaState, obj);	// arg
		callButtonFuncByIndex(obj->Id, 1, 1);	// index 1 = click, 0 args
	}
}

static void buttonHover(UIObject* obj, int justHovered) {
	if (obj) {
		lua_pushlightuserdata(_luaState, obj);	  // arg
		lua_pushboolean(_luaState, justHovered);  // arg
		callButtonFuncByIndex(obj->Id, 2, 2);	  // index 2 = hover, 1 arg
	}
}

static int createButton(lua_State* state) {
	// args - name, loc table, parent userdata, funcsTable
	if (LuaGetStackSize() != 4 || !LuaIsString(1) || !LuaIsTable(2) || !LuaIsInt(4) || !LuaIsTable(5)) {
		sgLogError("Could not create button from lua, bad params passed in");
		LuaPushNil();
		return 1;
	}
	UIObject* obj = createUIObject();
	obj->Type = UIObjectTypesButton;
	UIButton* buttonData = SDL_calloc(1, sizeof(*buttonData));
	obj->Data = buttonData;
	buttonData->MouseOverLastFrame = false;
	// We need to get the functions from the stack, store them in lua ref if it's there, otherwise don't
	LuaPushTableToStacki(5);
	LuaPushTableFromRegistryByName(LUA_BUTTON_FUNCS_KEY);
	lua_pushinteger(_luaState, obj->Id);  // key
	lua_pushvalue(_luaState, -2);		  // value (funcsTable)
	lua_settable(_luaState, -3);		  // global[objId] = funcsTable
	lua_pop(_luaState, 1);				  // pop global table
	buttonData->ButtonClickEvent = buttonPress;
	buttonData->ButtonHoverEvent = buttonHover;
	AddUIObject(obj, obj->Parent);
	LuaPushLightUserdata(obj);
	return 1;
}

static const luaL_Reg uiLib[] = {
	{"CreatePanel", createPanel},
	{"CreateImage", createImage},
	{"CreateText", createText},
	{"CreateRect", createRect},
	{"CreateLayoutGroup", createLayoutGroup},
	{"CreateButton", createButton},
	{NULL, NULL}};

void RegisterLuaUIFunctions(void) {
	initButtonFuncTable(_luaState);
	luaL_newlib(_luaState, uiLib);
	lua_setglobal(_luaState, "cUI");
}
