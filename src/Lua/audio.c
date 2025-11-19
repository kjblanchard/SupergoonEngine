#include <Supergoon/Audio/Audio.h>
#include <Supergoon/lua.h>
#include <lauxlib.h>
#include <lua.h>
#include <stdio.h>

static int PlaySfx(lua_State* L) {
	if (LuaGetStackSize(L) != 2 || !LuaIsString(L, 1) || !LuaIsFloat(L, 2)) {
		return 0;
	}
	PlaySfxOneShot(LuaGetStringi(L, 1), LuaGetFloati(L, 2));
	return 0;
}

static int playBgm(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 3, LuaFunctionParameterTypeString, LuaFunctionParameterTypeNumber, LuaFunctionParameterTypeInt)) {
		return 0;
	}
	SetBgmTrack(LuaGetIntFromStacki(L, 3));
	LoadBgm(LuaGetStringi(L, 1), LuaGetFloati(L, 2), -1);
	PlayBgm();
	return 0;
}

static int restartBgm(lua_State* L) {
	/* if (!LuaCheckFunctionCallParamsAndTypes(L, 3, LuaFunctionParameterTypeString, LuaFunctionParameterTypeNumber, LuaFunctionParameterTypeInt)) { */
	/* 	return 0; */
	/* } */
	/* SetBgmTrack(LuaGetIntFromStacki(L, 3)); */
	/* RestartStream(); */
	/* LoadBgm(LuaGetStringi(L, 1), LuaGetFloati(L, 2), -1); */
	/* PlayBgm(); */
	return 0;
}

static int unpauseBgm(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 1, LuaFunctionParameterTypeInt)) {
		return 0;
	}
	SetBgmTrack(LuaGetIntFromStacki(L, 1));
	PlayBgm();
	return 0;
}

static int stopBGM(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 1, LuaFunctionParameterTypeInt)) {
		return 0;
	}
	SetBgmTrack(LuaGetIntFromStacki(L, 1));
	StopBgm();
	return 0;
}

static int setBgmGlobalAudio(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 1, LuaFunctionParameterTypeNumber)) {
		return 0;
	}
	SetGlobalBgmVolume(LuaGetFloati(L, 1));
	return 0;
}

static int setSfxGlobalAudio(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 1, LuaFunctionParameterTypeNumber)) {
		return 0;
	}
	SetGlobalSfxVolume(LuaGetFloati(L, 1));
	return 0;
}

static const luaL_Reg audioLib[] = {
	{"PlaySfx", PlaySfx},
	{"PlayBgm", playBgm},
	{"UnPauseBgm", unpauseBgm},
	{"StopBgm", stopBGM},
	{"SetGlobalBgmVolume", setBgmGlobalAudio},
	{"SetGlobalSfxVolume", setSfxGlobalAudio},
	{NULL, NULL}};

void RegisterLuaAudioFunctions(void) {
	luaL_newlib(_luaState, audioLib);
	lua_setglobal(_luaState, "cAudio");
}
