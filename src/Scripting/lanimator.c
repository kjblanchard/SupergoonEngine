#include <Supergoon/Animation/animationdata.h>
#include <Supergoon/Animation/animator.h>
#include <Supergoon/Graphics/shader.h>
#include <Supergoon/lua.h>
#include <Supergoon/sprite.h>
#include <Supergoon/state.h>
#include <Supergoon/string.h>
#include <sgforge/unpack.h>
#include <sgtools/log.h>
#include <stdio.h>

static int playAnimation(LuaState L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 3, LuaFunctionParameterTypeUserdata, LuaFunctionParameterTypeString, LuaFunctionParameterTypeInt)) {
		sgLogWarn("Bad params for create animator");
		return 0;
	}
	PlayAnimation(LuaGetLightUserdatai(L, 1), LuaGetStringi(L, 2), LuaGetIntFromStacki(L, 3));
	return 0;
}

static int createAnimationData(LuaState L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 1, LuaFunctionParameterTypeString)) {
		sgLogWarn("Bad params for create animator data");
		return 0;
	}
	const char* name = LuaGetStringi(L, 1);
	AnimationData* raw = CreateAnimationData();
	char* buf;
	size_t sz;
	char fullPath[256];
	snprintf(fullPath, sizeof fullPath, "%s.json", name);
	int result = GetDataFromDirectory(fullPath, &buf, &sz, AssetDirectory);
	if (!result || !buf || !sz) {
		sgLogWarn("Could not load json for animation data %s", name);
		return 0;
	}
	CreateAnimationDataFromAsepriteBuffer(raw, buf, sz);
	LuaPushLightUserdata(L, raw);
	return 1;
}

static int createAnimator(LuaState L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 3, LuaFunctionParameterTypeString, LuaFunctionParameterTypeUserdata, LuaFunctionParameterTypeUserdata)) {
		sgLogWarn("Bad params for create animator");
		return 0;
	}
	const char* name = LuaGetStringi(L, 1);
	Animator* a = CreateAnimator(name, LuaGetLightUserdatai(L, 2));
	a->Sprite = LuaGetLightUserdatai(L, 3);
	a->AnimationSpeed = 1.0f;
	LuaPushLightUserdata(L, a);
	return 1;
}

static const LuaCFuncRegister animatorLib[] = {
	{"CreateAnimationData", createAnimationData},
	{"CreateAnimator", createAnimator},
	{"PlayAnimation", playAnimation},
};

void RegisterLuaAnimatorFunctions(void) {
	LuaRegisterFunctionsToLuaLibrary(animatorLib, "Animator");
}
