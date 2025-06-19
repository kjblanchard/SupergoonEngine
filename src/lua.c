#include <SDL3/SDL_filesystem.h>
#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdio.h>
#include <string.h>

LuaState _luaState = NULL;

static void setLuaPath(void) {
	int value = lua_getglobal(_luaState, "package");
	if (value == LUA_TNIL)
		sgLogCritical("Could not get lua package, what the");
	lua_getfield(_luaState, -1, "path");
	const char* basePath = SDL_GetBasePath();
	const char* nextPath = "assets/lua/?.lua;../Resources/assets/lua/?.lua;assets/scripts/?.lua;../Resources/assets/scripts/?.lua";
	const char* currentLuaPath = lua_tostring(_luaState, -1);  // grab path string from top of stack
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

void LuaRunFile(const char* path) {
	const char* basePath = SDL_GetBasePath();
	size_t size = strlen(basePath) + strlen(path) + 1;
	char fullPath[size];
	snprintf(fullPath, size, "%s%s", basePath, path);

	if (luaL_dofile(_luaState, fullPath) != 0) {
		const char* luaError = lua_tostring(_luaState, -1);
		sgLogError("Lua error: %s", luaError);
	}
}

int LuaGetStackSize(LuaState L) {
	return lua_gettop(L);
}

void LuaPopStack(LuaState L, int num) {
	lua_pop(L, num);
}

void LuaClearStack(LuaState L) {
	lua_settop(L, 0);
}
// Gets the number of items on the stack, remember that the index starts at 1.
int LuaGetStack(LuaState L) {
	return lua_gettop(L);
}
void LuaRemoveIndex(LuaState L, int index) {
	lua_remove(L, index);
}
// Moves the tip to the index passed in, pushing everything else up.  Useful for if you pass func args first.
void LuaMoveStackTipToIndex(LuaState L, int index) {
	lua_insert(L, index);  // Rearrage the stack so that the function is before the actual arguments.
}
void sgCloseLua(void) {
	lua_close(_luaState);
}

// Tables
void LuaPushTableFromFile(LuaState L, const char* path) {
	LuaRunFile(path);
	if (!lua_istable(L, -1)) {
		printf("map.lua did not return a table!\n");
		lua_pop(L, 1);
	}
}
int LuaGetIntFromTablei(LuaState L, int i) {
	lua_rawgeti(L, -1, i + 1);
	int value = lua_tointeger(_luaState, -1);
	lua_pop(L, 1);
	return value;
}

const char* LuaGetStringFromTablei(LuaState L, int i) {
	lua_rawgeti(L, -1, i + 1);
	const char* value = lua_tostring(_luaState, -1);
	if (value == NULL) {
		sgLogDebug("nil value in string, returning empty ");
		value = "";
	}
	lua_pop(L, 1);
	return value;
}

float LuaGetFloatFromTablei(LuaState L, int i) {
	lua_rawgeti(L, -1, i + 1);
	float value = lua_tonumber(_luaState, -1);
	lua_pop(L, 1);
	return value;
}

void LuaPushNewTableToStack(LuaState L) {
	lua_newtable(L);
}

void LuaPushFloatToTable(LuaState L, const char* key, float value) {
	lua_pushstring(L, key);
	lua_pushnumber(L, value);
	lua_settable(L, -3);
}

void LuaPushIntToTable(LuaState L, const char* key, int value) {
	lua_pushstring(L, key);
	lua_pushinteger(L, value);
	lua_settable(L, -3);
}

void LuaPushStringToTable(LuaState L, const char* key, const char* value) {
	lua_pushstring(L, key);
	lua_pushstring(L, value);
	lua_settable(L, -3);
}

void LuaPushTableToTable(LuaState L, const char* key) {
	// lua_pushstring(L, key);
	// lua_pushvalue(L, -2);
	// lua_settable(L, -3);
	// // since we duplicated it with pushvalue, we should actually pop the table value too.
	// lua_pop(L, 1);

	// stack: [-1] properties table
	//        [-2] main table
	lua_pushstring(L, key);	 // [-1] key
	lua_insert(L, -2);		 // move key below value
	// stack is now:
	// [-1] properties table
	// [-2] key
	// [-3] main table
	lua_settable(L, -3);  // sets main_table[key] = properties
}
void LuaPushValueToArrayTable(LuaState L) {
	int len = lua_rawlen(L, -2);  // length of the array table (at index -2)
	// lua_pushvalue(L, -1);		  // duplicate the value to push
	lua_rawseti(L, -2, len + 1);  // t[len+1] = value
								  // since we duplicated it with pushvalue, we should actually pop the table value too.
								  // lua_pop(L, 1);
}

void LuaPushTableFromRegistryByName(LuaState L, const char* tableName) {
	lua_getfield(L, LUA_REGISTRYINDEX, tableName);	// pushes the table onto the stack
	// Optionally, you can check it's a table if needed:
	if (!lua_istable(L, -1)) {
		lua_pop(L, 1);	// remove non-table
		luaL_error(L, "Registry value '%s' is not a table", tableName);
	}
}
void LuaPushTableToStacki(LuaState L, int i) {
	if (!lua_istable(L, i)) {
		sgLogWarn("Table pushed is not a real lua table, what even");
	}
	lua_pushvalue(L, i);
}
// If a table is on the stack, you will push the table at i on the stack
// Remember lua table indexes start at 1, so this will add 1.
void LuaPushTableObjectToStacki(LuaState L, int i) {
	lua_rawgeti(L, -1, i + 1);
}
// Table must be on stack, and an array table
int LuaGetTableLength(LuaState L) {
	return lua_rawlen(L, -1);
}
// Table must be on stack, used for key/value tables
int LuaGetTableLengthMap(LuaState L) {
	if (!lua_istable(L, -1)) {
		sgLogWarn("Table is not on tip of stack!");
		return 0;
	}
	int len = 0;
	lua_pushnil(L);

	while (lua_next(L, -2) != 0) {
		++len;
		lua_pop(L, 1);
	}
	lua_pop(L, 1);
	return len;
}
void LuaGetTable(LuaState L, const char* tableFieldName) {
	lua_getfield(L, -1, tableFieldName);
	if (!lua_istable(L, -1)) {
		sgLogWarn("Table pushed is not a real lua table, what did you do");
	}
}
int LuaGetTablei(LuaState L, int i) {
	if (lua_istable(L, i)) {
		return lua_gettable(L, 0);
	} else {
		return 0;
	}
}
// Starts the table iteration for lua tables that are key/value pairs, returns if there is any more values left
// This pushes two values to the table for iteration, The key is -2, value is -1.  Key is used for iteration, so don't pop it off, use stopiteration when done, only pop value
void LuaStartTableKeyValueIteration(LuaState L) {
	lua_pushnil(L);
	lua_pushnil(L);
}
// Goes to the next item in the table, returns if there is any more values left
// You must pop the value off before next iteration.
int LuaNextTableKeyValueIterate(LuaState L) {
	return lua_next(L, -2);
}
// Ends the iteration
void LuaEndTableKeyValueIteration(LuaState L) {
	// Removes the nil from the start table iteration
	lua_pop(L, 1);
}

// Ints
int LuaGetInt(LuaState L, const char* field) {
	lua_getfield(L, -1, field);
	int fieldInt = lua_tointeger(L, -1);
	lua_pop(L, 1);
	return fieldInt;
}
int LuaGetIntFromStack(LuaState L) {
	return lua_tointeger(L, -1);
}
int LuaGetIntFromStacki(LuaState L, int i) {
	return lua_tointeger(L, i);
}
// Bools
int LuaGetBool(LuaState L, const char* field) {
	lua_getfield(L, -1, field);
	int fieldInt = lua_toboolean(L, -1);
	lua_pop(L, 1);
	return fieldInt;
}
int LuaGetBooli(LuaState L, int i) {
	return lua_toboolean(L, i);
}
// Floats
float LuaGetFloat(LuaState L, const char* field) {
	lua_getfield(L, -1, field);
	float fieldFloat = lua_tonumber(L, -1);
	lua_pop(L, 1);
	return fieldFloat;
}
float LuaGetFloati(LuaState L, int i) {
	return lua_tonumber(L, i);
}
float LuaGetFloatFromStack(LuaState L) {
	return lua_tonumber(L, -1);
}
float LuaGetFloatFromTableStackiKey(LuaState L, int i, const char* key) {
	lua_getfield(L, i, key);
	float fieldFloat = lua_tonumber(L, -1);
	lua_pop(L, 1);
	return fieldFloat;
}
// Strings
// i is stack location of table, const char* is key.. so not directly on stack -1.
void LuaCopyString(LuaState L, const char* name, char* location, int strlen) {
	lua_getfield(L, -1, name);
	strncpy(location, lua_tostring(L, -1), strlen);
	lua_pop(L, 1);
}
// Does not pop off, please do the needful
void LuaCopyStringStack(LuaState L, int stackLocation, char* location, int strlen) {
	strncpy(location, lua_tostring(L, stackLocation), strlen);
}
char* LuaAllocateString(LuaState L, const char* fieldName) {
	lua_getfield(L, -1, fieldName);
	const char* fieldString = lua_tostring(L, -1);
	char* allocatedString = strdup(fieldString);
	lua_pop(L, 1);
	return allocatedString;
}
// String at location must be freed, allocates memory to it.
char* LuaAllocateStringStack(LuaState L, int stackLocation) {
	const char* string = lua_tostring(L, stackLocation);
	return strdup(string);
}
const char* LuaGetString(LuaState L, const char* name) {
	lua_getfield(L, -1, name);
	const char* str = lua_tostring(L, -1);
	lua_pop(L, 1);
	return str;
}
const char* LuaGetStringi(LuaState L, int i) {
	return lua_tostring(L, i);
}
// Get light userdata at stack position i, or null if not userdata.
void* LuaGetLightUserdatai(LuaState L, int i) {
	return lua_isuserdata(L, i) ? lua_touserdata(L, i) : NULL;
}
// CheckStacks
int LuaIsString(LuaState L, int stackLocation) {
	return lua_isstring(L, stackLocation);
}
int LuaIsFloat(LuaState L, int stackLocation) {
	return lua_isnumber(L, stackLocation);
}
int LuaIsInt(LuaState L, int stackLocation) {
	return lua_isinteger(L, stackLocation);
}
int LuaIsTable(LuaState L, int stackLocation) {
	return lua_istable(L, stackLocation);
}
int LuaIsNili(LuaState L, int stackLocation) {
	return lua_isnil(L, stackLocation);
}
int LuaIsBool(LuaState L, int stackLocation) {
	return lua_isboolean(L, stackLocation);
}
int LuaIsLuaFunc(LuaState L, int stackLocation) {
	return lua_isfunction(L, stackLocation);
}
// Push to stack
void LuaPushBool(LuaState L, int boolean) {
	lua_pushboolean(L, boolean);
}
void LuaPushNil(LuaState L) {
	lua_pushnil(L);
}
void LuaPushLightUserdata(LuaState L, void* data) {
	lua_pushlightuserdata(L, data);
}
void LuaPushFloat(LuaState L, float data) {
	lua_pushnumber(L, data);
}

void LuaPushInt(LuaState L, int data) {
	lua_pushinteger(L, data);
}
void LuaPushString(LuaState L, const char* data) {
	lua_pushstring(L, data);
}
// Registry
void LuaEnsureRegistryTable(LuaState L, const char* registryKey) {
	lua_getfield(L, LUA_REGISTRYINDEX, registryKey);
	if (!lua_istable(L, -1)) {
		lua_pop(L, 1);	  // pop non-table
		lua_newtable(L);  // create new table
		lua_setfield(L, LUA_REGISTRYINDEX, registryKey);
	}
	lua_pop(L, 1);
}
int LuaRegistryGetSubTableEntry(LuaState L, const char* registryKey, int subKey) {
	lua_getfield(L, LUA_REGISTRYINDEX, registryKey);  // push registry[registryKey]
	if (!lua_istable(L, -1)) {
		lua_pop(L, 1);
		return false;
	}
	lua_pushinteger(L, subKey);
	lua_gettable(L, -2);		// push registry[registryKey][subKey]
	lua_remove(L, -2);			// remove registry table, leave just result
	return lua_istable(L, -1);	// or lua_isfunction(), if expecting a func
}
void LuaRegistrySetSubTableEntry(LuaState L, const char* registryKey, int subKey, int valueIndex) {
	lua_getfield(L, LUA_REGISTRYINDEX, registryKey);  // registry table on top or nil
	lua_pushinteger(L, subKey);						  // subkey to get from this
	lua_pushvalue(L, valueIndex);					  // copy value (from anywhere on stack)
	lua_settable(L, -3);							  // registry[subKey] = value
	lua_pop(L, 1);									  // pop registry table
}
// Functions
void LuaGetLuaFuncAtIndex(LuaState L, int index) {
	lua_rawgeti(L, -1, index);	// get function at index (1 = click, 2 = hover)
	if (!lua_isfunction(L, -1)) {
		sgLogWarn("No function was pushed, what the, pop everything");
		lua_pop(L, 3);
		return;
	}
}
void LuaGetLuaFunc(LuaState L, const char* field) {
	lua_getfield(L, -1, field);
	if (!lua_isfunction(L, -1)) {
		sgLogWarn("No function was pushed, what the, pop everything");
		lua_pop(L, 3);
		return;
	}
}
void RunLuaFunctionOnStack(LuaState L, int numArgs) {
	if (lua_pcall(L, numArgs, 0, 0) != LUA_OK) {
		const char* err = lua_tostring(L, -1);
		sgLogWarn("Button func error: %s\n", err);
		lua_pop(L, 1);	// pop func
	}
}

// int LuaCheckFunctionCallParamsAndTypes(LuaState L, int numArgsOnStack, ...) {
int LuaCheckFunctionCallParamsAndTypes(LuaState L, int numArgsOnStack, ...) {
	if (LuaGetStackSize(L) != numArgsOnStack) {
		sgLogWarn("Bad number of arguments passed to C function from lua");
		return false;
	}
	va_list argP;
	LuaFunctionParameterTypes param = LuaFunctionParameterTypePass;

	int luaPos = 0;
	va_start(argP, numArgsOnStack);
	for (size_t i = 0; i < numArgsOnStack; i++) {
		// param = va_arg(argP, LuaFunctionParameterTypes);
		param = (LuaFunctionParameterTypes)va_arg(argP, int);
		// param = *(LuaFunctionParameterTypes*)argP;
		++luaPos;
		if (!param) {
			sgLogWarn("Could not get arg from vaargs, returning false");
			return false;
		}
		switch (param) {
			case LuaFunctionParameterTypeInt:
				if (!LuaIsInt(L, luaPos)) {
					goto rfalse;
				}
				continue;
			case LuaFunctionParameterTypeNumber:
				if (!LuaIsFloat(L, luaPos)) {
					goto rfalse;
				}
				continue;
			case LuaFunctionParameterTypeString:
				if (!LuaIsString(L, luaPos)) {
					goto rfalse;
				}
				continue;
			case LuaFunctionParameterTypeFunction:
				if (!LuaIsLuaFunc(L, luaPos)) {
					goto rfalse;
				}
				continue;
			case LuaFUnctionParameterTypeUserdata:
				if (!lua_islightuserdata(L, luaPos)) {
					goto rfalse;
				}
				continue;
			case LuaFunctionParameterTypeTable:
				if (!LuaIsTable(L, luaPos)) {
					goto rfalse;
				}
				continue;
			case LuaFunctionParameterTypePass:
			default:
				continue;
		}
	}
	va_end(argP);
	return true;
rfalse:
	va_end(argP);
	sgLogWarn("Bad parameter passed into lua function, expecting %s at pos %d but got %s", LuaGetParamType(param), luaPos, LuaGetTypeStringi(L, luaPos));
	return false;
}
const char* LuaGetParamType(LuaFunctionParameterTypes paramType) {
	switch (paramType) {
		case LuaFunctionParameterTypeInt:
			return "int";
		case LuaFunctionParameterTypeNumber:
			return "number";
		case LuaFunctionParameterTypeString:
			return "string";
		case LuaFunctionParameterTypeFunction:
			return "function";
		case LuaFunctionParameterTypeTable:
			return "table";
		case LuaFUnctionParameterTypeUserdata:
			return "userdata";
		default:
			return "not implemented?";
	}
}

const char* LuaGetTypeStringi(LuaState L, int pos) {
	return luaL_typename(L, pos);
}
