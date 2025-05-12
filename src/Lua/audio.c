#include <Supergoon/Audio/Audio.h>
#include <Supergoon/lua.h>
#include <lauxlib.h>
#include <lua.h>

static int PlaySfx(lua_State* L) {
	if (LuaGetStackSize() != 2 || !LuaIsString(1) || !LuaIsFloat(2)) {
		return 0;
	}
	PlaySfxOneShot(LuaGetStringi(1), LuaGetFloati(2));
	return 0;
}

static const luaL_Reg audioLib[] = {
	{"PlaySfx", PlaySfx},
	{NULL, NULL}};

void RegisterLuaAudioFunctions(void) {
	luaL_newlib(_luaState, audioLib);
	lua_setglobal(_luaState, "cAudio");
}
