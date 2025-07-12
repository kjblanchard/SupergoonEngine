#include <Supergoon/Audio/Audio.h>
#include <Supergoon/engine.h>
#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <Supergoon/map.h>
#include <Supergoon/state.h>
#include <Supergoon/window.h>
#include <SupergoonEngine/Lua/engine.h>
#include <SupergoonEngine/Lua/object.h>
#include <SupergoonEngine/Lua/scene.h>
#include <SupergoonEngine/camera.h>
#include <SupergoonEngine/gameobject.h>
#include <SupergoonEngine/map.h>
#include <SupergoonEngine/window.h>
#include <lauxlib.h>
#include <lua.h>

static int _inputFuncRef = LUA_REFNIL;
static int _updateFuncRef = LUA_REFNIL;
static int _drawFuncRef = LUA_REFNIL;

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
	int logX = LuaGetIntFromStacki(L, 1);
	int logY = LuaGetIntFromStacki(L, 2);
	SetScalingOptions(logX, logY);
	// TODO, prolly should be it's own function too.
	SetCameraSize(logX, logY);
	return 0;
}

static void inputFunc(void) {
	if (_inputFuncRef != LUA_REFNIL) {
		lua_rawgeti(_luaState, LUA_REGISTRYINDEX, _inputFuncRef);
		if (lua_pcall(_luaState, 0, 0, 0) != LUA_OK) {
			const char* err = lua_tostring(_luaState, -1);
			fprintf(stderr, "Error in input func: %s\n", err);
			lua_pop(_luaState, 1);
		}
	}
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

static void drawFunc(void) {
	if (_drawFuncRef != LUA_REFNIL) {
		lua_rawgeti(_luaState, LUA_REGISTRYINDEX, _drawFuncRef);
		if (lua_pcall(_luaState, 0, 0, 0) != LUA_OK) {
			const char* err = lua_tostring(_luaState, -1);
			fprintf(stderr, "Error in update func: %s\n", err);
			lua_pop(_luaState, 1);
		}
	}
}

static int setInputFunc(lua_State* L) {
	if (LuaGetStackSize(L) != 1 || !LuaIsLuaFunc(L, 1)) {
		sgLogWarn("Bad parameters passed into setupda options from lua");
		return 0;
	}
	_inputFuncRef = luaL_ref(_luaState, LUA_REGISTRYINDEX);
	SetInputFunction(inputFunc);
	return 0;
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

static int setDrawFunc(lua_State* L) {
	if (LuaGetStackSize(L) != 1 || !LuaIsLuaFunc(L, 1)) {
		sgLogWarn("Bad parameters passed into setupda options from lua");
		return 0;
	}
	_drawFuncRef = luaL_ref(_luaState, LUA_REGISTRYINDEX);
	SetDrawFunction(drawFunc);
	return 0;
}

static int drawRect(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 4, LuaFunctionParameterTypeNumber, LuaFunctionParameterTypeNumber, LuaFunctionParameterTypeNumber, LuaFunctionParameterTypeNumber)) {
		sgLogWarn("Bad args passed into setDrawFunc");
	}
	DrawRect(&(RectangleF){LuaGetFloati(L, 1), LuaGetFloati(L, 2), LuaGetFloati(L, 3), LuaGetFloati(L, 4)}, &(sgColor){0, 255, 0, 255}, false);
	return 0;
}

static int drawRectCamOffset(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 4, LuaFunctionParameterTypeNumber, LuaFunctionParameterTypeNumber, LuaFunctionParameterTypeNumber, LuaFunctionParameterTypeNumber)) {
		sgLogWarn("Bad args passed into setDrawFunc");
	}
	DrawRect(&(RectangleF){LuaGetFloati(L, 1) - CameraX, LuaGetFloati(L, 2) - CameraY, LuaGetFloati(L, 3), LuaGetFloati(L, 4)}, &(sgColor){0, 255, 0, 255}, false);
	return 0;
}

static int getCurrentMapName(lua_State* L) {
	if (!_currentMap) {
		return 0;
	}
	LuaPushString(L, _currentMap->BaseFilename);
	return 1;
}

static const luaL_Reg sceneLib[] = {
	{"SetWindowOptions", setWindowOptions},
	{"SetScalingOptions", setScalingOptions},
	{"SetUpdateFunc", setUpdateFunc},
	{"SetInputFunc", setInputFunc},
	{"SetDrawFunc", setDrawFunc},
	{"DrawRect", drawRect},
	{"DrawRectCamOffset", drawRectCamOffset},
	{"MapName", getCurrentMapName},
	{NULL, NULL}};

void RegisterLuaEngineFunctions(void) {
	luaL_newlib(_luaState, sceneLib);
	lua_setglobal(_luaState, "cEngine");
}

void PushGamestateToLua(void) {
	lua_getglobal(_luaState, "cEngine");  // Push the cEngine table
	lua_pushnumber(_luaState, DeltaTimeSeconds);
	lua_setfield(_luaState, -2, "DeltaTimeSeconds");
	lua_pushnumber(_luaState, DeltaTimeMilliseconds);
	lua_setfield(_luaState, -2, "DeltaTimeMilliseconds");
	lua_pushinteger(_luaState, Ticks);
	lua_setfield(_luaState, -2, "Ticks");
	bool isMobile = false;
#if IS_MOBILE
	isMobile = true;
#endif
	lua_pushboolean(_luaState, isMobile);
	lua_setfield(_luaState, -2, "IsMobile");
	lua_pop(_luaState, 1);	// Pop the cEngine table
}
