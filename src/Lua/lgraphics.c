#include <Supergoon/Graphics/shader.h>
#include <Supergoon/Graphics/texture.h>
#include <Supergoon/log.h>
#include <Supergoon/lua.h>
#include <lauxlib.h>
#include <lua.h>
static int createShader(lua_State *L) {
  if (!LuaCheckFunctionCallParamsAndTypes(L, 2, LuaFunctionParameterTypeString,
                                          LuaFunctionParameterTypeString)) {
    sgLogWarn("Bad function call to create shader, requires two strings for "
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
    sgLogWarn("Bad function call to create texture, requires two strings for "
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
    sgLogWarn("Bad function call to create shader, requires two strings for "
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

  DrawTexture(LuaGetLightUserdatai(L, 1), LuaGetLightUserdatai(L, 2), &dst,
              &src, 1.0);
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
    {"DrawTexture", drawTexture},
    {NULL, NULL}};

void RegisterLuaGraphicsFunctions(void) {
  luaL_newlib(_luaState, graphicsLib);
  lua_setglobal(_luaState, "cGraphics");
}
