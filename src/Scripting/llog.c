#include <Supergoon/Scripting/linput.h>
#include <Supergoon/lua.h>
#include <sgtools/log.h>

static int log(LuaState L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 2, LuaFunctionParameterTypeInt, LuaFunctionParameterTypeString)) {
		sgLogWarn("Bad params for log");
		return 0;
	}
	const char* s = LuaGetStringi(L, 2);
	switch ((sgLogLevel)LuaGetIntFromStacki(L, 1)) {
		case sgLogLevelDebug:
			sgLogDebug(s);
			break;
		case sgLogLevelInfo:
			sgLogInfo(s);
			break;
		case sgLogLevelWarn:
			sgLogWarn(s);
			break;
		case sgLogLevelError:
			sgLogError(s);
			break;
		case sgLogLevelCritical:
			sgLogCritical(s);
			break;
		case sgLogLevelDefault:
		default:
			return 0;
	}
	return 0;
}

static int level(LuaState L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, LuaFunctionParameterTypeInt)) {
		sgLogWarn("Bad params for setlog");
		return 0;
	}
	sgSetLogLevel(LuaGetIntFromStacki(L, 1));
	return 0;
}

static const LuaCFuncRegister logLib[] = {
	{"Log", log},
	{"SetLogLevel", level},
};

void RegisterLuaLogFunctions(void) {
	LuaRegisterFunctionsToLuaLibrary(logLib, "Log");
}
