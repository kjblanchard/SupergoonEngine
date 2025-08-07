#include <Supergoon/gameobject.h>
#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <Supergoon/map.h>
#include <SupergoonEngine/Lua/object.h>
#include <SupergoonEngine/gameobject.h>
#include <lauxlib.h>
#include <lua.h>
#include <stdbool.h>

static int LuaCreateObjectFuncs[MAX_GAMEOBJECT_TYPES] = {LUA_REFNIL};
static int LuaStartObjectFuncs[MAX_GAMEOBJECT_TYPES] = {LUA_REFNIL};
static int LuaUpdateObjectFuncs[MAX_GAMEOBJECT_TYPES] = {LUA_REFNIL};
static int LuaDestroyObjectFuncs[MAX_GAMEOBJECT_TYPES] = {LUA_REFNIL};

static void createObject(void* userdata, GameObject* go) {
	if (LuaCreateObjectFuncs[go->Type] != LUA_REFNIL) {
		TiledObject* objectData = (TiledObject*)userdata;
		if (!objectData) {
			sgLogError("Could not create object properly, bad userdata");
		}

		lua_rawgeti(_luaState, LUA_REGISTRYINDEX, LuaCreateObjectFuncs[go->Type]);
		// lua_pushlightuserdata(_luaState, userdata);
		// We should push this as a table
		LuaPushNewTableToStack(_luaState);
		LuaPushFloatToTable(_luaState, "x", objectData->X);
		LuaPushFloatToTable(_luaState, "y", objectData->Y);
		LuaPushFloatToTable(_luaState, "w", objectData->Width);
		LuaPushFloatToTable(_luaState, "h", objectData->Height);
		// Create properties table
		LuaPushNewTableToStack(_luaState);
		for (size_t i = 0; i < objectData->NumProperties; i++) {
			TiledProperty* prop = &objectData->Properties[i];
			switch (prop->PropertyType) {
				case TiledPropertyTypeFloat:
					LuaPushFloatToTable(_luaState, prop->Name, prop->Data.FloatData);
					break;
				case TiledPropertyTypeInt:
					LuaPushIntToTable(_luaState, prop->Name, prop->Data.IntData);
					break;
				case TiledPropertyTypeString:
					LuaPushStringToTable(_luaState, prop->Name, prop->Data.StringData);
					break;
				default:
					continue;
			}
		}
		LuaPushTableToTable(_luaState, "properties");
		lua_pushlightuserdata(_luaState, go);
		if (lua_pcall(_luaState, 2, 0, 0) != LUA_OK) {
			const char* err = lua_tostring(_luaState, -1);
			sgLogError("Error in create gameobject %s", err);
			lua_pop(_luaState, 1);
		}
	}
}
static void startObject(GameObject* go) {
	if (LuaStartObjectFuncs[go->Type] != LUA_REFNIL) {
		lua_rawgeti(_luaState, LUA_REGISTRYINDEX, LuaStartObjectFuncs[go->Type]);
		lua_pushlightuserdata(_luaState, go);
		if (lua_pcall(_luaState, 1, 0, 0) != LUA_OK) {
			const char* err = lua_tostring(_luaState, -1);
			sgLogError("Error in start gameobject %s", err);
			lua_pop(_luaState, 1);
		}
	}
}
static void updateObject(GameObject* go) {
	if (LuaUpdateObjectFuncs[go->Type] != LUA_REFNIL) {
		lua_rawgeti(_luaState, LUA_REGISTRYINDEX, LuaUpdateObjectFuncs[go->Type]);
		lua_pushlightuserdata(_luaState, go);
		if (lua_pcall(_luaState, 1, 0, 0) != LUA_OK) {
			const char* err = lua_tostring(_luaState, -1);
			sgLogError("Error in update gameobject %s", err);
			lua_pop(_luaState, 1);
		}
	}
}

