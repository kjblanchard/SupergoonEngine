#include <Supergoon/filesystem.h>
#include <Supergoon/lua.h>
#include <assert.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <sgforge/directory.h>
#include <sgtools/log.h>
#include <sgtools/tools.h>
#include <stdio.h>
#include <string.h>

#include "sgforge/unpack.h"

static LuaState luaGlobalState = NULL;
static Directory* scriptDirectory = NULL;

static int buffer_searcher(lua_State* L) {
	if (!scriptDirectory) {
		lua_pushstring(L, "\n\tno script directory registered");
		return 1;
	}
	const char* modname = luaL_checkstring(L, 1);
	char path[256];
	snprintf(path, sizeof(path), "%s", modname);
	for (char* p = path; *p; p++) {
		if (*p == '.') *p = '/';
	}
	strncat(path, ".lua", sizeof(path) - strlen(path) - 1);
	char* buf;
	size_t sz;
	if (!GetDataFromDirectory(path, &buf, &sz, scriptDirectory)) {
		lua_pushfstring(L, "\n\tno buffer '%s'", path);
		return 1;
	}
	if (luaL_loadbuffer(L, buf, sz, path) != LUA_OK) {
		return luaL_error(L, "error loading module '%s':\n\t%s",
						  modname, lua_tostring(L, -1));
	}
	return 1;
}

static void setLuaPath(void) {
	int value = lua_getglobal(luaGlobalState, "package");
	if (value == LUA_TNIL)
		sgLogCritical("Could not get lua package, what the");
	lua_getfield(luaGlobalState, -1, "path");
	const char* basePath = GetBasePath();
	const char* nextPath =
		"assets/lua/?.lua;../Resources/assets/lua/?.lua;assets/scripts/?.lua;../"
		"Resources/assets/scripts/?.lua";
	const char* currentLuaPath =
		lua_tostring(luaGlobalState, -1);  // grab path string from top of stack

	// size_t full_str_len = strlen(currentLuaPath) + strlen(nextPath) +
	// strlen(basePath) + 2;
	char* full_str = NULL;
	asprintf(&full_str, "%s;%s%s", currentLuaPath, basePath, nextPath);
	lua_pop(luaGlobalState, 1);
	lua_pushstring(luaGlobalState, full_str);
	lua_setfield(luaGlobalState, -2, "path");
	lua_pop(luaGlobalState, 1);
	free(full_str);
}

void InitializeLuaSystem(void) {
	luaGlobalState = luaL_newstate();
	if (luaGlobalState == NULL) {
		sgLogCritical("Could not initialize Lua");
		return;
	}
	luaL_openlibs(luaGlobalState);
	setLuaPath();
}

void LuaSetScriptDirectory(Directory* d) {
	scriptDirectory = d;
	lua_getglobal(luaGlobalState, "package");
	lua_getfield(luaGlobalState, -1, "searchers");
	int len = (int)lua_rawlen(luaGlobalState, -1);
	lua_pushcfunction(luaGlobalState, buffer_searcher);
	lua_rawseti(luaGlobalState, -2, len + 1);
	lua_pop(luaGlobalState, 2);
}

void LuaRunFile(const char* path) {
	const char* basePath = GetBasePath();
	// size_t size = strlen(basePath) + strlen(path) + 1;
	char* fullPath = NULL;
	asprintf(&fullPath, "%s%s", basePath, path);
	if (luaL_dofile(luaGlobalState, fullPath) != 0) {
		const char* luaError = lua_tostring(luaGlobalState, -1);
		sgLogError("Lua error: %s", luaError);
	}
	free(fullPath);
}

void LuaRunFileFromBuffer(const char* p, Directory* d) {
	char* buf;
	size_t sz;
	GetDataFromDirectory(p, &buf, &sz, d);
	int result = luaL_loadbuffer(luaGlobalState, buf, sz, p);
	if (result != LUA_OK) {
		goto error;
	}
	result = lua_pcall(luaGlobalState, 0, LUA_MULTRET, 0);
	if (result == LUA_OK) {
		return;
	}
error: {
	const char* luaError = lua_tostring(luaGlobalState, -1);
	sgLogError("Lua error: %s", luaError);
	lua_pop(luaGlobalState, 1);
}
}

int LuaGetStackSize(LuaState L) { return lua_gettop(L); }

void LuaPopStack(LuaState L, int num) { lua_pop(L, num); }

void LuaClearStack(LuaState L) { lua_settop(L, 0); }
// Gets the number of items on the stack, remember that the index starts at 1.
int LuaGetStack(LuaState L) { return lua_gettop(L); }
void LuaRemoveIndex(LuaState L, int index) { lua_remove(L, index); }
// Moves the tip to the index passed in, pushing everything else up.  Useful for
// if you pass func args first.
void LuaMoveStackTipToIndex(LuaState L, int index) {
	lua_insert(L, index);  // Rearrage the stack so that the function is before the
						   // actual arguments.
}
void ShutdownLuaSystem(void) { lua_close(luaGlobalState); }

