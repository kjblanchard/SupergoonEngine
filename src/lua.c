#include <SDL3/SDL_filesystem.h>
#include <Supergoon/log.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdio.h>
#include <string.h>

lua_State *_luaState;

static void setLuaPath(void) {
	int value = lua_getglobal(_luaState, "package");
	if (value == LUA_TNIL)
		sgLogCritical("Could not get lua package, what the");
	lua_getfield(_luaState, -1, "path");
	const char *basePath = SDL_GetBasePath();
	const char *nextPath = "assets/lua/?.lua;../Resources/assets/lua/?.lua";
	const char *currentLuaPath = lua_tostring(_luaState, -1);  // grab path string from top of stack
	size_t full_str_len = strlen(currentLuaPath) + strlen(nextPath) + strlen(basePath) + 2;
	char full_str[full_str_len];
	snprintf(full_str, full_str_len, "%s;%s%s", currentLuaPath, basePath, nextPath);
	lua_pop(_luaState, 1);
	lua_pushstring(_luaState, full_str);
	lua_setfield(_luaState, -2, "path");
	lua_pop(_luaState, 1);
}

void InitializeLuaEngine(void) {
	_luaState = luaL_newstate();
	if (_luaState == NULL) {
		sgLogCritical("Could not initialize Lua");
		return;
	}
	luaL_openlibs(_luaState);
	setLuaPath();
}

void LuaRunFile(const char *path) {
	const char *basePath = SDL_GetBasePath();
	size_t size = strlen(basePath) + strlen(path) + 1;
	char fullPath[size];
	snprintf(fullPath, size, "%s%s", basePath, path);

	if (luaL_dofile(_luaState, fullPath) != 0) {
		const char *luaError = lua_tostring(_luaState, -1);
		sgLogError("Lua error: %s", luaError);
	}
}