static void destroyObject(GameObject* go) {
	if (LuaDestroyObjectFuncs[go->Type] != LUA_REFNIL) {
		lua_rawgeti(_luaState, LUA_REGISTRYINDEX, LuaDestroyObjectFuncs[go->Type]);
		lua_pushlightuserdata(_luaState, go);
		if (lua_pcall(_luaState, 1, 0, 0) != LUA_OK) {
			const char* err = lua_tostring(_luaState, -1);
			sgLogError("Error in update gameobject %s", err);
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
	// if (!lua_isfunction(L, -1)) return luaL_error(L, "Expected function at index 1 (create)");
	if (LuaIsLuaFunc(L, -1)) {
		LuaCreateObjectFuncs[type] = luaL_ref(L, LUA_REGISTRYINDEX);
	} else {
		LuaCreateObjectFuncs[type] = LUA_REFNIL;
	}

	lua_rawgeti(L, 2, 2);
	// if (!lua_isfunction(L, -1)) return luaL_error(L, "Expected function at index 2 (start)");
	if (LuaIsLuaFunc(L, -1)) {
		LuaStartObjectFuncs[type] = luaL_ref(L, LUA_REGISTRYINDEX);
	} else {
		LuaStartObjectFuncs[type] = LUA_REFNIL;
	}
	lua_rawgeti(L, 2, 3);
	if (LuaIsLuaFunc(L, -1)) {
		LuaUpdateObjectFuncs[type] = luaL_ref(L, LUA_REGISTRYINDEX);
	} else {
		LuaUpdateObjectFuncs[type] = LUA_REFNIL;
	}
	// if (!lua_isfunction(L, -1)) return luaL_error(L, "Expected function at index 3 (update)");
	lua_rawgeti(L, 2, 4);
	if (LuaIsLuaFunc(L, -1)) {
		LuaDestroyObjectFuncs[type] = luaL_ref(L, LUA_REGISTRYINDEX);
	} else {
		LuaDestroyObjectFuncs[type] = LUA_REFNIL;
	}
	// if (!lua_isfunction(L, -1)) return luaL_error(L, "Expected function at index 4 (update)");
	ObjectSetCreateFunction(type, createObject);
	ObjectSetStartFunction(type, startObject);
	ObjectSetUpdateFunction(type, updateObject);
	ObjectSetDestroyFunction(type, destroyObject);
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

static int getGameobjectPosition(lua_State* L) {
	if (LuaGetStackSize(L) != 1) {
		sgLogWarn("Bad args trying to get go position from lua");
		LuaPushNil(L);
		return 1;
	}
	GameObject* go = (GameObject*)LuaGetLightUserdatai(L, 1);
	if (!go) {
		sgLogWarn("Bad cast from go Lua");
		LuaPushNil(L);
		return 1;
	}
	LuaPushFloat(L, go->X);
	LuaPushFloat(L, go->Y);
	return 2;
}

static int setGameobjectPosition(lua_State* L) {
	if (LuaGetStackSize(L) != 3 || !LuaIsFloat(L, 2) || !LuaIsFloat(L, 3)) {
		sgLogWarn("Bad args trying to set go position from lua");
		return 0;
	}
	GameObject* go = (GameObject*)LuaGetLightUserdatai(L, 1);
	if (!go) {
		sgLogWarn("Bad cast from go Lua");
		return 0;
	}
	go->X = LuaGetFloati(L, 2);
	go->Y = LuaGetFloati(L, 3);
	return 0;
}

static int setGameobjectSize(lua_State* L) {
	if (LuaGetStackSize(L) != 3 || !LuaIsFloat(L, 2) || !LuaIsFloat(L, 3)) {
		sgLogWarn("Bad args trying to set go position from lua");
		return 0;
	}
	GameObject* go = (GameObject*)LuaGetLightUserdatai(L, 1);
	if (!go) {
		sgLogWarn("Bad cast from go Lua");
		return 0;
	}
	go->W = LuaGetFloati(L, 2);
	go->H = LuaGetFloati(L, 3);
	return 0;
}

static int getGameobjectSize(lua_State* L) {
	if (LuaGetStackSize(L) != 1) {
		sgLogWarn("Bad args trying to get go position from lua");
		LuaPushNil(L);
		return 1;
	}
	GameObject* go = (GameObject*)LuaGetLightUserdatai(L, 1);
	if (!go) {
		sgLogWarn("Bad cast from go Lua");
		LuaPushNil(L);
		return 1;
	}
	LuaPushFloat(L, go->W);
	LuaPushFloat(L, go->H);
	return 2;
}

static int getGameobjectId(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 1, LuaFUnctionParameterTypeUserdata)) {
		sgLogWarn("Bad args trying to get go position from lua");
		LuaPushNil(L);
		return 1;
	}
	GameObject* go = (GameObject*)LuaGetLightUserdatai(L, 1);
	if (!go) {
		sgLogWarn("Bad cast from go Lua");
		LuaPushNil(L);
		return 1;
	}
	LuaPushInt(L, go->Id);
	return 1;
}

static int destroyObjects(lua_State* L) {
	DestroyGameObjects();
	return 0;
}

static int handleGameobjectSolids(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 1, LuaFUnctionParameterTypeUserdata)) {
		LuaPushNil(L);
		return 1;
	}
	GameObject* go = (GameObject*)LuaGetLightUserdatai(L, 1);
	if (!go) {
		sgLogWarn("Bad cast from go Lua");
		LuaPushNil(L);
		return 1;
	}
	CheckGameobjectForCollisionWithSolids(go);
	return 0;
}

