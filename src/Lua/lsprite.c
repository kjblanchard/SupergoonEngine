#include <Supergoon/Graphics/graphics.h>
#include <Supergoon/Lua/sprite.h>
#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <Supergoon/sprite.h>
#include <lauxlib.h>
#include <lua.h>

static int newSprite(lua_State* L) {
	if (LuaGetStackSize(L) != 4 || !LuaIsString(L, 1) || !LuaIsTable(L, 3) || !LuaIsTable(L, 4)) {
		sgLogWarn("Bad args passed to new sprite from lua!");
		LuaPushNil(L);
		return 1;
	}
	Sprite* sprite = NewSprite();
	sprite->Texture = CreateTextureFromIndexedBMP(LuaGetStringi(L, 1));
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
	{"DestroySprite", destroySprite},
	{NULL, NULL}};

void RegisterLuaSpriteFunctions(void) {
	luaL_newlib(_luaState, spriteLib);
	lua_setglobal(_luaState, "cSprite");
}
