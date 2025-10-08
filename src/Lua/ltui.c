#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <SupergoonEngine/Lua/tui.h>
#include <SupergoonEngine/TUI/panel.h>
#include <SupergoonEngine/TUI/text.h>
#include <lauxlib.h>
#include <lua.h>

static int createPanel(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 4, LuaFunctionParameterTypeInt, LuaFunctionParameterTypeInt, LuaFunctionParameterTypeInt, LuaFunctionParameterTypeInt)) {
		return 0;
	}
	void* panel = PanelCreate(LuaGetIntFromStacki(L, 1), LuaGetIntFromStacki(L, 2), LuaGetIntFromStacki(L, 3), LuaGetIntFromStacki(L, 4));
	LuaPushLightUserdata(L, panel);
	return 1;
}

static int createText(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 4, LuaFUnctionParameterTypeUserdata, LuaFunctionParameterTypeInt, LuaFunctionParameterTypeInt, LuaFunctionParameterTypeString)) {
		return 0;
	}
	void* text = TextCreate(LuaGetLightUserdatai(L, 1), LuaGetIntFromStacki(L, 2), LuaGetIntFromStacki(L, 3), LuaGetStringi(L, 4));
	LuaPushLightUserdata(L, text);
	return 1;
}

static int updateText(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 2, LuaFUnctionParameterTypeUserdata, LuaFunctionParameterTypeString)) {
		return 0;
	}
	TextUpdateText(LuaGetLightUserdatai(L, 1), LuaGetStringi(L, 2));
	return 0;
}

static int addChildToPanel(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 3, LuaFUnctionParameterTypeUserdata, LuaFUnctionParameterTypeUserdata, LuaFunctionParameterTypeInt)) {
		return 0;
	}
	int childType = LuaGetIntFromStacki(L, 3);
	TUIDrawCallback func = NULL;

	switch (childType) {
		case 1:
			func = TextDraw;
			break;

		default:
			break;
	}
	PanelAddChild(LuaGetLightUserdatai(L, 1), LuaGetLightUserdatai(L, 2), func);
	return 0;
}

static int createPanelEx(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 6, LuaFunctionParameterTypeInt, LuaFunctionParameterTypeInt, LuaFunctionParameterTypeInt, LuaFunctionParameterTypeInt, LuaFunctionParameterTypeBoolean, LuaFunctionParameterTypeString)) {
		return 0;
	}
	void* panel = PanelCreate(LuaGetIntFromStacki(L, 1), LuaGetIntFromStacki(L, 2), LuaGetIntFromStacki(L, 3), LuaGetIntFromStacki(L, 4));
	PanelAddBorder(panel, LuaGetBooli(L, 5));
	PanelAddName(panel, LuaGetStringi(L, 6));
	LuaPushLightUserdata(L, panel);
	return 1;
}

static int drawPanel(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 1, LuaFUnctionParameterTypeUserdata)) {
		return 0;
	}
	PanelDraw(LuaGetLightUserdatai(L, 1));
	return 0;
}

static const luaL_Reg tuiLib[] = {
	{"NewPanel", createPanel},
	{"NewText", createText},
	{"UpdateText", updateText},
	{"NewPanelEx", createPanelEx},
	{"DrawPanel", drawPanel},
	{"AddChild", addChildToPanel},
	{NULL, NULL}};

void RegisterLuaTuiFunctions(void) {
	luaL_newlib(_luaState, tuiLib);
	lua_setglobal(_luaState, "cTUI");
}
