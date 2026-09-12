#include <Supergoon/Scripting/laudio.h>
#include <Supergoon/Scripting/lgraphics.h>
#include <Supergoon/Scripting/scripting.h>
void InitializeLuaScriptingFunctions(void) {
	RegisterLuaGraphicsFunctions();
	RegisterLuaAudioFunctions();
}
