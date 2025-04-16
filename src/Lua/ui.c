#include <Supergoon/UI/ui.h>
#include <Supergoon/UI/uiimage.h>
#include <Supergoon/UI/uilayoutgroup.h>
#include <Supergoon/UI/uiobject.h>
#include <Supergoon/UI/uirect.h>
#include <Supergoon/UI/uitext.h>
#include <Supergoon/lua.h>
#include <SupergoonEngine/Lua/ui.h>
#include <SupergoonEngine/ui.h>
#include <lauxlib.h>
#include <lua.h>

static int createPanel(lua_State* L) {
	// args - name, loc table, parent userdata
	if (LuaGetStackSize() != 3 || !LuaIsString(1) || !LuaIsTable(2)) {
		LuaPushNil();
		return 0;
	}
	UIObject* testObj = SDL_calloc(1, sizeof(*testObj));
	testObj->Name = strdup(LuaGetStringi(1));
	testObj->XOffset = LuaGetFloatFromTableStacki(2, "x");
	testObj->YOffset = LuaGetFloatFromTableStacki(2, "y");
	testObj->Location.w = LuaGetFloatFromTableStacki(2, "w");
	testObj->Location.h = LuaGetFloatFromTableStacki(2, "h");
	testObj->Parent = LuaGetLightUserdatai(3);
	testObj->Flags |= UIObjectFlagActive | UIObjectFlagVisible | UIObjectFlagDirty;
	AddUIObject(testObj, testObj->Parent);
	LuaPushLightUserdata(testObj);
	return 1;
}

static const luaL_Reg uiLib[] = {
	{"CreatePanel", createPanel},
	{NULL, NULL}};

void RegisterLuaUIFunctions(void) {
	luaL_newlib(_luaState, uiLib);
	lua_setglobal(_luaState, "UI");
}
