#include <Supergoon/Graphics/graphics.h>
#include <Supergoon/Graphics/texture.h>
#include <Supergoon/Platform/opengl/openglTexture.h>
#include <imgui.h>

#include <DebugGameWindow.hpp>

using namespace std;

extern "C" {
extern Texture* _imGUIScreenRenderTargetTexture;
extern int _logicalX;
extern int _logicalY;
}

void DebugGameWindowDraw() {
	ImVec2 imguiWindowSize = ImGui::GetContentRegionAvail();
	int scaleX = imguiWindowSize.x / _logicalX;
	int scaleY = imguiWindowSize.y / _logicalY;
	int scale = (scaleX < scaleY) ? scaleX : scaleY;
	if (scale < 1) scale = 1;
	int scaledWidth = _logicalX * scale;
	int scaledHeight = _logicalY * scale;
	auto tex = (void*)TextureGetID(_imGUIScreenRenderTargetTexture);
	if (!tex) return;
	ImGui::Image(tex, ImVec2(scaledWidth, scaledHeight), ImVec2(0, 1), ImVec2(1, 0));
}
