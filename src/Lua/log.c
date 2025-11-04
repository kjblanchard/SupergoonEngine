#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <lauxlib.h>
#include <lua.h>

static int _logFuncRef = -1;

static void luaCustomLogFunc(const char* timeString, const char* messageString, int logLevel) {
	if (_logFuncRef == -1) {
		return;
	}
	LuaPushRefValueInLuaRegistry(_luaState, _logFuncRef);
	LuaPushString(_luaState, timeString);
	LuaPushString(_luaState, messageString);
	LuaPushInt(_luaState, logLevel);
	RunLuaFunctionOnStack(_luaState, 3);
}

static int setLogFunc(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 1, LuaFunctionParameterTypeFunction)) {
		sgLogWarn("Bad set log function params!");
		return 0;
	}
	_logFuncRef = LuaCreateRefInLuaRegistry(L, 1);
	sgSetDebugFunction(luaCustomLogFunc);
	return 0;
}

static int logFunc(lua_State* L) {
	if (LuaGetStackSize(L) != 2 || !LuaIsString(L, 1) || !LuaIsInt(L, 2)) {
		sgLogWarn("Bad log function params!");
		return 0;
	}
	int logType = LuaGetIntFromStacki(L, 2);
	switch (logType) {
		case Log_LDebug:
			sgLogDebug(LuaGetStringi(L, 1));
			break;
		case Log_LCritical:
			sgLogCritical(LuaGetStringi(L, 1));
			break;
		case Log_LError:
			sgLogError(LuaGetStringi(L, 1));
			break;
		case Log_LInfo:
			sgLogInfo(LuaGetStringi(L, 1));
			break;
		default:
			sgLogWarn(LuaGetStringi(L, 1));
			break;
	}
	return 0;
}

static const luaL_Reg uiLib[] = {
	{"Log", logFunc},
	{"SetLogFunc", setLogFunc},
	{NULL, NULL}};

void RegisterLuaLogFunctions(void) {
	luaL_newlib(_luaState, uiLib);
	lua_setglobal(_luaState, "cLog");
}
