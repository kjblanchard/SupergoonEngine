#include <Supergoon/Input/keyboard.h>
#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <SupergoonEngine/Lua/input.h>
#include <lauxlib.h>
#include <lua.h>
#include <stdbool.h>

static int isKeyboardKeyPressed(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 1, LuaFunctionParameterTypePass)) {
		sgLogWarn("Bad params sent into keyboard key pressed");
		LuaPushBool(L, false);
		return 1;
	}
	int key = 0;
	if (lua_type(L, 1) == LUA_TSTRING) {
		const char* s = lua_tostring(L, 1);
		if (s && s[0] != '\0') {
			key = (int)s[0];  // ASCII
		}
	} else if (lua_type(L, 1) == LUA_TNUMBER) {
		key = (int)lua_tointeger(L, 1);
	} else {
		sgLogWarn("Bad key passed, returning false");
		LuaPushBool(L, false);
		return 1;
	}
	LuaPushBool(L, IsKeyboardKeyJustPressed(key));
	return 1;
}

static int isKeyboardKeyDown(lua_State* L) {
	if (LuaGetStackSize(L) != 1 || !LuaIsInt(L, 1)) {
		sgLogWarn("Bad params sent into keyboard key pressed from lua");
		return 0;
	}
	LuaPushBool(L, IsKeyboardKeyDown(LuaGetIntFromStacki(L, 1)));
	return 1;
}

static const luaL_Reg inputLib[] = {
	{"IsKeyboardKeyPressed", isKeyboardKeyPressed},
	{"IsKeyboardKeyDown", isKeyboardKeyDown},
	{
		"SetKeyboardKeyPressedFunc",
	},
	{NULL, NULL}};

void RegisterLuaInputFunctions(void) {
	luaL_newlib(_luaState, inputLib);
	lua_setglobal(_luaState, "cInput");
}
