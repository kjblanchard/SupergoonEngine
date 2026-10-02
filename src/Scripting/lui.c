#include <Supergoon/Scripting/lui.h>
#include <Supergoon/UI/animation.h>
#include <Supergoon/UI/image.h>
#include <Supergoon/UI/nineslice.h>
#include <Supergoon/UI/object.h>
#include <Supergoon/UI/text.h>
#include <Supergoon/lua.h>
#include <Supergoon/sprite.h>
#include <Supergoon/state.h>
#include <Supergoon/text.h>
#include <assert.h>
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
	Sprite* s = LuaGetLightUserdatai(L, 2);
	s->Manual = true;
	UIObject* o = CreateUIImage(&(UIImageArgs){
		.Object = LuaGetLightUserdatai(L, 1),
		.Sprite = LuaGetLightUserdatai(L, 2),
	});
	LuaPushLightUserdata(L, o);
	return 1;
}
// uiobject, texture, colortbl, offsettbl
static int createUINineSlice(LuaState L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 4, LuaFunctionParameterTypeUserdata, LuaFunctionParameterTypeUserdata, LuaFunctionParameterTypeTable, LuaFunctionParameterTypeTable)) {
		sgLogWarn("Bad params for create nineslice");
		return 0;
	}
	UIObject* o = CreateUINineSlice(&(UINineSliceArgs){
		.Object = LuaGetLightUserdatai(L, 1),
		.Texture = LuaGetLightUserdatai(L, 2),
		.NineSliceOffset = (Point){LuaGetFloatFromTableStackIndex(L, 4, 1), LuaGetFloatFromTableStackIndex(L, 4, 2)},
		.Color = (Color){
			LuaGetFloatFromTableStackIndex(L, 3, 1),
			LuaGetFloatFromTableStackIndex(L, 3, 2),
			LuaGetFloatFromTableStackIndex(L, 3, 3),
			LuaGetFloatFromTableStackIndex(L, 3, 4),
		}});
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
	Sprite* s = LuaGetLightUserdatai(L, 2);
	s->Manual = true;
	LuaPushLightUserdata(L, o);
	return 1;
}
// uiobj, text, font, size, centered, color
static int createUIText(LuaState L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 6, LuaFunctionParameterTypeUserdata, LuaFunctionParameterTypeString, LuaFunctionParameterTypeString, LuaFunctionParameterTypeInt, LuaFunctionParameterTypeBoolean, LuaFunctionParameterTypeTable)) {
		sgLogWarn("Bad params for create uitext");
		return 0;
	}
	UIObject* o = (UIObject*)LuaGetLightUserdatai(L, 1);
	assert(o);
	TextSetFont(LuaGetStringi(L, 3), (unsigned int)LuaGetIntFromStacki(L, 4), AssetDirectory);
	Text* t = TextCreate(&o->Rect, LuaGetStringi(L, 2));
	t->CenteredX = t->CenteredY = (unsigned int)LuaGetBooli(L, 5);
	t->Color = (Color){
		(uint8_t)LuaGetFloatFromTableStackIndex(L, 6, 1),
		(uint8_t)LuaGetFloatFromTableStackIndex(L, 6, 2),
		(uint8_t)LuaGetFloatFromTableStackIndex(L, 6, 3),
		(uint8_t)LuaGetFloatFromTableStackIndex(L, 6, 3),
	};
	TextLoad(t);
	CreateUIText(&(UITextArgs){
		.Text = t,
		.Object = o,
		.Color = t->Color});
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

static int setUIObjectVisible(LuaState L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 2, LuaFunctionParameterTypeUserdata, LuaFunctionParameterTypeBoolean)) {
		sgLogWarn("Bad params for set visible");
		return 0;
	}
	UIObject* o = LuaGetLightUserdatai(L, 1);
	bool visible = LuaGetBooli(L, 2);
	o->Visible = (unsigned int)LuaGetBooli(L, 2);
	return 0;
}

static const LuaCFuncRegister uiLib[] = {
	{"CreateUIObject", createUIObjectL},
	{"CreateUIImage", createUiImage},
	{"CreateUIAnimator", createUIAnimation},
	{"DrawUIObject", drawUIObject},
	{"SetRootUI", setRootUIObject},
	{"SetUIObjectVisible", setUIObjectVisible},
	{"CreateNineSlice", createUINineSlice},
	{"CreateUIText", createUIText},
};

void RegisterLuaUIFunctions(void) {
	LuaRegisterFunctionsToLuaLibrary(uiLib, "UI");
}
