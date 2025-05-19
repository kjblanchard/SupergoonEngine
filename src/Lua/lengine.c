#include <Supergoon/engine.h>
#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <Supergoon/map.h>
#include <Supergoon/state.h>
#include <Supergoon/window.h>
#include <SupergoonEngine/Lua/engine.h>
#include <SupergoonEngine/Lua/object.h>
#include <SupergoonEngine/Lua/scene.h>
#include <SupergoonEngine/gameobject.h>
#include <SupergoonEngine/window.h>
#include <lauxlib.h>
#include <lua.h>

static int _updateFuncRef = -1;

static int setWindowOptions(lua_State* L) {
	if (LuaGetStackSize(L) != 3 || !LuaIsInt(L, 1) || !LuaIsInt(L, 2) || !LuaIsString(L, 3)) {
		sgLogWarn("Bad parameters passed into window options from lua");
		return 0;
	}
	SetWindowOptions(LuaGetIntFromStacki(L, 1), LuaGetIntFromStacki(L, 2), LuaGetStringi(L, 3));
	return 0;
}

static int setScalingOptions(lua_State* L) {
	if (LuaGetStackSize(L) != 2 || !LuaIsInt(L, 1) || !LuaIsInt(L, 2)) {
		sgLogWarn("Bad parameters passed into scaling options from lua");
		return 0;
	}
	SetScalingOptions(LuaGetIntFromStacki(L, 1), LuaGetIntFromStacki(L, 2));
	return 0;
}

static int getDeltaTime(lua_State* L) {
	LuaPushFloat(L, DeltaTimeSeconds);
	return 1;
}

static void updateFunc(void) {
	if (_updateFuncRef != LUA_REFNIL) {
		lua_rawgeti(_luaState, LUA_REGISTRYINDEX, _updateFuncRef);
		if (lua_pcall(_luaState, 0, 0, 0) != LUA_OK) {
			const char* err = lua_tostring(_luaState, -1);
			fprintf(stderr, "Error in update func: %s\n", err);
			lua_pop(_luaState, 1);
		}
	}
}

static int setUpdateFunc(lua_State* L) {
	if (LuaGetStackSize(L) != 1 || !LuaIsLuaFunc(L, 1)) {
		sgLogWarn("Bad parameters passed into setupda options from lua");
		return 0;
	}
	_updateFuncRef = luaL_ref(_luaState, LUA_REGISTRYINDEX);
	SetUpdateFunction(updateFunc);
	return 0;
}

static const luaL_Reg sceneLib[] = {
	{"SetWindowOptions", setWindowOptions},
	{"SetScalingOptions", setScalingOptions},
	{"DeltaTime", getDeltaTime},
	{"SetUpdateFunc", setUpdateFunc},
	{NULL, NULL}};

void RegisterLuaEngineFunctions(void) {
	luaL_newlib(_luaState, sceneLib);
	lua_setglobal(_luaState, "cEngine");
}
