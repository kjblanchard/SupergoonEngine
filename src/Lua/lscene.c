#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <Supergoon/map.h>
#include <SupergoonEngine/Lua/object.h>
#include <SupergoonEngine/Lua/scene.h>
#include <SupergoonEngine/gameobject.h>
#include <lauxlib.h>
#include <lua.h>

static int loadMap(lua_State* L) {
	if (LuaGetStackSize() != 1 || !LuaIsString(1)) {
		sgLogWarn("Bad parameters passed into load map from lua");
		return 0;
	}
	LoadMap(LuaGetStringi(1));
	return 0;
}

static int loadObjectsFromMap(lua_State* L) {
	LoadObjectsFromMap();
	return 0;
}

static const luaL_Reg sceneLib[] = {
	{"LoadMap", loadMap},
	{"LoadObjectsOnMap", loadObjectsFromMap},
	{NULL, NULL}};

void RegisterLuaSceneFuncs(void) {
	luaL_newlib(_luaState, sceneLib);
	lua_setglobal(_luaState, "cScene");
}
