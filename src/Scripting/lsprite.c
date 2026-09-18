#include <Supergoon/Graphics/shader.h>
#include <Supergoon/lua.h>
#include <Supergoon/sprite.h>
#include <Supergoon/state.h>
#include <Supergoon/string.h>
#include <sgtools/log.h>

static int createSprite(LuaState L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 5, LuaFunctionParameterTypeNumber, LuaFunctionParameterTypeNumber, LuaFunctionParameterTypeUserdata, LuaFunctionParameterTypeTable, LuaFunctionParameterTypeTable)) {
		sgLogWarn("Bad params for create sprite");
		return 0;
	}
	Sprite* s = SpriteNew();
	s->ParentX = LuaGetFloati(L, 1);
	s->ParentY = LuaGetFloati(L, 2);
	s->Texture = LuaGetLightUserdatai(L, 3);
	s->OffsetAndSizeRectF.x = LuaGetFloatFromTableStackIndex(L, 4, 1);
	s->OffsetAndSizeRectF.y = LuaGetFloatFromTableStackIndex(L, 4, 2);
	s->OffsetAndSizeRectF.w = LuaGetFloatFromTableStackIndex(L, 4, 3);
	s->OffsetAndSizeRectF.h = LuaGetFloatFromTableStackIndex(L, 4, 4);
	s->TextureSourceRect.x = LuaGetFloatFromTableStackIndex(L, 5, 1);
	s->TextureSourceRect.y = LuaGetFloatFromTableStackIndex(L, 5, 2);
	s->TextureSourceRect.w = LuaGetFloatFromTableStackIndex(L, 5, 3);
	s->TextureSourceRect.h = LuaGetFloatFromTableStackIndex(L, 5, 4);
	s->Shader = GetDefaultShader();
	SpriteSetVisible(s, true);
	LuaPushLightUserdata(L, s);
	return 1;
}

static const LuaCFuncRegister spriteLib[] = {
	{"CreateSprite", createSprite},
};

void RegisterLuaSpriteFunctions(void) {
	LuaRegisterFunctionsToLuaLibrary(spriteLib, "Sprite");
}
