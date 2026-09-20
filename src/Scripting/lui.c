#include <Supergoon/Scripting/lui.h>
#include <Supergoon/UI/animation.h>
#include <Supergoon/UI/image.h>
#include <Supergoon/UI/object.h>
#include <Supergoon/lua.h>
#include <Supergoon/sprite.h>
#include <Supergoon/state.h>
#include <string.h>

#include "sgtools/log.h"

static UIObject* createUIObject(LuaState L) {
	UIObject* o = UIObjectCreate();
	o->Name = strdup(LuaGetStringi(L, 1));
	o->Rect = (RectangleF){
		LuaGetFloatFromTableStackIndex(L, 2, 1),
		LuaGetFloatFromTableStackIndex(L, 2, 2),
		LuaGetFloatFromTableStackIndex(L, 2, 3),
		LuaGetFloatFromTableStackIndex(L, 2, 4),
	};
	o->Priority = (unsigned int)LuaGetIntFromStacki(L, 4);
	UIObjectAddChild(LuaGetLightUserdatai(L, 3), o);
	return o;
}

// Name, rect, parent, priority
static int createUIObjectL(LuaState L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 4, LuaFunctionParameterTypeString, LuaFunctionParameterTypeTable, LuaFunctionParameterTypePass, LuaFunctionParameterTypeInt)) {
		sgLogWarn("Bad params for create UIObject");
		return 0;
	}
	UIObject* o = createUIObject(L);
	LuaPushLightUserdata(L, o);
	return 1;
}

// uiobject, sprite
static int createUiImage(LuaState L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 2, LuaFunctionParameterTypeUserdata, LuaFunctionParameterTypeUserdata)) {
		sgLogWarn("Bad params for create UIImage");
		return 0;
	}
	SpriteSetManual(LuaGetLightUserdatai(L, 2), true);
	UIObject* o = CreateUIImage(&(UIImageArgs){
		.Object = LuaGetLightUserdatai(L, 1),
		.Sprite = LuaGetLightUserdatai(L, 2),
	});
	LuaPushLightUserdata(L, o);
	return 1;
}

static int createUIAnimation(LuaState L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 3, LuaFunctionParameterTypeUserdata, LuaFunctionParameterTypeUserdata, LuaFunctionParameterTypeUserdata)) {
		sgLogWarn("Bad params for create UIImage");
		return 0;
	}
	UIObject* o = UICreateAnimator(&(UIAnimatorArgs){
		.Object = LuaGetLightUserdatai(L, 1),
		.Sprite = LuaGetLightUserdatai(L, 2),
		.Animator = LuaGetLightUserdatai(L, 3),
	});
	SpriteSetManual(LuaGetLightUserdatai(L, 2), true);
	LuaPushLightUserdata(L, o);
	return 1;
}

static int setRootUIObject(LuaState L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 1, LuaFunctionParameterTypeUserdata)) {
		sgLogWarn("Bad params for set UI root");
		return 0;
	}
#ifdef imgui
	RootUIObject = LuaGetLightUserdatai(L, 1);
#endif
	return 0;
}

static int drawUIObject(LuaState L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 1, LuaFunctionParameterTypeUserdata)) {
		sgLogWarn("Bad params for draw uiobject");
		return 0;
	}
	UIObjectDraw(LuaGetLightUserdatai(L, 1), &(Vector2){0, 0});
	return 1;
}

static const LuaCFuncRegister uiLib[] = {
	{"CreateUIObject", createUIObjectL},
	{"CreateUIImage", createUiImage},
	{"CreateUIAnimator", createUIAnimation},
	{"DrawUIObject", drawUIObject},
	{"SetRootUI", setRootUIObject},
};

void RegisterLuaUIFunctions(void) {
	LuaRegisterFunctionsToLuaLibrary(uiLib, "UI");
}
