#include <Supergoon/UI/ui.h>
#include <Supergoon/lua.h>
#include <Supergoon/map.h>

#include <Supergoon/Debug/Debug.hpp>
#include <Supergoon/Debug/Scene.hpp>
#include <Supergoon/pch.hpp>
#include <string>
#include <vector>
using namespace std;

#define LUA_SCENES_FILE "assets/lua/scenes.lua"
struct scene {
	string Title;
	string UIName;
	string BGMName;
	float Volume;
	float FadeInTime;
	float FadeOutTime;
};
static vector<scene> _scenes;

static void getScenesFromLuaScenesFile() {
	LuaRunFile(LUA_SCENES_FILE);	   // Scenes.lua table is on top of stack.
	LuaGetTable(_luaState, "scenes");  // scenes table is on top of stack
	auto len = LuaGetTableLengthMap(_luaState);
	LuaStartTableKeyValueIteration(_luaState);
	for (size_t i = 0; i < len; i++) {
		if (!LuaNextTableKeyValueIterate(_luaState)) {
			break;
		}
		scene scene;
		scene.Title = string(LuaGetStringFromTablei(_luaState, 0));
		scene.UIName = string(LuaGetStringFromTablei(_luaState, 1));
		scene.BGMName = string(LuaGetStringFromTablei(_luaState, 2));
		scene.Volume = LuaGetFloatFromTablei(_luaState, 3);
		scene.FadeInTime = LuaGetFloatFromTablei(_luaState, 4);
		scene.FadeOutTime = LuaGetFloatFromTablei(_luaState, 5);
		_scenes.emplace_back(std::move(scene));	 // Don't copy, c++ man
		LuaPopStack(_luaState, 1);				 // Push the value table and the value table off
	}
	LuaEndTableKeyValueIteration(_luaState);
	LuaPopStack(_luaState, 2);
}
void InitializeSceneDebugWindow() {
	_scenes.clear();
	getScenesFromLuaScenesFile();
}
void ShowSceneDebugWindow() {
	auto window_flags = GetDefaultWindowFlags();
	bool p_open;
	if (!ImGui::Begin("Scenes", &p_open, window_flags)) {
		ImGui::End();
		return;
	}
	static int sceneCurrent = 0;
	if (ImGui::Button("Update Scenes from file")) {
		InitializeSceneDebugWindow();
	}
	if (ImGui::Button("Load Selected Scene")) {
		if (_scenes.size() > sceneCurrent) {
			LuaPushTableFromFile(_luaState, "assets/lua/Engine.lua");
			LuaGetLuaFunc(_luaState, "LoadSceneEx");
			LuaPushString(_luaState, _scenes[sceneCurrent].Title.c_str());
			LuaPushString(_luaState, _scenes[sceneCurrent].UIName.c_str());
			LuaPushString(_luaState, _scenes[sceneCurrent].BGMName.c_str());
			LuaPushFloat(_luaState, 1.0);
			LuaPushFloat(_luaState, 0.25);
			LuaPushFloat(_luaState, 0.25);
			RunLuaFunctionOnStack(_luaState, 6);
		}
	}
	LuaClearStack(_luaState);
	// Create a vector to hold the const char* pointers
	std::vector<const char*> cStrings;
	// Reserve space for performance (optional but good practice)
	cStrings.reserve(_scenes.size());
	// Extract the const char* pointers from std::string
	for (const auto& scenei : _scenes) {
		cStrings.push_back(scenei.Title.c_str());
	}
	ImGui::ListBox("Scene List", &sceneCurrent, cStrings.data(), cStrings.size(), 4);
	ImGui::End();
}
