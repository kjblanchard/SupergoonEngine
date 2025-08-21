#include <Supergoon/Imgui/imgui.h>
#include <Supergoon/gameobject.h>
#include <Supergoon/log.h>
#include <SupergoonEngine/gameobject.h>
#include <SupergoonEngine/tools.h>

#include <Supergoon/Debug/Debug.hpp>
#include <string>
using namespace std;
extern int _showingSolidBoxes;
void ShowGameObjectDebugWindow(void) {
	auto window_flags = GetDefaultWindowFlags();
	bool p_open;
	if (!ImGui::Begin("Gameobjects", &p_open, window_flags)) {
		ImGui::End();
		return;
	}
	static int activeGameObjects = 0;
	int activeGameobjectsThisFrame = 0;
	ImGui::Text("GameObject Num: %ld", _numGameObjects);
	ImGui::Text("Active GameObjects: %d", activeGameObjects);
	bool showingSolids = (bool)_showingSolidBoxes;
	if (ImGui::Checkbox("Debug Solids", &showingSolids)) {
		_showingSolidBoxes = (int)showingSolids;
	}
	for (size_t i = 0; i < _numGameObjects; i++) {
		GameObject* go = _gameObjects[i];
		if ((go->Flags & GameObjectFlagDestroyed) || (go->Flags == 0)) {
			continue;
		}
		ImGui::PushID(go->Id);
		auto nameStr = go->Name ? go->Name : to_string(go->Id);
		if (ImGui::TreeNode(nameStr.c_str())) {
			ImGui::Text("ID: %d", go->Id);
			ImGui::Text("Name: %s", go->Name);
			// ImGui::SliderFloat("X: ", &go->X, -500, 2000);
			ImGui::DragFloat("X", &go->X);
			ImGui::DragFloat("Y", &go->Y);
			// ImGui::SliderFloat("Y: ", &go->Y, -500, 2000);
			auto debugDraw = HAS_ALL_FLAGS(go->Flags, GameObjectFlagDebugDraw);
			if (ImGui::Checkbox("Debug Box: ", &debugDraw)) {
				if (debugDraw) {
					SET_FLAG(go->Flags, GameObjectFlagDebugDraw);
				} else {
					CLEAR_FLAG(go->Flags, GameObjectFlagDebugDraw);
				}
			}
			ImGui::TreePop();
		}
		ImGui::PopID();
		++activeGameobjectsThisFrame;
	}
	if (activeGameObjects != activeGameobjectsThisFrame) {
		activeGameObjects = activeGameobjectsThisFrame;
	}
	ImGui::End();
}
