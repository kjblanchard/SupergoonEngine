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

static int getDeltaTime(lua_State* L) {
	LuaPushFloat(DeltaTimeSeconds);
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
	if (LuaGetStackSize() != 1 || !LuaIsLuaFunc(1)) {
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
