#include <Supergoon/effects.h>
#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <lauxlib.h>
#include <lua.h>
static int fadeoutScreen(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 1, LuaFunctionParameterTypeNumber)) {
		sgLogWarn("Tried to fadeout with improper values");
		return 0;
	}
	return 0;
}

static int fadeinScreen(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 1, LuaFunctionParameterTypeNumber)) {
		sgLogWarn("Tried to fadeout with improper values");
		return 0;
	}
	return 0;
}

static int isScreenFading(lua_State* L) {
	return 1;
}

static const luaL_Reg effectsLib[] = {
	{"Fadeout", fadeoutScreen},
	{"Fadein", fadeinScreen},
	{"IsScreenFading", isScreenFading},
	{NULL, NULL}};

void RegisterLuaEffectsFunctions(void) {
	luaL_newlib(_luaState, effectsLib);
	lua_setglobal(_luaState, "cEffects");
}
