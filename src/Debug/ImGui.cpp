#include <SDL3/SDL.h>
#include <Supergoon/Imgui/imgui.h>
#include <Supergoon/Imgui/imgui_impl_sdl3.h>
#include <Supergoon/Imgui/imgui_impl_sdlrenderer3.h>
#include <Supergoon/window.h>

#include <Supergoon/Debug/Debug.hpp>
#include <Supergoon/Debug/ImGui.hpp>
#include <string>

void InitializeImGui(void) {
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // IF using Docking Branch
	static auto thing = std::string(SDL_GetPrefPath("Supergoon Games", "EscapeTheFate")) + "debug.ini";
	io.IniFilename = thing.c_str();
	ImGui_ImplSDL3_InitForSDLRenderer(GetGameWindow(), GetGameRenderer());
	ImGui_ImplSDLRenderer3_Init(GetGameRenderer());
}
void HandleImGuiEvent(Event* event) {
	ImGui_ImplSDL3_ProcessEvent(event);
}

void StartImGuiFrame(void) {
	// ClearRenderTargetTexture(_imguiGameTexture);
	// Draw everything to this target instead when building with imgui.
	// SDL_SetRenderTarget(_renderer, _imguiGameTexture);
	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();
}

void DrawImGui(void) {
	SDL_SetRenderTarget(GetGameRenderer(), NULL);
	ShowWidgets();
	ImGui::ShowDemoWindow();
	ImGui::Render();
	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), GetGameRenderer());
}
