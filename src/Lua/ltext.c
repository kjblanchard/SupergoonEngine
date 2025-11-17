#include <Supergoon/Graphics/texture.h>
#include <Supergoon/Primitives/rectangle.h>
#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <Supergoon/text.h>
#include <assert.h>
#include <lauxlib.h>
#include <lua.h>
#include <string.h>

static int createText(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(_luaState, 7, LuaFunctionParameterTypeString, LuaFunctionParameterTypeInt, LuaFunctionParameterTypeTable, LuaFunctionParameterTypeString, LuaFunctionParameterTypeInt, LuaFunctionParameterTypeBoolean, LuaFunctionParameterTypeBoolean)) {
		sgLogWarn("trying to create text with bad params");
		return 0;
	}
	if (!TextSetFont(LuaGetStringi(L, 1), LuaGetIntFromStacki(L, 2))) {
		sgLogWarn("bad font passed");
		return 0;
	};
	RectangleF location = {
		LuaGetFloatFromTableStackiKey(L, 3, "x"),
		LuaGetFloatFromTableStackiKey(L, 3, "y"),
		LuaGetFloatFromTableStackiKey(L, 3, "w"),
		LuaGetFloatFromTableStackiKey(L, 3, "h"),
	};
	Text* text = TextCreate(&location, LuaGetStringi(L, 4));

	text->NumLettersToDraw = LuaGetIntFromStacki(L, 5);
	text->CenteredX = LuaGetBooli(L, 6);
	text->CenteredY = LuaGetBooli(L, 7);
	text->WordWrap = true;
	TextLoad(text);
	LuaPushLightUserdata(L, text);
	return 1;
}

static int updateText(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 1, LuaFunctionParameterTypeUserdata)) {
		sgLogWarn("bad update text params");
		return 0;
	}
	Text* text = (Text*)LuaGetLightUserdatai(L, 1);
	if (text) TextOnDirty(text);
	return 0;
}

static int updateLettersText(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 2, LuaFunctionParameterTypeUserdata, LuaFunctionParameterTypeNumber)) {
		sgLogWarn("bad update text lettersparams");
		return 0;
	}
	Text* text = (Text*)LuaGetLightUserdatai(L, 1);
	if (!text) return 0;
	text->NumLettersToDraw = LuaGetIntFromStacki(L, 2);
	TextOnDirty(text);

	return 0;
}

static int updateTextText(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 2, LuaFunctionParameterTypeUserdata, LuaFunctionParameterTypeString)) {
		sgLogWarn("bad update text params");
		return 0;
	}
	Text* text = (Text*)LuaGetLightUserdatai(L, 1);
	if (!text) return 0;
	if (text->Text) free(text->Text);
	if (text->Texture) TextureDestroy(text->Texture);
	text->Texture = NULL;
	text->Text = strdup(LuaGetStringi(L, 2));
	text->NumLettersToDraw = strlen(text->Text);
	if (text) TextOnDirty(text);
	return 0;
}

static int drawText(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(_luaState, 3, LuaFunctionParameterTypeUserdata, LuaFunctionParameterTypeNumber, LuaFunctionParameterTypeNumber)) {
		sgLogWarn("trying to draw text with bad params");
		return 0;
	}
	TextDraw(LuaGetLightUserdatai(L, 1), LuaGetFloati(L, 2), LuaGetFloati(L, 3));
	return 0;
}
static int setTextCentered(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(_luaState, 3, LuaFunctionParameterTypeUserdata, LuaFunctionParameterTypeBoolean, LuaFunctionParameterTypeBoolean)) {
		sgLogWarn("trying to set text centered badly");
		return 0;
	}
	Text* text = LuaGetLightUserdatai(L, 1);
	text->CenteredX = LuaGetBooli(L, 2);
	text->CenteredY = LuaGetBooli(L, 3);
	TextRedrawText(text);
	TextOnDirty(text);
	return 0;
}

static int destroyText(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(_luaState, 1, LuaFunctionParameterTypeUserdata)) {
		sgLogWarn("trying to destroy text badly");
		return 0;
	}
	TextDestroy(LuaGetLightUserdatai(L, 1));
	return 0;
}

static const luaL_Reg textLib[] = {
	{"CreateText", createText},
	{"DestroyText", destroyText},
	{"UpdateText", updateText},
	{"UpdateTextText", updateTextText},
	{"SetTextCentered", setTextCentered},
	{"SetTextNumLetters", updateLettersText},
	{"DrawText", drawText},
	{NULL, NULL}};

void RegisterLuaTextFunctions(void) {
	luaL_newlib(_luaState, textLib);
	lua_setglobal(_luaState, "cText");
}
