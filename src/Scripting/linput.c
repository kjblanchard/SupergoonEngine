#include <Supergoon/Scripting/linput.h>
#include <Supergoon/lua.h>
#include <sgtools/log.h>

// static int setWindow(LuaState L) {
// 	if (!LuaCheckFunctionCallParamsAndTypes(L, 3, LuaFunctionParameterTypeInt, LuaFunctionParameterTypeInt, LuaFunctionParameterTypeString)) {
// 		sgLogWarn("Bad params for set window");
// 		return 0;
// 	}
// 	SetWindowOptions(LuaGetIntFromStacki(L, 1), LuaGetIntFromStacki(L, 2), LuaGetStringi(L, 3));
// 	return 0;
// }

static const LuaCFuncRegister inputLib[] = {
	// {"SetWindow", setWindow},
};

void RegisterLuaInputFunctions(void) {
	LuaRegisterFunctionsToLuaLibrary(inputLib, "Input");
}