int LuaGetStackSize(void) {
	return lua_gettop(_luaState);
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

void LuaPushTableFromRegistryByName(const char *tableName) {
	lua_getfield(_luaState, LUA_REGISTRYINDEX, tableName);	// pushes the table onto the stack
	// Optionally, you can check it's a table if needed:
	if (!lua_istable(_luaState, -1)) {
		lua_pop(_luaState, 1);	// remove non-table
		luaL_error(_luaState, "Registry value '%s' is not a table", tableName);
	}
}

void LuaPushTableToStacki(int i) {
	if (!lua_istable(_luaState, i)) {
		sgLogWarn("Table pushed is not a real lua table, what even");
	}
	lua_pushvalue(_luaState, i);
}

void LuaPushTableToStack(const char *tableFieldName) {
	lua_getfield(_luaState, -1, tableFieldName);
	if (!lua_istable(_luaState, -1)) {
		sgLogWarn("Table pushed is not a real lua table, what did you do");
	}
}

int LuaGetTableLength(void) {
	// return lua_objlen(_luaState, -1);
	return lua_rawlen(_luaState, -1);
}

int LuaGetTableLengthMap(void) {
	int len = 0;
	lua_pushnil(_luaState);
	while (lua_next(_luaState, -2) != 0) {
		++len;
		lua_pop(_luaState, 1);
	}
	lua_pop(_luaState, 1);
	return len;
}

int LuaGetTablei(int i) {
	if (lua_istable(_luaState, i)) {
		return lua_gettable(_luaState, 0);
	} else {
		return 0;
	}
}

void LuaCopyString(const char *name, char *location, int strlen) {
	lua_getfield(_luaState, -1, name);
	strncpy(location, lua_tostring(_luaState, -1), strlen);
	lua_pop(_luaState, 1);
}

void LuaCopyStringStack(int stackLocation, char *location, int strlen) {
	strncpy(location, lua_tostring(_luaState, stackLocation), strlen);
}
char *LuaAllocateString(const char *fieldName) {
	lua_getfield(_luaState, -1, fieldName);
	const char *fieldString = lua_tostring(_luaState, -1);
	char *allocatedString = strdup(fieldString);
	lua_pop(_luaState, 1);
	return allocatedString;
}

char *LuaAllocateStringStack(int stackLocation) {
	const char *string = lua_tostring(_luaState, stackLocation);
	return strdup(string);
}

void LuaPushTableObjectToStacki(int i) {
	lua_rawgeti(_luaState, -1, i + 1);
}

void LuaPopStack(int num) {
	lua_pop(_luaState, num);
}

int LuaGetIntFromStack(void) {
	return lua_tointeger(_luaState, -1);
}
int LuaGetIntFromStacki(int i) {
	return lua_tointeger(_luaState, i);
}

int LuaGetBooli(int i) {
	return lua_toboolean(_luaState, i);
}

float LuaGetFloat(const char *field) {
	lua_getfield(_luaState, -1, field);
	float fieldFloat = lua_tonumber(_luaState, -1);
	lua_pop(_luaState, 1);
	return fieldFloat;
}

float LuaGetFloati(int i) {
	return lua_tonumber(_luaState, i);
}

float LuaGetFloatFromStack(void) {
	return lua_tonumber(_luaState, -1);
}

const char *LuaGetString(const char *name) {
	lua_getfield(_luaState, -1, name);
	const char *str = lua_tostring(_luaState, -1);
	lua_pop(_luaState, 1);
	return str;
}
const char *LuaGetStringi(int i) {
	return lua_tostring(_luaState, i);
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

void LuaStartTableKeyValueIteration(void) {
	lua_pushnil(_luaState);
	lua_pushnil(_luaState);
}

int LuaNextTableKeyValueIterate(void) {
	return lua_next(_luaState, -2);
}

void LuaEndTableKeyValueIteration(void) {
	// Removes the nil from the start table iteration
	lua_pop(_luaState, 1);
}
bool LuaIsString(int stackLocation) {
	return lua_isstring(_luaState, stackLocation);
}

bool LuaIsInt(int stackLocation) {
	return lua_isinteger(_luaState, stackLocation);
}

bool LuaIsFloat(int stackLocation) {
	return lua_isnumber(_luaState, stackLocation);
}

bool LuaIsTable(int stackLocation) {
	return lua_istable(_luaState, stackLocation);
}

float LuaGetFloatFromTableStacki(int i, const char *key) {
	lua_getfield(_luaState, i, key);
	float fieldFloat = lua_tonumber(_luaState, -1);
	lua_pop(_luaState, 1);
	return fieldFloat;
}

void LuaPushLightUserdata(void *data) {
	lua_pushlightuserdata(_luaState, data);
}

void LuaPushNil(void) {
	lua_pushnil(_luaState);
}

void *LuaGetLightUserdatai(int i) {
	return lua_isuserdata(_luaState, i) ? lua_touserdata(_luaState, i) : NULL;
}
int LuaIsNili(int stackLocation) {
	return lua_isnil(_luaState, stackLocation);
}
int LuaIsBool(int stackLocation) {
	return lua_isboolean(_luaState, stackLocation);
}

void LuaRegistrySetSubTableEntry(const char *registryKey, int subKey, int valueIndex) {
	lua_getfield(_luaState, LUA_REGISTRYINDEX, registryKey);  // registry table on top or nil
	lua_pushinteger(_luaState, subKey);						  // subkey to get from this
	lua_pushvalue(_luaState, valueIndex);					  // copy value (from anywhere on stack)
	lua_settable(_luaState, -3);							  // registry[subKey] = value
	lua_pop(_luaState, 1);									  // pop registry table
}

void LuaEnsureRegistryTable(const char *registryKey) {
	lua_getfield(_luaState, LUA_REGISTRYINDEX, registryKey);
	if (!lua_istable(_luaState, -1)) {
		lua_pop(_luaState, 1);	  // pop non-table
		lua_newtable(_luaState);  // create new table
		lua_setfield(_luaState, LUA_REGISTRYINDEX, registryKey);
	}
	lua_pop(_luaState, 1);
}

bool LuaRegistryGetSubTableEntry(const char *registryKey, int subKey) {
	lua_getfield(_luaState, LUA_REGISTRYINDEX, registryKey);  // push registry[registryKey]
	if (!lua_istable(_luaState, -1)) {
		lua_pop(_luaState, 1);
		return false;
	}
	lua_pushinteger(_luaState, subKey);
	lua_gettable(_luaState, -2);		// push registry[registryKey][subKey]
	lua_remove(_luaState, -2);			// remove registry table, leave just result
	return lua_istable(_luaState, -1);	// or lua_isfunction(), if expecting a func
}

// Table must be directly on the stack, index can be anywhere.
void LuaGetLuaFuncAtIndex(int index) {
	lua_rawgeti(_luaState, -1, index);	// get function at index (1 = click, 2 = hover)
	if (!lua_isfunction(_luaState, -1)) {
		sgLogWarn("No function was pushed, what the, pop everything");
		lua_pop(_luaState, 3);
		return;
	}
}

int LuaGetStack(void) {
	return lua_gettop(_luaState);
}

int LuaRemoveIndex(int index) {
	lua_remove(_luaState, index);
}

void RunLuaFunctionOnStack(int numArgs) {
	if (lua_pcall(_luaState, numArgs, 0, 0) != LUA_OK) {
		const char *err = lua_tostring(_luaState, -1);
		sgLogWarn("Button func error: %s\n", err);
		lua_pop(_luaState, 1);	// pop func
	}
}

void LuaMoveStackTipToIndex(int index) {
	lua_insert(_luaState, index);  // Rearrage the stack so that the function is before the actual arguments.
}

void LuaPushFloat(float data) {
	lua_pushnumber(_luaState, data);
}
