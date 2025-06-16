#include <Supergoon/Animation/animator.h>
#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <SupergoonEngine/Animation/animator.h>
#include <SupergoonEngine/Lua/animation.h>
#include <lauxlib.h>
#include <lua.h>
// Pass in the animator file name, and the sprite to be used.  Sprite will be taken over by animator.
static int newAnimator(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(_luaState, 2, LuaFunctionParameterTypeString, LuaFUnctionParameterTypeUserdata)) {
		return 0;
	}
	AnimatorHandle animHandle = CreateAnimator(LuaGetStringi(L, 1));
	Animator* anim = &_animators.Animators[animHandle];
	anim->Sprite = LuaGetLightUserdatai(L, 2);
	LuaPushInt(L, animHandle);
	return 1;
}

static int playAnimation(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(_luaState, 2, LuaFunctionParameterTypeInt, LuaFunctionParameterTypeString)) {
		return 0;
	}
	PlayAnimation(LuaGetIntFromStacki(L, 1), LuaGetStringi(L, 2));
	return 0;
}

static int setAnimatorSpeed(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(_luaState, 2, LuaFunctionParameterTypeInt, LuaFunctionParameterTypeNumber)) {
		return 0;
	}
	SetAnimatorAnimationSpeed(LuaGetIntFromStacki(L, 1), LuaGetFloati(L, 2));
	return 0;
}

static int destroyAnimator(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(_luaState, 1, LuaFunctionParameterTypeInt)) {
		return 0;
	}
	DestroyAnimator(LuaGetIntFromStacki(L, 1));
	return 0;
}

static const luaL_Reg animationLib[] = {
	{"CreateAnimator", newAnimator},
	{"PlayAnimation", playAnimation},
	{"SetAnimatorSpeed", setAnimatorSpeed},
	{"DestroyAnimator", destroyAnimator},
	{NULL, NULL}};

void RegisterLuaAnimationFunctions(void) {
	luaL_newlib(_luaState, animationLib);
	lua_setglobal(_luaState, "cAnimation");
}
