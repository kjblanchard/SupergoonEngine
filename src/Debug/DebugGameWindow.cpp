#include <Supergoon/Graphics/graphics.h>
#include <Supergoon/Graphics/texture.h>
#include <Supergoon/Input/mouse.h>
#include <imgui.h>

#include <DebugGameWindow.hpp>

#include <algorithm>
#include <cmath>

extern "C" {
extern Texture* _imGUIScreenRenderTargetTexture;
extern int _logicalX;
extern int _logicalY;
}

void DebugGameWindowDraw() {
	if (!_imGUIScreenRenderTargetTexture || _logicalX <= 0 || _logicalY <= 0) return;
	ImVec2 avail = ImGui::GetContentRegionAvail();
	if (avail.x <= 0 || avail.y <= 0) return;
	// Work in physical pixels so one RT texel maps to one screen pixel on
	// high-DPI displays, where an imgui unit is DisplayFramebufferScale pixels.
	ImVec2 fbScale = ImGui::GetIO().DisplayFramebufferScale;
	float availPxW = avail.x * fbScale.x;
	float availPxH = avail.y * fbScale.y;
	// Integer game scale only; applied next DrawStart, which also re-renders text.
	int gameScale = std::min((int)availPxW / _logicalX, (int)availPxH / _logicalY);
	GraphicsRequestGameScale(std::max(gameScale, 1));

	int rtW = TextureGetWidth(_imGUIScreenRenderTargetTexture);
	int rtH = TextureGetHeight(_imGUIScreenRenderTargetTexture);
	// Normally 1 (pixel exact).  Only shrinks when the window is smaller than 1x,
	// or for the single frame before a smaller requested scale is applied.
	float fit = std::min({1.0f, availPxW / (float)rtW, availPxH / (float)rtH});
	ImVec2 size(rtW * fit / fbScale.x, rtH * fit / fbScale.y);
	ImVec2 cursor = ImGui::GetCursorPos();
	ImGui::SetCursorPos(ImVec2(cursor.x + std::floor((avail.x - size.x) / 2.0f),
							   cursor.y + std::floor((avail.y - size.y) / 2.0f)));
	auto tex = (void*)TextureGetID(_imGUIScreenRenderTargetTexture);
	if (!tex) return;
	// SDL render targets are top-down, same as the window, so no V flip.
	ImVec2 imagePos = ImGui::GetCursorPos();
	ImGui::Image(tex, size, ImVec2(0, 0), ImVec2(1, 1));
	// An Image doesn't take clicks, so imgui would start dragging the window and
	// report the image as not hovered.  This button over it owns the clicks instead.
	ImGui::SetCursorPos(imagePos);
	ImGui::InvisibleButton("##gameview", size,
						   ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | ImGuiButtonFlags_MouseButtonMiddle);
	// imgui units are SDL window points, same as SDL_GetMouseState
	ImVec2 imageMin = ImGui::GetItemRectMin();
	ImVec2 imageSize = ImGui::GetItemRectSize();
	MouseSetGameViewRect(imageMin.x, imageMin.y, imageSize.x, imageSize.y,
						 ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem));
}