// Tables
void LuaPushTableFromFile(LuaState L, const char* path) {
	LuaRunFile(path);
	if (!lua_istable(L, -1)) {
		printf("map.lua did not return a table!\n");
		lua_pop(L, 1);
	}
}

void LuaPushTableFromStackTip(LuaState L, const char* path) {
	lua_getfield(L, -1, path);
	if (!lua_istable(L, -1)) {
		printf("field did not get a table!\n");
		lua_pop(L, 1);
	}
}
int LuaGetIntFromTablei(LuaState L, int i) {
	lua_rawgeti(L, -1, i + 1);
	int value = lua_tointeger(luaGlobalState, -1);
	lua_pop(L, 1);
	return value;
}

const char* LuaGetStringFromTablei(LuaState L, int i) {
	lua_rawgeti(L, -1, i + 1);
	const char* value = lua_tostring(luaGlobalState, -1);
	if (value == NULL) {
		sgLogDebug("nil value in string, returning empty ");
		value = "";
	}
	lua_pop(L, 1);
	return value;
}

float LuaGetFloatFromTablei(LuaState L, int i) {
	lua_rawgeti(L, -1, i + 1);
	float value = lua_tonumber(luaGlobalState, -1);
	lua_pop(L, 1);
	return value;
}

void LuaPushNewTableToStack(LuaState L) { lua_newtable(L); }

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
	// // since we duplicated it with pushvalue, we should actually pop the table
	// value too. lua_pop(L, 1);

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
	lua_rawseti(L, -2,
				len + 1);  // t[len+1] = value
						   // since we duplicated it with pushvalue, we should
						   // actually pop the table value too. lua_pop(L, 1);
}

