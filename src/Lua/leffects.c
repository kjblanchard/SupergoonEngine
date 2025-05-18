#include <Supergoon/effects.h>
#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <lauxlib.h>
#include <lua.h>
static int fadeoutScreen(lua_State* L) {
	if (LuaGetStackSize() != 1 || !LuaGetFloati(1)) {
		sgLogWarn("Tried to fadeout with improper values");
		return 0;
	}
	FadeoutScreen(LuaGetFloati(1));
	return 0;
}

static int fadeinScreen(lua_State* L) {
	if (LuaGetStackSize() != 1 || !LuaGetFloati(1)) {
		sgLogWarn("Tried to fadeout with improper values");
		return 0;
	}
	FadeinScreen(LuaGetFloati(1));
	return 0;
}

static int isScreenFading(lua_State* L) {
	LuaPushBool(IsScreenFading());
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
