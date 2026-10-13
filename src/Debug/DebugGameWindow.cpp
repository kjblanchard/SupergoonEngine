#include <Supergoon/Graphics/graphics.h>
#include <Supergoon/Graphics/texture.h>
#include <Supergoon/Platform/opengl/openglTexture.h>
#include <imgui.h>

#include <DebugGameWindow.hpp>

#include <cmath>

extern "C" {
extern Texture* _imGUIScreenRenderTargetTexture;
}

void DebugGameWindowDraw() {
	ImVec2 avail = ImGui::GetContentRegionAvail();
	int rtW = TextureGetWidth(_imGUIScreenRenderTargetTexture);
	int rtH = TextureGetHeight(_imGUIScreenRenderTargetTexture);
	float scaleX = avail.x / (float)rtW;
	float scaleY = avail.y / (float)rtH;
	float scale = (scaleX < scaleY) ? scaleX : scaleY;
	if (scale < 0.01f) scale = 0.01f;
	auto tex = (void*)TextureGetID(_imGUIScreenRenderTargetTexture);
	if (!tex) return;
	ImGui::Image(tex, ImVec2(rtW * scale, rtH * scale), ImVec2(0, 1), ImVec2(1, 0));
}
