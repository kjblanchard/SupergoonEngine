#include <Supergoon/Animation/animator.h>
#include <Supergoon/Lua/animation.h>
#include <Supergoon/Lua/camera.h>
#include <Supergoon/camera.h>
#include <Supergoon/gameobject.h>
#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <assert.h>
#include <lauxlib.h>
#include <lua.h>

static int setCameraFollowTarget(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(_luaState, 1, LuaFunctionParameterTypeUserdata)) {
		sgLogWarn("Trying to set a camera follow target with invalid parameters");
		return 0;
	}
	if (LuaIsNili(L, 1)) {
		sgLogWarn("Passed a nil ref into camera follow");
		return 0;
	}
	sgGameObject* go = (sgGameObject*)LuaGetLightUserdatai(L, 1);
	if (!go) {
		sgLogWarn("Bad gameobject passed to camera follow target");
		return 0;
	}
	SetCameraFollowTarget(&go->X, &go->Y);
	return 0;
}

static const luaL_Reg cameraLib[] = {
	{"SetCameraFollowTarget", setCameraFollowTarget},
	{NULL, NULL}};

void RegisterLuaCameraFunctions(void) {
	luaL_newlib(_luaState, cameraLib);
	lua_setglobal(_luaState, "cCamera");
}
