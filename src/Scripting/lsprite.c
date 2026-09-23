#include <Supergoon/Graphics/shader.h>
#include <Supergoon/lua.h>
#include <Supergoon/sprite.h>
#include <Supergoon/state.h>
#include <Supergoon/string.h>
#include <sgtools/log.h>

static int createSprite(LuaState L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 3, LuaFunctionParameterTypeTable, LuaFunctionParameterTypeUserdata, LuaFunctionParameterTypeTable)) {
		sgLogWarn("Bad params for create sprite");
		return 0;
	}
	Sprite* s = SpriteNew();
	s->Location.x = LuaGetFloatFromTableStackIndex(L, 1, 1);
	s->Location.y = LuaGetFloatFromTableStackIndex(L, 1, 2);
	s->Location.w = LuaGetFloatFromTableStackIndex(L, 1, 3);
	s->Location.h = LuaGetFloatFromTableStackIndex(L, 1, 4);
	s->Texture = LuaGetLightUserdatai(L, 2);
	s->TextureSourceRect.x = LuaGetFloatFromTableStackIndex(L, 3, 1);
	s->TextureSourceRect.y = LuaGetFloatFromTableStackIndex(L, 3, 2);
	s->TextureSourceRect.w = LuaGetFloatFromTableStackIndex(L, 3, 3);
	s->TextureSourceRect.h = LuaGetFloatFromTableStackIndex(L, 3, 4);
	s->Shader = GetDefaultShader();
	s->Visible = true;
	LuaPushLightUserdata(L, s);
	return 1;
}

static const LuaCFuncRegister spriteLib[] = {
	{"CreateSprite", createSprite},
};

void RegisterLuaSpriteFunctions(void) {
	LuaRegisterFunctionsToLuaLibrary(spriteLib, "Sprite");
}
