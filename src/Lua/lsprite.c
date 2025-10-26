#include <Supergoon/Graphics/graphics.h>
#include <Supergoon/Graphics/shader.h>
#include <Supergoon/Graphics/texture.h>
#include <Supergoon/Lua/sprite.h>
#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <Supergoon/sprite.h>
#include <lauxlib.h>
#include <lua.h>

static int newSprite(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 4, LuaFunctionParameterTypeString, LuaFunctionParameterTypePass, LuaFunctionParameterTypeTable, LuaFunctionParameterTypeTable)) {
		sgLogWarn("bad sprite args");
		return 0;
	}
	Sprite* sprite = NewSprite();
	sprite->Parent = LuaGetLightUserdatai(L, 2);
	sprite->Texture = TextureCreate();
	TextureLoadFromBmp(sprite->Texture, LuaGetStringi(L, 1));
	sprite->Shader = ShaderCreate();
	ShaderCompile(sprite->Shader, "2dSpriteVertex", "2dSpriteFragment");
	sprite->TextureSourceRect.x = LuaGetFloatFromTableStackiKey(L, 3, "x");
	sprite->TextureSourceRect.y = LuaGetFloatFromTableStackiKey(L, 3, "y");
	sprite->TextureSourceRect.h = LuaGetFloatFromTableStackiKey(L, 3, "h");
	sprite->TextureSourceRect.w = LuaGetFloatFromTableStackiKey(L, 3, "w");
	sprite->OffsetAndSizeRectF.x = LuaGetFloatFromTableStackiKey(L, 4, "x");
	sprite->OffsetAndSizeRectF.y = LuaGetFloatFromTableStackiKey(L, 4, "y");
	sprite->OffsetAndSizeRectF.h = LuaGetFloatFromTableStackiKey(L, 4, "h");
	sprite->OffsetAndSizeRectF.w = LuaGetFloatFromTableStackiKey(L, 4, "w");
	sprite->Flags |= SpriteFlagVisible;
	LuaPushLightUserdata(L, sprite);
	return 1;
}

static int setSpriteScale(lua_State* L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 2, LuaFunctionParameterTypeUserdata, LuaFunctionParameterTypeNumber)) {
		sgLogWarn("bad sprite args");
		return 0;
	}
	Sprite* sprite = (Sprite*)LuaGetLightUserdatai(L, 1);
	if (!sprite) {
		sgLogWarn("Could not convert sprite from first param, bad one");
		return 0;
	}
	sprite->Scale = LuaGetFloati(L, 2);
	return 0;
}
static int destroySprite(lua_State* L) {
	if (LuaGetStackSize(L) != 1) {
		sgLogWarn("Bad args passed to destroy sprite from lua!");
		return 0;
	}
	DestroySprite(LuaGetLightUserdatai(L, 1));
	return 0;
}

static const luaL_Reg spriteLib[] = {
	{"NewSprite", newSprite},
	{"SetSpriteScale", setSpriteScale},
	{"DestroySprite", destroySprite},
	{NULL, NULL}};

void RegisterLuaSpriteFunctions(void) {
	luaL_newlib(_luaState, spriteLib);
	lua_setglobal(_luaState, "cSprite");
}
