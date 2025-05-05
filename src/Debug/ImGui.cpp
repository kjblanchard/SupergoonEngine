#include <SDL3/SDL.h>
#include <Supergoon/Imgui/imgui.h>
#include <Supergoon/Imgui/imgui_impl_sdl3.h>
#include <Supergoon/Imgui/imgui_impl_sdlrenderer3.h>
#include <Supergoon/events.h>
#include <Supergoon/window.h>

#include <Supergoon/Debug/Debug.hpp>
#include <Supergoon/Debug/ImGui.hpp>
#include <string>

// defined in window.c
extern Window* _window;
extern Renderer* _renderer;
// defined in Game.cpp
extern bool _isFocusedLastFrame;

void InitializeImGui(void) {
	assert(_renderer && _window && "No renderer, make sure window is created");
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // IF using Docking Branch
	// static auto thing = std::string(SDL_GetPrefPath("Supergoon Games", "EscapeTheFate")) + "debug.ini";
	char* prefPath = SDL_GetPrefPath("Supergoon Games", "EscapeTheFate");
	static auto thing = std::string(prefPath) + "debug.ini";
	io.IniFilename = thing.c_str();
	SDL_free(prefPath);
	ImGui_ImplSDL3_InitForSDLRenderer(_window, _renderer);
	ImGui_ImplSDLRenderer3_Init(_renderer);
	InitializeDebug();
}

bool HandleImGuiEvent(Event* event) {
	ImGui_ImplSDL3_ProcessEvent(event);
	// return _isFocusedLastFrame; //TODO this was used to try and only focus the game, however it broke the audio events..
	return true;
}

void StartImGuiFrame(void) {
	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();
}

void DrawImGui(void) {
	assert(_renderer && "No renderer, make sure window is created");
	// Draw imgui to the entire screen.
	SDL_SetRenderTarget(_renderer, NULL);
	ShowWidgets();
	ImGui::ShowDemoWindow();
	ImGui::Render();
	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), _renderer);
}

void ShutdownImGui(void) {
	ImGui_ImplSDLRenderer3_Shutdown();	// Shut down SDL Renderer binding
	ImGui_ImplSDL3_Shutdown();			// Shut down SDL binding
	ImGui::DestroyContext();
}
