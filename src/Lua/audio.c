#include <Supergoon/Audio/Audio.h>
#include <Supergoon/lua.h>
#include <lauxlib.h>
#include <lua.h>

static int PlaySfx(lua_State* L) {
	if (LuaGetStackSize(L) != 2 || !LuaIsString(L, 1) || !LuaIsFloat(L, 2)) {
		return 0;
	}
	const char* name = LuaGetStringi(L, 1);
	float volume = LuaGetFloati(L, 2);
	PlaySfxOneShot(LuaGetStringi(L, 1), LuaGetFloati(L, 2));
	return 0;
}

static int playBgm(lua_State* L) {
	if (LuaGetStackSize(L) != 2 || !LuaIsString(L, 1) || !LuaIsFloat(L, 2)) {
		return 0;
	}
	SetBgmTrack(0);
	LoadBgm(LuaGetStringi(L, 1), LuaGetFloati(L, 2), -1);
	PlayBgm();
	return 0;
}

static const luaL_Reg audioLib[] = {
	{"PlaySfx", PlaySfx},
	{"PlayBgm", playBgm},
	{NULL, NULL}};

void RegisterLuaAudioFunctions(void) {
	luaL_newlib(_luaState, audioLib);
	lua_setglobal(_luaState, "cAudio");
}
