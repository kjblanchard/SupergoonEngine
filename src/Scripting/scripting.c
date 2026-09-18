#include <Supergoon/Scripting/laudio.h>
#include <Supergoon/Scripting/lgraphics.h>
#include <Supergoon/Scripting/linput.h>
#include <Supergoon/Scripting/llog.h>
#include <Supergoon/Scripting/lscene.h>
#include <Supergoon/Scripting/lsprite.h>
#include <Supergoon/Scripting/scripting.h>
void InitializeLuaScriptingFunctions(void) {
	RegisterLuaGraphicsFunctions();
	RegisterLuaAudioFunctions();
	RegisterLuaSceneFunctions();
	RegisterLuaInputFunctions();
	RegisterLuaLogFunctions();
	RegisterLuaSpriteFunctions();
}
