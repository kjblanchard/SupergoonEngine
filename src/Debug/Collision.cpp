#include <Supergoon/Imgui/imgui.h>
#include <Supergoon/Tweening/tween.h>
#include <Supergoon/log.h>
#include <SupergoonEngine/tween.h>
#include <stdio.h>

#include <Supergoon/Debug/Collision.hpp>
#include <Supergoon/Debug/Debug.hpp>
#include <Supergoon/Debug/Tween.hpp>
#include <string>
// extern int _showingSolidBoxes;
void ShowCollisionDebugWindow(void) {
	// auto window_flags = GetDefaultWindowFlags();
	// bool p_open;
	// if (!ImGui::Begin("Collision", &p_open, window_flags)) {
	// 	ImGui::End();
	// 	return;
	// }
	// ImGui::Text("CurrentTweens %d : %d", _numberTweens, MAX_TWEENS);
	// for (size_t i = 0; i < (size_t)_numberTweens; i++) {
	// 	if (_tweens[i].Available) {
	// 		continue;
	// 	}
	// 	sgTween* tween = &_tweens[i];
	// 	ImGui::BeginDisabled(true);
	// 	auto startedString = "Started ##" + to_string(i);
	// 	ImGui::Checkbox(startedString.c_str(), (bool*)&tween->Started);
	// 	ImGui::EndDisabled();

	// 	double percent = tween->CurrentTimeSeconds / tween->TotalTimeSeconds;
	// 	char buf[32];
	// 	snprintf(buf, 32, "%.1fsec/%.1fsec ##%ld", (tween->CurrentTimeSeconds), tween->TotalTimeSeconds, i);
	// 	ImGui::ProgressBar(percent, ImVec2(0.f, 0.f), buf);
	// }

	// ImGui::End();
}
