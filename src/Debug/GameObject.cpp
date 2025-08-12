#include <Supergoon/Imgui/imgui.h>
#include <Supergoon/gameobject.h>
#include <Supergoon/log.h>
#include <SupergoonEngine/gameobject.h>

#include <Supergoon/Debug/Debug.hpp>
#include <string>
using namespace std;
// extern int _showingSolidBoxes;
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
	for (size_t i = 0; i < _numGameObjects; i++) {
		GameObject* go = _gameObjects[i];
		if ((go->Flags & GameObjectFlagDestroyed) || (go->Flags == 0)) {
			continue;
		}
		auto idStr = to_string(go->Id);
		if (ImGui::TreeNode(idStr.c_str())) {
			ImGui::Text("X: %f", go->X);
			ImGui::Text("Y: %f", go->Y);
			// auto xLabel = "X: ##" + idStr;
			// auto yLabel = "Y: ##" + idStr;
			ImGui::TreePop();
		}
		++activeGameobjectsThisFrame;
	}
	if (activeGameObjects != activeGameobjectsThisFrame) {
		activeGameObjects = activeGameobjectsThisFrame;
	}
	ImGui::End();
}
