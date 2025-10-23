#include <Supergoon/Lua/animation.h>
#include <Supergoon/Lua/audio.h>
#include <Supergoon/Lua/camera.h>
#include <Supergoon/Lua/effects.h>
#include <Supergoon/Lua/engine.h>
#include <Supergoon/Lua/graphics.h>
#include <Supergoon/Lua/input.h>
#include <Supergoon/Lua/log.h>
// #include <Supergoon/Lua/object.h>
// #include <Supergoon/Lua/scene.h>
#include <Supergoon/Lua/scripting.h>
#include <Supergoon/Lua/sprite.h>
#include <Supergoon/Lua/ui.h>
void RegisterAllLuaFunctions(void) {
	RegisterLuaUIFunctions();
	// RegisterLuaObjectFunctions();
	// RegisterLuaSceneFuncs();
	RegisterLuaEffectsFunctions();
	RegisterLuaSpriteFunctions();
	RegisterLuaAnimationFunctions();
	RegisterLuaLogFunctions();
	RegisterLuaAudioFunctions();
	RegisterLuaEngineFunctions();
	RegisterLuaInputFunctions();
	RegisterLuaCameraFunctions();
	RegisterLuaGraphicsFunctions();
}
