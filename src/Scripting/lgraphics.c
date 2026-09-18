#include <Supergoon/Graphics/graphics.h>
#include <Supergoon/Scripting/lgraphics.h>
#include <Supergoon/lua.h>
#include <Supergoon/state.h>
#include <Supergoon/window.h>
#include <assert.h>
#include <sgtools/log.h>
#include <stdio.h>
#include <string.h>

#include "sgforge/unpack.h"

static int setWindow(LuaState L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 3, LuaFunctionParameterTypeInt, LuaFunctionParameterTypeInt, LuaFunctionParameterTypeString)) {
		sgLogWarn("Bad params for set window");
		return 0;
	}
	SetWindowOptions(LuaGetIntFromStacki(L, 1), LuaGetIntFromStacki(L, 2), LuaGetStringi(L, 3));
	return 0;
}

static int setLogical(LuaState L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 2, LuaFunctionParameterTypeInt, LuaFunctionParameterTypeInt)) {
		sgLogWarn("Bad params for set logical");
		return 0;
	}
	GraphicsSetLogicalWorldSize(LuaGetIntFromStacki(L, 1), LuaGetIntFromStacki(L, 2));
	return 0;
}

static int loadTextureFromPng(LuaState L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 1, LuaFunctionParameterTypeString)) {
		sgLogWarn("Bad params for load texture");
		return 0;
	}
	const char* name = LuaGetStringi(L, 1);
	Texture* t = TextureCreate(name);
	char* buf;
	size_t sz;
	GetDataFromDirectory(name, &buf, &sz, AssetDirectory);
	TextureLoadFromPngBuffer(t, name, buf, sz);
	LuaPushLightUserdata(L, t);
	return 1;
}

static const LuaCFuncRegister graphicsLib[] = {
	{"SetWindow", setWindow},
	{"SetLogicalWorld", setLogical},
	{"LoadTexture", loadTextureFromPng},
};

void RegisterLuaGraphicsFunctions(void) {
	LuaRegisterFunctionsToLuaLibrary(graphicsLib, "Graphics");
}
