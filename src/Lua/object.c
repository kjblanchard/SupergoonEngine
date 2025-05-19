#include <Supergoon/gameobject.h>
#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <SupergoonEngine/Lua/object.h>
#include <SupergoonEngine/gameobject.h>
#include <lauxlib.h>
#include <lua.h>
#include <stdbool.h>

static int LuaCreateObjectFuncs[MAX_GAMEOBJECT_TYPES] = {0};
static int LuaStartObjectFuncs[MAX_GAMEOBJECT_TYPES] = {0};
static int LuaUpdateObjectFuncs[MAX_GAMEOBJECT_TYPES] = {0};

static void createObject(void* userdata, GameObject* go) {
	if (LuaCreateObjectFuncs[go->Type] != LUA_REFNIL) {
		lua_rawgeti(_luaState, LUA_REGISTRYINDEX, LuaCreateObjectFuncs[go->Type]);
		lua_pushlightuserdata(_luaState, userdata);
		lua_pushlightuserdata(_luaState, go);
		if (lua_pcall(_luaState, 2, 0, 0) != LUA_OK) {
			const char* err = lua_tostring(_luaState, -1);
			fprintf(stderr, "Error in CreateObject: %s\n", err);
			lua_pop(_luaState, 1);
		}
	}
}
static void startObject(GameObject* go) {
	if (LuaCreateObjectFuncs[go->Type] != LUA_REFNIL) {
		lua_rawgeti(_luaState, LUA_REGISTRYINDEX, LuaStartObjectFuncs[go->Type]);
		lua_pushlightuserdata(_luaState, go);
		if (lua_pcall(_luaState, 1, 0, 0) != LUA_OK) {
			const char* err = lua_tostring(_luaState, -1);
			fprintf(stderr, "Error in CreateObject: %s\n", err);
			lua_pop(_luaState, 1);
		}
	}
}
static void updateObject(GameObject* go) {
	if (LuaCreateObjectFuncs[go->Type] != LUA_REFNIL) {
		lua_rawgeti(_luaState, LUA_REGISTRYINDEX, LuaUpdateObjectFuncs[go->Type]);
		lua_pushlightuserdata(_luaState, go);
		if (lua_pcall(_luaState, 1, 0, 0) != LUA_OK) {
			const char* err = lua_tostring(_luaState, -1);
			fprintf(stderr, "Error in CreateObject: %s\n", err);
			lua_pop(_luaState, 1);
		}
	}
}

static int l_register_object_functions(lua_State* L) {
	int type = luaL_checkinteger(L, 1);
	luaL_checktype(L, 2, LUA_TTABLE);  // Expects an array table
	if (type < 0 || type >= MAX_GAMEOBJECT_TYPES)
		return luaL_error(L, "Invalid object type");
	lua_rawgeti(L, 2, 1);
	if (!lua_isfunction(L, -1)) return luaL_error(L, "Expected function at index 1 (create)");
	LuaCreateObjectFuncs[type] = luaL_ref(L, LUA_REGISTRYINDEX);
	lua_rawgeti(L, 2, 2);
	if (!lua_isfunction(L, -1)) return luaL_error(L, "Expected function at index 2 (start)");
	LuaStartObjectFuncs[type] = luaL_ref(L, LUA_REGISTRYINDEX);
	lua_rawgeti(L, 2, 3);
	if (!lua_isfunction(L, -1)) return luaL_error(L, "Expected function at index 3 (update)");
	LuaUpdateObjectFuncs[type] = luaL_ref(L, LUA_REGISTRYINDEX);
	ObjectSetCreateFunction(type, createObject);
	ObjectSetStartFunction(type, startObject);
	ObjectSetUpdateFunction(type, updateObject);
	return 0;
}

static int setAllGameobjectsToBeDestroyed(lua_State* L) {
	bool force = false;
	if (!LuaIsBool(L, 1)) {
		sgLogWarn("No bool sent it for destroying all gameobjects, setting to false");
	}
	force = LuaGetBooli(L, 1);

	SetGameobjectsToBeDeleted(force);
	return 0;
}

static int destroyObjects(lua_State* L) {
	DestroyGameObjects();
	return 0;
}

static const luaL_Reg objectLib[] = {
	{"NewGameObjectType", l_register_object_functions},
	{"SetDestroyGameObjects", setAllGameobjectsToBeDestroyed},
	{"DestroyGameObjects", destroyObjects},
	{NULL, NULL}};

void RegisterLuaObjectFunctions(void) {
	luaL_newlib(_luaState, objectLib);
	lua_setglobal(_luaState, "cGameObject");
}