static int checkSolidsRect(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 1, LuaFunctionParameterTypeTable)) {
		LuaPushNil(L);
		LuaPushNil(L);
		return 2;
	}
	RectangleF rect = {
		LuaGetFloatFromTableStackiKey(L, 1, "x"),
		LuaGetFloatFromTableStackiKey(L, 1, "y"),
		LuaGetFloatFromTableStackiKey(L, 1, "w"),
		LuaGetFloatFromTableStackiKey(L, 1, "h")};
	CheckRectForCollisionWithSolids(&rect);
	LuaPushNewTableToStack(L);
	LuaPushFloatToTable(L, "x", rect.x);
	LuaPushFloatToTable(L, "y", rect.y);
	LuaPushFloatToTable(L, "w", rect.w);
	LuaPushFloatToTable(L, "h", rect.h);
	return 1;
}

static int createGameObject(lua_State* L) {
	GameObject* obj = AddGameObject();
	LuaPushLightUserdata(L, obj);
	return 1;
}

static int setType(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 2, LuaFUnctionParameterTypeUserdata, LuaFunctionParameterTypeInt)) {
		sgLogWarn("Bad args trying to set go type from lua");
		LuaPushNil(L);
		return 1;
	}
	GameObject* go = (GameObject*)LuaGetLightUserdatai(L, 1);
	if (!go) {
		sgLogWarn("Bad cast from go Lua");
		LuaPushNil(L);
		return 1;
	}
	int type = LuaGetIntFromStacki(L, 2);
	go->Type = type;
	return 0;
}

static const luaL_Reg objectLib[] = {
	{"NewGameObjectType", l_register_object_functions},
	{"SetDestroyGameObjects", setAllGameobjectsToBeDestroyed},
	{"Position", getGameobjectPosition},
	{"SetPosition", setGameobjectPosition},
	{"SetSize", setGameobjectSize},
	{"Size", getGameobjectSize},
	{"Id", getGameobjectId},
	{"SetType", setType},
	{"DestroyGameObjects", destroyObjects},
	{"CheckSolids", handleGameobjectSolids},
	{"CheckSolidsRect", checkSolidsRect},
	{"CreateGameObject", createGameObject},

	{NULL, NULL}};

void RegisterLuaObjectFunctions(void) {
	luaL_newlib(_luaState, objectLib);
	lua_setglobal(_luaState, "cGameObject");
}
