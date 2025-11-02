#include <Supergoon/Graphics/texture.h>
#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <Supergoon/text.h>
#include <assert.h>
#include <lauxlib.h>
#include <lua.h>

static int createText(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(_luaState, 2, LuaFunctionParameterTypeString, LuaFunctionParameterTypeInt)) {
		sgLogWarn("trying to create text with bad params");
		return 0;
	}
	if (!TextSetFont(LuaGetStringi(L, 1), LuaGetIntFromStacki(L, 2))) {
		sgLogWarn("bad font passed");
		return 0;
	};
	Text* text = TextCreate();
	TextLoad(text);
	LuaPushLightUserdata(L, text);
	return 1;
}

static int drawText(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(_luaState, 1, LuaFunctionParameterTypeUserdata)) {
		sgLogWarn("trying to draw text with bad params");
		return 0;
	}
	TextDraw(LuaGetLightUserdatai(L, 1));
	return 0;
}

static const luaL_Reg textLib[] = {
	{"CreateText", createText},
	{"DrawText", drawText},
	{NULL, NULL}};

void RegisterLuaTextFunctions(void) {
	luaL_newlib(_luaState, textLib);
	lua_setglobal(_luaState, "cText");
}
