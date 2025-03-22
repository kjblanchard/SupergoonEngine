#include <Supergoon/log.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

static lua_State *_luaState;

void InitializeLuaEngine(void) {
	_luaState = luaL_newstate();
	if (_luaState == NULL) {
		sgLogCritical("Could not initialize Lua");
		return;
	}
	luaL_openlibs(_luaState);
}

void LuaRunFile(const char *path) {
	const char *basePath = SDL_GetBasePath();
	size_t size = strlen(basePath) + strlen(path) + 1;
	char fullPath[size];
	snprintf(fullPath, size, "%s%s", basePath, path);
	if (luaL_dofile(_luaState, fullPath) != 0) {
		const char *luaError = lua_tostring(_luaState, -1);
		sgLogCritical("Lua error: %s", luaError);
	}
}

void LuaPushTableFromFile(const char *path) {
	LuaRunFile(path);
	if (!lua_istable(_luaState, -1)) {
		printf("map.lua did not return a table!\n");
		lua_pop(_luaState, 1);
	}
}

int LuaGetInt(const char *field) {
	lua_getfield(_luaState, -1, field);
	int fieldInt = lua_tointeger(_luaState, -1);
	lua_pop(_luaState, 1);
	return fieldInt;
}

void LuaPushTableToStack(const char *tableFieldName) {
	lua_getfield(_luaState, -1, tableFieldName);
	if (!lua_istable(_luaState, -1)) {
		sgLogWarn("Table pushed is not a real lua table, what did you do");
	}
}

int LuaGetTableLength(void) {
	return lua_objlen(_luaState, -1);
}

void LuaCopyString(const char *name, char *location, int strlen) {
	lua_getfield(_luaState, -1, name);
	strncpy(location, lua_tostring(_luaState, -1), strlen);
	lua_pop(_luaState, 1);
}

void LuaPushTableObjectToStacki(int i) {
	lua_rawgeti(_luaState, -1, i + 1);
}

void LuaPopStack(int num) {
	lua_pop(_luaState, num);
}

int LuaGetIntFromStack() {
	return lua_tointeger(_luaState, -1);
}

const char *LuaGetString(const char *name) {
	lua_getfield(_luaState, -1, name);
	const char *str = lua_tostring(_luaState, -1);
	lua_pop(_luaState, 1);
	return str;
}

int LuaGetIntFromTablei(int i) {
	lua_rawgeti(_luaState, -1, i + 1);
	int value = lua_tointeger(_luaState, -1);
	lua_pop(_luaState, 1);
	return value;
}

void LuaClearStack(void) {
	lua_settop(_luaState, 0);
}

void sgCloseLua(void) {
	lua_close(_luaState);
}
