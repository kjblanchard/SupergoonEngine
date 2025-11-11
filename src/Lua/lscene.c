#include <Supergoon/Lua/object.h>
#include <Supergoon/Lua/scene.h>
#include <Supergoon/Primitives/rectangle.h>
#include <Supergoon/gameobject.h>
#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <Supergoon/map.h>
#include <lauxlib.h>
#include <lua.h>

static int loadMap(lua_State *L) {
	// TODO since using coroutine, we have to use L.. noooooo
	lua_State *previous = _luaState;
	_luaState = L;
	if (LuaGetStackSize(L) != 1 || !LuaIsString(L, 1)) {
		sgLogWarn("Bad parameters passed into load map from lua");
		return 0;
	}
	LoadMap(LuaGetStringi(L, 1));
	_luaState = previous;
	return 0;
}
static int checkRectAgainstSceneSolids(lua_State *L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 1, LuaFunctionParameterTypeTable)) {
		sgLogWarn("Bad params passed to check scene rect");
		return 0;
	}
	RectangleF rect = {
		LuaGetFloatFromTableStackiKey(L, 1, "x"),
		LuaGetFloatFromTableStackiKey(L, 1, "y"),
		LuaGetFloatFromTableStackiKey(L, 1, "w"),
		LuaGetFloatFromTableStackiKey(L, 1, "h"),
	};
	CheckRectForCollisionWithSolids(&rect);
	LuaPushFloat(L, rect.x);
	LuaPushFloat(L, rect.y);
	return 2;
}

static const luaL_Reg sceneLib[] = {{"LoadMap", loadMap},
									{"ResolveCollisionWithSolids", checkRectAgainstSceneSolids},
									{NULL, NULL}};

void RegisterLuaSceneFuncs(void) {
	luaL_newlib(_luaState, sceneLib);
	lua_setglobal(_luaState, "cScene");
}
