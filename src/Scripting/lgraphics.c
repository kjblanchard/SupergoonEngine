#include <Supergoon/Graphics/graphics.h>
#include <Supergoon/Scripting/lgraphics.h>
#include <Supergoon/lua.h>
#include <Supergoon/window.h>
#include <sgtools/log.h>

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
		sgLogWarn("Bad params for set window");
		return 0;
	}
	GraphicsSetLogicalWorldSize(LuaGetIntFromStacki(L, 1), LuaGetIntFromStacki(L, 2));
	return 0;
}

static const LuaCFuncRegister graphicsLib[] = {
	{"SetWindow", setWindow},
	{"SetLogicalWorld", setLogical},
};

void RegisterLuaGraphicsFunctions(void) {
	LuaRegisterFunctionsToLuaLibrary(graphicsLib, "Graphics");
}
