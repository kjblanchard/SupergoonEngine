#include <Supergoon/Input/keyboard.h>
#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <SupergoonEngine/Lua/input.h>
#include <lauxlib.h>
#include <lua.h>

static int isKeyboardKeyPressed(lua_State* L) {
	if (LuaGetStackSize(L) != 1 || !LuaIsInt(L, 1)) {
		sgLogWarn("Bad params sent into keyboard key pressed from lua");
		return 0;
	}
	LuaPushBool(L, IsKeyboardKeyJustPressed(LuaGetIntFromStacki(L, 1)));
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
	{NULL, NULL}};

void RegisterLuaInputFunctions(void) {
	luaL_newlib(_luaState, inputLib);
	lua_setglobal(_luaState, "cInput");
}
