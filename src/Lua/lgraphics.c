#include <Supergoon/Graphics/graphics.h>
#include <Supergoon/Graphics/shader.h>
#include <Supergoon/Graphics/texture.h>
#include <Supergoon/Primitives/Color.h>
#include <Supergoon/Primitives/rectangle.h>
#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <Supergoon/text.h>
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

static int create9SliceImage(lua_State *L) {
	// args - dst, filename, color, xSize of 9slice, ySize of 9slice
	if (!LuaCheckFunctionCallParamsAndTypes(L, 5, LuaFunctionParameterTypeTable, LuaFunctionParameterTypeString, LuaFunctionParameterTypeTable, LuaFunctionParameterTypeInt, LuaFunctionParameterTypeInt)) {
		LuaPushNil(L);
		return 1;
	}
	RectangleF dst = {
		LuaGetFloatFromTableStackiKey(L, 1, "x"),
		LuaGetFloatFromTableStackiKey(L, 1, "y"),
		LuaGetFloatFromTableStackiKey(L, 1, "w"),
		LuaGetFloatFromTableStackiKey(L, 1, "h"),
	};
	Color color = {
		LuaGetFloatFromTableStackiKey(L, 3, "r"),
		LuaGetFloatFromTableStackiKey(L, 3, "g"),
		LuaGetFloatFromTableStackiKey(L, 3, "b"),
		LuaGetFloatFromTableStackiKey(L, 3, "a"),
	};
	const char *filename = LuaGetStringi(L, 2);
	Texture *renderTargetTexture = TextureCreateRenderTarget(dst.w, dst.h);
	Texture *nineSliceImageTexture = TextureCreate();
	TextureLoadFromBmp(nineSliceImageTexture, filename);
	int nineSliceImageW = TextureGetWidth(nineSliceImageTexture);
	int nineSliceImageH = TextureGetHeight(nineSliceImageTexture);
	TextureClearRenderTarget(renderTargetTexture, color.R / (float)255, color.G / (float)255, color.B / (float)255, color.A / (float)255);
	float sizeX = LuaGetFloati(L, 4);
	float sizeY = LuaGetFloati(L, 5);
	// / Draw the corners
	// tl
	RectangleF srcRect = {0, 0, sizeX, sizeY};
	RectangleF dstRect = {0, 0, sizeX, sizeY};
	DrawTextureToTexture(renderTargetTexture, nineSliceImageTexture, GetDefaultShader(), &dstRect, &srcRect, 1.0);
	// tr
	srcRect = (RectangleF){nineSliceImageW - sizeX, 0, sizeX, sizeY};
	dstRect = (RectangleF){dst.w - sizeX, 0, sizeX, sizeY};
	DrawTextureToTexture(renderTargetTexture, nineSliceImageTexture, GetDefaultShader(), &dstRect, &srcRect, 1.0);
	// // bl
	srcRect = (RectangleF){0, nineSliceImageH - sizeY, sizeX, sizeY};
	dstRect = (RectangleF){0, dst.h - sizeY, sizeX, sizeY};
	DrawTextureToTexture(renderTargetTexture, nineSliceImageTexture, GetDefaultShader(), &dstRect, &srcRect, 1.0);
	// br
	srcRect = (RectangleF){nineSliceImageW - sizeX, nineSliceImageH - sizeY, sizeX, sizeY};
	dstRect = (RectangleF){dst.w - sizeX, dst.h - sizeY, sizeX, sizeY};
	DrawTextureToTexture(renderTargetTexture, nineSliceImageTexture, GetDefaultShader(), &dstRect, &srcRect, 1.0);
	// draw the bars
	int length = dst.w - (sizeX);
	int height = dst.h - (sizeY);
	// top
	srcRect = (RectangleF){1 + sizeX, 0, 1, sizeY};
	for (size_t i = sizeX; i < length; i++) {
		dstRect = (RectangleF){(float)i, 0, 1, sizeY};
		DrawTextureToTexture(renderTargetTexture, nineSliceImageTexture, GetDefaultShader(), &dstRect, &srcRect, 1.0);
	}
	// bottom
	for (size_t i = sizeX; i < length; i++) {
		dstRect = (RectangleF){(float)i, dst.h - sizeY + 4, 1, sizeY};
		DrawTextureToTexture(renderTargetTexture, nineSliceImageTexture, GetDefaultShader(), &dstRect, &srcRect, 1.0);
	}
	// left
	srcRect = (RectangleF){0, sizeY + 1, sizeX, 1};
	for (size_t i = sizeY; i < height; i++) {
		dstRect = (RectangleF){0, (float)i, sizeX, 1};
		DrawTextureToTexture(renderTargetTexture, nineSliceImageTexture, GetDefaultShader(), &dstRect, &srcRect, 1.0);
	}
	// right
	for (size_t i = sizeY; i < height; i++) {
		dstRect = (RectangleF){dst.w - sizeX + 3, (float)i, sizeX, 1};
		DrawTextureToTexture(renderTargetTexture, nineSliceImageTexture, GetDefaultShader(), &dstRect, &srcRect, 1.0);
	}
	UnloadTexture(nineSliceImageTexture);
	LuaPushLightUserdata(L, renderTargetTexture);
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
	Color color = {255, 255, 255, 255};

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

static int getStandardSpriteShader(lua_State *L) {
	LuaPushLightUserdata(L, GetDefaultShader());
	GetDefaultShader();
	return 1;
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

static int drawRect(lua_State *L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 3, LuaFunctionParameterTypeTable, LuaFunctionParameterTypeBoolean, LuaFunctionParameterTypeBoolean)) {
		sgLogWarn("Bad params for drawRect");
		return 0;
	}

	RectangleF rect = {
		LuaGetFloatFromTableStackiKey(L, 1, "x"),
		LuaGetFloatFromTableStackiKey(L, 1, "y"),
		LuaGetFloatFromTableStackiKey(L, 1, "w"),
		LuaGetFloatFromTableStackiKey(L, 1, "h")};
	Color color = {255, 255, 255, 255};
	DrawRect(&rect, &color, LuaGetBooli(L, 2), LuaGetBooli(L, 3));
	return 0;
}

