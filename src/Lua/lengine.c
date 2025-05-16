#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <Supergoon/map.h>
#include <Supergoon/window.h>
#include <SupergoonEngine/Lua/engine.h>
#include <SupergoonEngine/Lua/object.h>
#include <SupergoonEngine/Lua/scene.h>
#include <SupergoonEngine/gameobject.h>
#include <SupergoonEngine/window.h>
#include <lauxlib.h>
#include <lua.h>

static int setWindowOptions(lua_State* L) {
	if (LuaGetStackSize() != 3 || !LuaIsInt(1) || !LuaIsInt(2) || !LuaIsString(3)) {
		sgLogWarn("Bad parameters passed into window options from lua");
		return 0;
	}
	SetWindowOptions(LuaGetIntFromStacki(1), LuaGetIntFromStacki(2), LuaGetStringi(3));
	return 0;
}

static int setScalingOptions(lua_State* L) {
	if (LuaGetStackSize() != 2 || !LuaIsInt(1) || !LuaIsInt(2)) {
		sgLogWarn("Bad parameters passed into scaling options from lua");
		return 0;
	}
	SetScalingOptions(LuaGetIntFromStacki(1), LuaGetIntFromStacki(2));
	return 0;
}

static const luaL_Reg sceneLib[] = {
	{"SetWindowOptions", setWindowOptions},
	{"SetScalingOptions", setScalingOptions},
	{NULL, NULL}};

void RegisterLuaEngineFunctions(void) {
	luaL_newlib(_luaState, sceneLib);
	lua_setglobal(_luaState, "cEngine");
}
