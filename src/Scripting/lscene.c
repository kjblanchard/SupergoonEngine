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

static int getAllEntityData(LuaState l) {
	LuaPushNewTableToStack(l);	// array table that will be left on stack to be sent back
	if (!_currentMap) {
		sgLogWarn("No current map, cannot get gameobjects!");
		return 0;
	}
	for (int i = 0; i < _currentMap->NumObjects; ++i) {
		TiledObject* o = &_currentMap->Objects[i];
		LuaPushNewTableToStack(l);	// put all data in lua table
		LuaPushIntToTable(l, "ObjectType", o->ObjectType);
		LuaPushIntToTable(l, "X", o->X);
		LuaPushIntToTable(l, "Y", o->Y);
		LuaPushIntToTable(l, "W", o->Width);
		LuaPushIntToTable(l, "H", o->Height);
		LuaPushNewTableToStack(l);	// put all properties in a table that will be added to the object table
		for (int j = 0; j < o->NumProperties; ++j) {
			TiledProperty* p = &o->Properties[j];
			switch (p->PropertyType) {
				case TiledPropertyTypeInt:
					LuaPushIntToTable(l, p->Name, p->Data.IntData);
					break;
				case TiledPropertyTypeFloat:
					LuaPushFloatToTable(l, p->Name, p->Data.FloatData);
					break;
				case TiledPropertyTypeString:
					LuaPushStringToTable(l, p->Name, p->Data.StringData);
					break;
			}
		}
		LuaPushTableToTable(l, "Properties");  // add properties table to object table
		LuaPushValueToArrayTable(l);		   // Add object to the array table
	}
	return 1;  // return array table
}

static const LuaCFuncRegister sceneLib[] = {
	{"LoadScene", loadScene},
	{"GetGameObjectData", getAllEntityData},
};

void RegisterLuaSceneFunctions(void) {
	LuaRegisterFunctionsToLuaLibrary(sceneLib, "Scene");
}