static int drawNineSlice(lua_State *L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 2, LuaFunctionParameterTypeUserdata, LuaFunctionParameterTypeTable)) {
		sgLogWarn("Bad params for nine slice draw");
		return 0;
	}
	Texture *texture = LuaGetLightUserdatai(L, 1);
	RectangleF dst;
	dst.x = LuaGetFloatFromTableStackiKey(L, 2, "x");
	dst.y = LuaGetFloatFromTableStackiKey(L, 2, "y");
	dst.w = LuaGetFloatFromTableStackiKey(L, 2, "w");
	dst.h = LuaGetFloatFromTableStackiKey(L, 2, "h");
	RectangleF src = {0, 0, TextureGetWidth(texture), TextureGetHeight(texture)};
	Color color = {255, 255, 255, 255};
	DrawTexture(texture, GetDefaultShader(), &dst,
				&src, false, 1.0, false, &color);
	return 0;
}

static const luaL_Reg graphicsLib[] = {
	{"CreateShader", createShader},
	{"CreateTexture", createTexture},
	{"Create9SliceTexture", create9SliceImage},
	{"CreateRenderTargetTexture", createRenderTargetTexture},
	{"ClearRenderTargetTexture", clearRenderTargetTexture},
	{"SetRenderTarget", setRenderTarget},
	{"SetPreviousRenderTarget", setPreviousRenderTarget},
	{"DrawTexture", drawTexture},
	{"DrawNineSlice", drawNineSlice},
	{"DrawTextureToTexture", drawTextureToTexture},
	{"DrawRect", drawRect},
	{"GetSpriteShader", getStandardSpriteShader},
	{NULL, NULL}};

void RegisterLuaGraphicsFunctions(void) {
	luaL_newlib(_luaState, graphicsLib);
	lua_setglobal(_luaState, "cGraphics");
}