void LuaPushTableFromRegistryByName(LuaState L, const char* tableName) {
	lua_getfield(L, LUA_REGISTRYINDEX,
				 tableName);  // pushes the table onto the stack
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
int LuaGetTableLength(LuaState L) { return lua_rawlen(L, -1); }
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
void LuaPushTableFromGlobal(LuaState L, const char* global) {
	lua_getglobal(L, global);
}
void LuaGetTable(LuaState L, const char* tableFieldName) {
	lua_getfield(L, -1, tableFieldName);
	if (!lua_istable(L, -1)) {
		sgLogWarn("Table pushed is not a real lua table, what did you do");
	}
}
void LuaSetGlobal(LuaState L, const char* global) {
	lua_setglobal(luaGlobalState, global);
}

void LuaUnsetGlobal(LuaState L, const char* global) {
	lua_pushnil(L);
	lua_setglobal(luaGlobalState, global);
}
int LuaGetTablei(LuaState L, int i) {
	if (lua_istable(L, i)) {
		return lua_gettable(L, 0);
	} else {
		return 0;
	}
}
// Starts the table iteration for lua tables that are key/value pairs, returns
// if there is any more values left This pushes two values to the table for
// iteration, The key is -2, value is -1.  Key is used for iteration, so don't
// pop it off, use stopiteration when done, only pop value
void LuaStartTableKeyValueIteration(LuaState L) {
	lua_pushnil(L);
	lua_pushnil(L);
}
// Goes to the next item in the table, returns if there is any more values left
// You must pop the value off before next iteration.
int LuaNextTableKeyValueIterate(LuaState L) { return lua_next(L, -2); }
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
int LuaGetIntFromStack(LuaState L) { return lua_tointeger(L, -1); }
int LuaGetIntFromStacki(LuaState L, int i) { return lua_tointeger(L, i); }
// Bools
int LuaGetBool(LuaState L, const char* field) {
	lua_getfield(L, -1, field);
	int fieldInt = lua_toboolean(L, -1);
	lua_pop(L, 1);
	return fieldInt;
}
int LuaGetBooli(LuaState L, int i) { return lua_toboolean(L, i); }
// Floats
float LuaGetFloat(LuaState L, const char* field) {
	lua_getfield(L, -1, field);
	float fieldFloat = lua_tonumber(L, -1);
	lua_pop(L, 1);
	return fieldFloat;
}
float LuaGetFloati(LuaState L, int i) { return lua_tonumber(L, i); }
float LuaGetFloatFromStack(LuaState L) { return lua_tonumber(L, -1); }
float LuaGetFloatFromTableStackiKey(LuaState L, int i, const char* key) {
	lua_getfield(L, i, key);
	float fieldFloat = lua_tonumber(L, -1);
	lua_pop(L, 1);
	return fieldFloat;
}
// Strings
// i is stack location of table, const char* is key.. so not directly on stack
// -1.
void LuaCopyString(LuaState L, const char* name, char* location, int strlen) {
	lua_getfield(L, -1, name);
	strncpy(location, lua_tostring(L, -1), strlen);
	lua_pop(L, 1);
}
// Does not pop off, please do the needful
void LuaCopyStringStack(LuaState L, int stackLocation, char* location,
						int strlen) {
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
const char* LuaGetStringi(LuaState L, int i) { return lua_tostring(L, i); }
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
void LuaPushBool(LuaState L, int boolean) { lua_pushboolean(L, boolean); }
void LuaPushNil(LuaState L) { lua_pushnil(L); }
void LuaPushLightUserdata(LuaState L, void* data) {
	lua_pushlightuserdata(L, data);
}
void LuaPushFloat(LuaState L, float data) { lua_pushnumber(L, data); }

void LuaPushInt(LuaState L, int data) { lua_pushinteger(L, data); }
void LuaPushString(LuaState L, const char* data) { lua_pushstring(L, data); }
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
int LuaRegistryGetSubTableEntry(LuaState L, const char* registryKey,
								int subKey) {
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
void LuaRegistrySetSubTableEntry(LuaState L, const char* registryKey,
								 int subKey, int valueIndex) {
	lua_getfield(L, LUA_REGISTRYINDEX,
				 registryKey);	   // registry table on top or nil
	lua_pushinteger(L, subKey);	   // subkey to get from this
	lua_pushvalue(L, valueIndex);  // copy value (from anywhere on stack)
	lua_settable(L, -3);		   // registry[subKey] = value
	lua_pop(L, 1);				   // pop registry table
}

int LuaCreateRefInLuaRegistry(LuaState L, int i) {
	lua_pushvalue(L, i);
	int ref = luaL_ref(L, LUA_REGISTRYINDEX);
	return ref;
}

void LuaPushRefValueInLuaRegistry(LuaState L, int refInt) {
	lua_rawgeti(L, LUA_REGISTRYINDEX, refInt);
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
		sgLogWarn("Failed to run Lua function: %s\n", err);
		lua_pop(L, 1);	// pop func
	}
}

int LuaCheckFunctionCallParamsAndTypes(lua_State* L, int numArgsOnStack, ...) {
	if (lua_gettop(L) != numArgsOnStack) {	// Lua stack is 1-based
		sgLogWarn(
			"Bad number of arguments passed to C function from Lua: expected "
			"%d, got %d",
			numArgsOnStack, lua_gettop(L));
		return 0;
	}

	va_list argP;
	va_start(argP, numArgsOnStack);
	LuaFunctionParameterTypes param = LuaFunctionParameterTypeMax;
	int i = 0;

	for (i = 1; i <= numArgsOnStack; i++) {	 // Lua stack indices are 1-based
		param = (LuaFunctionParameterTypes)va_arg(argP, int);

		if (param < 0 || param >= LuaFunctionParameterTypeMax) {
			sgLogWarn("Invalid LuaFunctionParameterType enum value: %d", param);
			va_end(argP);
			return 0;
		}

		switch (param) {
			case LuaFunctionParameterTypeInt:
				if (!LuaIsInt(L, i))
					goto rfalse;
				break;
			case LuaFunctionParameterTypeNumber:
				if (!LuaIsFloat(L, i))
					goto rfalse;
				break;
			case LuaFunctionParameterTypeString:
				if (!LuaIsString(L, i))
					goto rfalse;
				break;
			case LuaFunctionParameterTypeFunction:
				if (!LuaIsLuaFunc(L, i))
					goto rfalse;
				break;
			case LuaFunctionParameterTypeUserdata:
				if (!lua_islightuserdata(L, i))
					goto rfalse;
				break;
			case LuaFunctionParameterTypeTable:
				if (!LuaIsTable(L, i))
					goto rfalse;
				break;
			case LuaFunctionParameterTypeBoolean:
				if (!LuaIsBool(L, i))
					goto rfalse;
				break;
			case LuaFunctionParameterTypePass:
			default:
				break;
		}
	}

	va_end(argP);
	return 1;

rfalse:
	sgLogWarn(
		"Bad parameter passed into Lua function at stack position %d, "
		"expecting %s but got %s",
		i, LuaGetParamType(param), LuaGetTypeStringi(L, i));
	va_end(argP);
	return 0;
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
		case LuaFunctionParameterTypeUserdata:
			return "userdata";
		case LuaFunctionParameterTypeBoolean:
			return "boolean";
		default:
			return "type not impelemented?  please fix";
	}
}

const char* LuaGetTypeStringi(LuaState L, int pos) {
	return luaL_typename(L, pos);
}

#define LUA_REGISTER_MAX 12
void LuaRegisterFunctionsToLuaLibraryInternal(const LuaCFuncRegister* f, size_t sz, const char* n) {
	assert(sz <= LUA_REGISTER_MAX);
	luaL_Reg funcs[LUA_REGISTER_MAX] = {0};
	for (size_t i = 0; i < sz; i++) {
		funcs[i].name = f[i].Name;
		funcs[i].func = f[i].Func;
	}
	funcs[sz] = (luaL_Reg){NULL, NULL};
	luaL_newlib(luaGlobalState, funcs);
	lua_setglobal(luaGlobalState, n);
}
