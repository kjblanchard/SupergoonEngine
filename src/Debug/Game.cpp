#include <Supergoon/Imgui/imgui.h>
#include <Supergoon/Imgui/imgui_impl_sdl3.h>
#include <Supergoon/Imgui/imgui_impl_sdlrenderer3.h>
#include <Supergoon/window.h>

#include <Supergoon/Debug/Debug.hpp>
#include <Supergoon/Debug/Game.hpp>
// #include <Supergoon/pch.hpp>
// #include <Supergoon/Events.hpp>
// #include <Supergoon/Graphics/Graphics.hpp>
// #include <Supergoon/Widgets/Game.hpp>
// #include <Supergoon/Widgets/Widgets.hpp>
// namespace Supergoon {
extern int _logicalWidth;
extern int _logicalHeight;
Texture* _imguiGameTexture;
bool _isFocusedLastFrame = true;
static u_int8_t _frames = 0;

void ShowGameDebugWindow(void) {
	if (_frames < 2) {
		++_frames;
	} else if (_frames == 2) {
		ImGui::SetNextWindowFocus();
		++_frames;
	}
	auto window_flags = GetDefaultWindowFlags();
	bool p_open;
	if (!ImGui::Begin("Game", &p_open, window_flags)) {
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}
	ImVec2 imguiWindowSize = ImGui::GetContentRegionAvail();
	int scaleX = imguiWindowSize.x / _logicalWidth;	  // Integer division for X axis
	int scaleY = imguiWindowSize.y / _logicalHeight;  // Integer division for Y axis

	// Choose the smaller scale factor to maintain aspect ratio
	int scale = (scaleX < scaleY) ? scaleX : scaleY;

	// Step 2: Calculate the scaled size (what SDL would render to the screen)
	int scaledWidth = _logicalWidth * scale;
	int scaledHeight = _logicalHeight * scale;

	// Step 3: Render the SDL_Texture in ImGui with the same scaling
	ImVec2 imguiSize(scaledWidth, scaledHeight);  // Use the scaled size
	auto tex = (ImTextureID)(intptr_t)_imguiGameTexture;
	ImGui::Image(tex, imguiSize);
	_isFocusedLastFrame = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootWindow);
	ImGui::End();
}
