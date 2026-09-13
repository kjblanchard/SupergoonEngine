#include <Supergoon/camera.h>
#include <imgui.h>

#include <debug/DebugCamera.hpp>
using namespace Etf;

void Etf::DisplayCameraTab() {
#ifdef imgui
	if (ImGui::CollapsingHeader("Camera")) {
		static float* camPosX = nullptr;
		static float* camPosY = nullptr;
		CameraGetPositionHandle(&camPosX, &camPosY);
		ImGui::SliderFloat("CameraX", camPosX, 0, 1000);
		ImGui::SliderFloat("CameraY", camPosY, 0, 1000);
		static float* previousCamFollowX = nullptr;
		static float* previousCamFollowY = nullptr;
		static float* camFollowX = nullptr;
		static float* camFollowY = nullptr;
		CameraGetFollow(&camFollowX, &camFollowY);
		auto cameraFollow = camFollowX && camFollowY;
		if (ImGui::Checkbox("IsFollowing", &cameraFollow)) {
			if (cameraFollow) {
				SetCameraFollowTarget(previousCamFollowX, previousCamFollowY);
			} else {
				previousCamFollowX = camFollowX;
				previousCamFollowY = camFollowY;
				ResetCameraFollow();
			}
		}
	}
#else
	return;
#endif
}
