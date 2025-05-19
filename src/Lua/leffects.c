#include <Supergoon/effects.h>
#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <lauxlib.h>
#include <lua.h>
static int fadeoutScreen(lua_State* L) {
	if (LuaGetStackSize(L) != 1 || !LuaGetFloati(L, 1)) {
		sgLogWarn("Tried to fadeout with improper values");
		return 0;
	}
	FadeoutScreen(LuaGetFloati(L, 1));
	return 0;
}

static int fadeinScreen(lua_State* L) {
	if (LuaGetStackSize(L) != 1 || !LuaGetFloati(L, 1)) {
		sgLogWarn("Tried to fadeout with improper values");
		return 0;
	}
	FadeinScreen(LuaGetFloati(L, 1));
	return 0;
}

static int isScreenFading(lua_State* L) {
	LuaPushBool(L, IsScreenFading());
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
