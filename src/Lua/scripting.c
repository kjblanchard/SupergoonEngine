#include <SupergoonEngine/Lua/animation.h>
#include <SupergoonEngine/Lua/audio.h>
#include <SupergoonEngine/Lua/camera.h>
#include <SupergoonEngine/Lua/effects.h>
#include <SupergoonEngine/Lua/engine.h>
#include <SupergoonEngine/Lua/input.h>
#include <SupergoonEngine/Lua/log.h>
#include <SupergoonEngine/Lua/object.h>
#include <SupergoonEngine/Lua/scene.h>
#include <SupergoonEngine/Lua/scripting.h>
#include <SupergoonEngine/Lua/sprite.h>
#include <SupergoonEngine/Lua/ui.h>
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
