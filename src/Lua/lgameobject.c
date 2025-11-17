#include <Supergoon/Lua/object.h>
#include <Supergoon/gameobject.h>
#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <Supergoon/map.h>
#include <lauxlib.h>
#include <lua.h>
#include <stdbool.h>

static int getGameobjectPosition(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 1, LuaFunctionParameterTypeUserdata)) {
		sgLogWarn("Bad args trying to set go position from lua");
		return 0;
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
	if (!LuaCheckFunctionCallParamsAndTypes(L, 3, LuaFunctionParameterTypeUserdata, LuaFunctionParameterTypeNumber, LuaFunctionParameterTypeNumber)) {
		sgLogWarn("Bad args trying to set go position from lua");
		return 0;
	}
	GameObject* go = (GameObject*)LuaGetLightUserdatai(L, 1);
	if (!go) {
		sgLogWarn("bad thing");
		return 0;
	}
	go->X = LuaGetFloati(L, 2);
	go->Y = LuaGetFloati(L, 3);
	return 0;
}

static int getGameobjectId(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 1, LuaFunctionParameterTypeUserdata)) {
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

static int createGameObject(lua_State* L) {
	GameObject* go = GameObjectCreate();
	LuaPushLightUserdata(L, go);
	return 1;
}

static int deleteGameObject(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 1, LuaFunctionParameterTypeUserdata)) {
		sgLogWarn("bad gameobject passed to delete gameobject");
		return 0;
	}
	GameObject* go = (GameObject*)LuaGetLightUserdatai(L, 1);
	if (go) GameObjectDestroy(go);
	return 0;
}

static const luaL_Reg objectLib[] = {
	{"CreateGameObject", createGameObject},
	{"DestroyGameObject", deleteGameObject},
	{"Position", getGameobjectPosition},
	{"SetPosition", setGameobjectPosition},
	{"Id", getGameobjectId},
	{NULL, NULL}};

void RegisterLuaObjectFunctions(void) {
	luaL_newlib(_luaState, objectLib);
	lua_setglobal(_luaState, "cGameObject");
}
