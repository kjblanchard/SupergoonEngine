#include <Supergoon/Graphics/shader.h>
#include <Supergoon/Graphics/texture.h>
#include <Supergoon/Primitives/Color.h>
#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <lauxlib.h>
#include <lua.h>
static int createShader(lua_State *L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 2, LuaFunctionParameterTypeString,
											LuaFunctionParameterTypeString)) {
		sgLogWarn(
			"Bad function call to create shader, requires two strings for "
			"filenames for shader source");
		return 0;
	}
	Shader *shader = ShaderCreate();
	ShaderCompile(shader, LuaGetStringi(L, 1), LuaGetStringi(L, 2));
	LuaPushLightUserdata(L, shader);
	return 1;
}

static int createTexture(lua_State *L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 1,
											LuaFunctionParameterTypeString)) {
		sgLogWarn(
			"Bad function call to create texture, requires two strings for "
			"filenames for shader source");
		return 0;
	}
	Texture *texture = TextureCreate();
	TextureLoadFromBmp(texture, LuaGetStringi(L, 1));
	LuaPushLightUserdata(L, texture);
	return 1;
}

static int drawTexture(lua_State *L) {
	if (!LuaCheckFunctionCallParamsAndTypes(
			L, 4, LuaFunctionParameterTypeUserdata,
			LuaFunctionParameterTypeUserdata, LuaFunctionParameterTypeTable,
			LuaFunctionParameterTypeTable)) {
		sgLogWarn(
			"Bad function call to create shader, requires two strings for "
			"filenames for shader source");
		return 0;
	}
	RectangleF dst;
	dst.x = LuaGetFloatFromTableStackiKey(L, 3, "x");
	dst.y = LuaGetFloatFromTableStackiKey(L, 3, "y");
	dst.w = LuaGetFloatFromTableStackiKey(L, 3, "w");
	dst.h = LuaGetFloatFromTableStackiKey(L, 3, "h");
	RectangleF src;
	src.x = LuaGetFloatFromTableStackiKey(L, 4, "x");
	src.y = LuaGetFloatFromTableStackiKey(L, 4, "y");
	src.w = LuaGetFloatFromTableStackiKey(L, 4, "w");
	src.h = LuaGetFloatFromTableStackiKey(L, 4, "h");
	sgColor color = {255, 255, 255, 255};

	DrawTexture(LuaGetLightUserdatai(L, 1), LuaGetLightUserdatai(L, 2), &dst,
				&src, true, 1.0, false, &color);
	return 0;
}
//    return cGraphics.DrawTextureToTexture(renderTargetTexture, srcTexture,
//    shader, dstRect, srcRect)
//
static int drawTextureToTexture(lua_State *L) {
	if (!LuaCheckFunctionCallParamsAndTypes(
			L, 5, LuaFunctionParameterTypeUserdata,
			LuaFunctionParameterTypeUserdata, LuaFunctionParameterTypeUserdata,
			LuaFunctionParameterTypeTable, LuaFunctionParameterTypeTable)) {
		sgLogWarn("Bad params passed to create render target texture");
		return 0;
	}
	RectangleF dst;
	dst.x = LuaGetFloatFromTableStackiKey(L, 4, "x");
	dst.y = LuaGetFloatFromTableStackiKey(L, 4, "y");
	dst.w = LuaGetFloatFromTableStackiKey(L, 4, "w");
	dst.h = LuaGetFloatFromTableStackiKey(L, 4, "h");
	RectangleF src;
	src.x = LuaGetFloatFromTableStackiKey(L, 5, "x");
	src.y = LuaGetFloatFromTableStackiKey(L, 5, "y");
	src.w = LuaGetFloatFromTableStackiKey(L, 5, "w");
	src.h = LuaGetFloatFromTableStackiKey(L, 5, "h");
	DrawTextureToTexture(LuaGetLightUserdatai(L, 1), LuaGetLightUserdatai(L, 2),
						 LuaGetLightUserdatai(L, 3), &dst, &src, 1.0);
	return 0;
}

static int createRenderTargetTexture(lua_State *L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 2, LuaFunctionParameterTypeInt,
											LuaFunctionParameterTypeInt)) {
		sgLogWarn("Bad params passed to create render target texture");
		return 0;
	}
	Texture *texture = TextureCreateRenderTarget(LuaGetIntFromStacki(L, 1),
												 LuaGetIntFromStacki(L, 2));
	LuaPushLightUserdata(L, texture);
	return 1;
}

static int setRenderTarget(lua_State *L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 1, LuaFunctionParameterTypePass)) {
		sgLogWarn("Bad params passed to set render target texture");
		return 0;
	}
	Texture *texture = (Texture *)LuaGetLightUserdatai(L, 1);
	SetRenderTarget(texture);
	return 1;
}

static int setPreviousRenderTarget(lua_State *L) {
	SetPreviousRenderTarget();
	return 0;
}

static int clearRenderTargetTexture(lua_State *L) {
	if (!LuaCheckFunctionCallParamsAndTypes(
			L, 5, LuaFunctionParameterTypeUserdata,
			LuaFunctionParameterTypeNumber, LuaFunctionParameterTypeNumber,
			LuaFunctionParameterTypeNumber, LuaFunctionParameterTypeNumber)) {
		sgLogWarn("Bad params passed to clear render target texture");
		return 0;
	}
	Texture *texture = (Texture *)LuaGetLightUserdatai(L, 1);
	TextureClearRenderTarget(texture, LuaGetIntFromStacki(L, 2),
							 LuaGetIntFromStacki(L, 3), LuaGetIntFromStacki(L, 4),
							 LuaGetIntFromStacki(L, 5));
	return 1;
}

static const luaL_Reg graphicsLib[] = {
	{"CreateShader", createShader},
	{"CreateTexture", createTexture},
	{"CreateRenderTargetTexture", createRenderTargetTexture},
	{"ClearRenderTargetTexture", clearRenderTargetTexture},
	{"SetRenderTarget", setRenderTarget},
	{"SetPreviousRenderTarget", setPreviousRenderTarget},
	{"DrawTexture", drawTexture},
	{"DrawTextureToTexture", drawTextureToTexture},
	{NULL, NULL}};

void RegisterLuaGraphicsFunctions(void) {
	luaL_newlib(_luaState, graphicsLib);
	lua_setglobal(_luaState, "cGraphics");
}
