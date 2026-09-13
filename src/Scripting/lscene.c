#include <Supergoon/Scripting/lscene.h>
#include <Supergoon/lua.h>
#include <Supergoon/map.h>
#include <Supergoon/state.h>
#include <Supergoon/string.h>
#include <Supergoon/window.h>
#include <assert.h>
#include <sgforge/unpack.h>
#include <sgtools/log.h>
#include <stdio.h>

static int loadScene(LuaState L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 1, LuaFunctionParameterTypeString)) {
		sgLogWarn("Bad params for load scene");
		return 0;
	}
	sgLogDebug("Starting load map");
	char* buf;
	size_t sz;
	const char* name = LuaGetStringi(L, 1);
	char nextMapName[256];
	int res = snprintf(nextMapName, sizeof nextMapName, "%s.tmj", name);
	assert(res > 0 && res < sizeof nextMapName && "load scene sprintf failure");
	int result = GetDataFromDirectory(nextMapName, &buf, &sz, AssetDirectory);
	if (!result) {
		sgLogError("Could not load map %s", nextMapName);
		return 0;
	}
	LoadMapFromBuffer(name, buf, sz);
	return 0;
}

static const LuaCFuncRegister sceneLib[] = {
	{"LoadScene", loadScene},
};

void RegisterLuaSceneFunctions(void) {
	LuaRegisterFunctionsToLuaLibrary(sceneLib, "Scene");
}
