#include <Supergoon/Lua/animation.h>
#include <Supergoon/Lua/audio.h>
#include <Supergoon/Lua/camera.h>
#include <Supergoon/Lua/effects.h>
#include <Supergoon/Lua/engine.h>
#include <Supergoon/Lua/input.h>
#include <Supergoon/Lua/log.h>
#include <Supergoon/Lua/object.h>
#include <Supergoon/Lua/scene.h>
#include <Supergoon/Lua/scripting.h>
#include <Supergoon/Lua/sprite.h>
#include <Supergoon/Lua/ui.h>
#ifdef tui
#include <SupergoonEngine/Lua/tui.h>
#endif
void RegisterAllLuaFunctions(void) {
// should be consolidated
#ifndef tui
	RegisterLuaUIFunctions();
	RegisterLuaObjectFunctions();
	RegisterLuaSceneFuncs();
	RegisterLuaEffectsFunctions();
	RegisterLuaSpriteFunctions();
	RegisterLuaAnimationFunctions();
#else
	RegisterLuaTuiFunctions();
#endif
	RegisterLuaLogFunctions();
	RegisterLuaAudioFunctions();
	RegisterLuaEngineFunctions();
	RegisterLuaInputFunctions();
	RegisterLuaCameraFunctions();
}
