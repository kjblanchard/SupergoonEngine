#include <Supergoon/Audio/Audio.h>
#include <Supergoon/lua.h>
#include <Supergoon/state.h>
#include <Supergoon/string.h>

#include "sgforge/unpack.h"
#include "sgtools/log.h"

void playBGMInternal(const char* n, float v, int l) {
	String path = StringConcat(n, ".ogg");
	char* buf;
	size_t sz;
	GetDataFromDirectory(path.Data, &buf, &sz, AssetDirectory);
	LoadBgmBuffer(path.Data, v, l, buf, sz);
	PlayBgm();
}

// 	LuaRegisterFunctionsToLuaLibrary(graphicsLib, "Graphics");

// void Engine::Audio::PlayBGMBackground(const std::string& name, float volume) {
// 	SetBgmTrack(1);
// 	playBGMInternal(name, volume);
// }

// void Engine::Audio::PlaySFXBuffer(const string& name, float volume) {
// 	sgLogDebug("Playing sfx %s", name.c_str());
// 	auto fullPath = std::format("{}.ogg", name);
// 	char* buf;
// 	size_t sz;
// 	GetDataFromDirectory(fullPath.c_str(), &buf, &sz, directory_);
// 	PlaySfxOneShot(fullPath.c_str(), volume, buf, sz);
// }

// void Engine::Audio::SetGlobalBGMVolume(float volume) {
// 	SetGlobalBgmVolume(volume);
// }

// void Engine::Audio::StopBGM() {
// 	SetBgmTrack(0);
// 	StopBgm();
// 	currentBGM_ = "";
// }

// void Engine::Audio::StopBGMBackground() {
// 	SetBgmTrack(1);
// 	StopBgm();
// }
static int playBgm(LuaState L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 3, LuaFunctionParameterTypeString, LuaFunctionParameterTypeNumber, LuaFunctionParameterTypeInt)) {
		sgLogWarn("Bad params for playbgm");
		return 0;
	}
	sgLogDebug("Playing bgm %s", LuaGetStringi(L, 1));
	SetBgmTrack(0);
	playBGMInternal(LuaGetStringi(L, 1), LuaGetFloati(L, 2), LuaGetIntFromStacki(L, 3));
	return 0;
}

static const LuaCFuncRegister audioLib[] = {
	{"PlayBgm", playBgm},
};

void RegisterLuaAudioFunctions(void) {
	LuaRegisterFunctionsToLuaLibrary(audioLib, "Audio");
}
