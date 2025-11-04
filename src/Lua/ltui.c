#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <SupergoonEngine/Lua/tui.h>
#include <SupergoonEngine/TUI/panel.h>
#include <SupergoonEngine/TUI/textbox.h>
#include <lauxlib.h>
#include <lua.h>
#include <ncurses.h>
#include <stdbool.h>

static int createPanel(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 4, LuaFunctionParameterTypeInt, LuaFunctionParameterTypeInt, LuaFunctionParameterTypeInt, LuaFunctionParameterTypeInt)) {
		return 0;
	}
	void* panel = PanelCreate(LuaGetIntFromStacki(L, 1), LuaGetIntFromStacki(L, 2), LuaGetIntFromStacki(L, 3), LuaGetIntFromStacki(L, 4));
	LuaPushLightUserdata(L, panel);
	return 1;
}

static int createText(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 4, LuaFunctionParameterTypeUserdata, LuaFunctionParameterTypeInt, LuaFunctionParameterTypeInt, LuaFunctionParameterTypeString)) {
		return 0;
	}
	void* text = TextboxCreate(LuaGetLightUserdatai(L, 1), LuaGetIntFromStacki(L, 2), LuaGetIntFromStacki(L, 3), LuaGetStringi(L, 4));
	LuaPushLightUserdata(L, text);
	return 1;
}

static int updateText(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 2, LuaFunctionParameterTypeUserdata, LuaFunctionParameterTypeString)) {
		return 0;
	}
	TextboxUpdateText(LuaGetLightUserdatai(L, 1), LuaGetStringi(L, 2));
	return 0;
}

static int addLineTextWithColor(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 3, LuaFunctionParameterTypeUserdata, LuaFunctionParameterTypeString, LuaFunctionParameterTypeInt)) {
		return 0;
	}
	TextboxAddLineTextWithColor(LuaGetLightUserdatai(L, 1), LuaGetStringi(L, 2), LuaGetIntFromStacki(L, 3));
	return 0;
}

static int addText(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 2, LuaFunctionParameterTypeUserdata, LuaFunctionParameterTypeString, LuaFunctionParameterTypeInt)) {
		return 0;
	}
	TextboxAddText(LuaGetLightUserdatai(L, 1), LuaGetStringi(L, 2));
	return 0;
}

static int setTextBoxStyle(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 2, LuaFunctionParameterTypeUserdata, LuaFunctionParameterTypeInt)) {
		return 0;
	}
	TextboxAddStyle(LuaGetLightUserdatai(L, 1), LuaGetIntFromStacki(L, 2));
	return 0;
}

static int setTextBoxColor(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 2, LuaFunctionParameterTypeUserdata, LuaFunctionParameterTypeInt)) {
		return 0;
	}
	TextboxAddColor(LuaGetLightUserdatai(L, 1), LuaGetIntFromStacki(L, 2));
	return 0;
}

static int addChildToPanel(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 3, LuaFunctionParameterTypeUserdata, LuaFunctionParameterTypeUserdata, LuaFunctionParameterTypeInt)) {
		return 0;
	}
	int childType = LuaGetIntFromStacki(L, 3);
	TUIDrawCallback func = NULL;

	switch (childType) {
		case 1:
			func = TextboxDraw;
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
	if (!LuaCheckFunctionCallParamsAndTypes(L, 1, LuaFunctionParameterTypeUserdata)) {
		return 0;
	}
	PanelDraw(LuaGetLightUserdatai(L, 1));
	return 0;
}

static int setShowCursor(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 1, LuaFunctionParameterTypeBoolean)) {
		return 0;
	}
	bool shouldEcho = LuaGetBooli(L, 1);
	int shouldShowCursor = shouldEcho ? 1 : 0;
	curs_set(shouldShowCursor);
	return 0;
}

static int setWindowCursor(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 3, LuaFunctionParameterTypeUserdata, LuaFunctionParameterTypeInt, LuaFunctionParameterTypeInt)) {
		return 0;
	}
	WINDOW* window = PanelGetCursesWindow(LuaGetLightUserdatai(L, 1));
	wmove(window, LuaGetIntFromStacki(L, 2), LuaGetIntFromStacki(L, 3));
	wrefresh(window);
	return 0;
}

static int setPanelFocus(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 2, LuaFunctionParameterTypeUserdata, LuaFunctionParameterTypeBoolean)) {
		return 0;
	}
	PanelSetFocus(LuaGetLightUserdatai(L, 1), LuaGetBooli(L, 2));
	return 0;
}

static const luaL_Reg tuiLib[] = {
	{"NewPanel", createPanel},
	{"SetPanelFocus", setPanelFocus},
	{"NewText", createText},
	{"UpdateText", updateText},
	{"AddLineTextWithColor", addLineTextWithColor},
	{"AddText", addText},
	{"AddColor", setTextBoxColor},
	{"AddStyle", setTextBoxStyle},
	{"NewPanelEx", createPanelEx},
	{"DrawPanel", drawPanel},
	{"AddChild", addChildToPanel},
	{"SetShowCursor", setShowCursor},
	{"SetCursorPosition", setWindowCursor},
	{NULL, NULL}};

void RegisterLuaTuiFunctions(void) {
	luaL_newlib(_luaState, tuiLib);
	lua_setglobal(_luaState, "cTUI");
}
