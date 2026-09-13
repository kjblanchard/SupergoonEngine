#include <SDL3/SDL_events.h>
#include <Supergoon/Graphics/graphics.h>
#include <Supergoon/Platform/sdl/sdlWindow.h>
#include <Supergoon/window.h>
#include <imconfig.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>

#include <DebugConsoleWindow.hpp>
#include <DebugGameWindow.hpp>
#include <DebugWindow.hpp>
#include <functional>
#include <string>
#include <vector>

using namespace std;

struct debugWindow {
	string WindowName;
	function<void()> DrawFunction;
};

namespace {
vector<function<void()>> mainWindowTabDrawFunctions_;
vector<debugWindow> windowsToDraw_;

static void drawWindowInternal(debugWindow& windowToDraw) {
#ifdef imgui
	static bool p_open = true;
	static bool no_titlebar = false;
	static bool no_scrollbar = false;
	static bool no_menu = false;
	static bool no_move = false;
	static bool no_resize = false;
	static bool no_collapse = false;
	static bool no_nav = false;
	static bool no_background = false;
	static bool no_bring_to_front = false;
	static bool unsaved_document = false;
	ImGuiWindowFlags window_flags = 0;
	if (no_titlebar) window_flags |= ImGuiWindowFlags_NoTitleBar;
	if (no_scrollbar) window_flags |= ImGuiWindowFlags_NoScrollbar;
	if (!no_menu) window_flags |= ImGuiWindowFlags_MenuBar;
	if (no_move) window_flags |= ImGuiWindowFlags_NoMove;
	if (no_resize) window_flags |= ImGuiWindowFlags_NoResize;
	if (no_collapse) window_flags |= ImGuiWindowFlags_NoCollapse;
	if (no_nav) window_flags |= ImGuiWindowFlags_NoNav;
	if (no_background) window_flags |= ImGuiWindowFlags_NoBackground;
	if (no_bring_to_front) window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	if (unsaved_document) window_flags |= ImGuiWindowFlags_UnsavedDocument;
	if (!ImGui::Begin(windowToDraw.WindowName.c_str(), &p_open, window_flags)) {
		ImGui::End();
		return;
	}
	windowToDraw.DrawFunction();
	ImGui::End();
#else
	return;
#endif
}

static void drawMainWindow() {
#ifdef imgui
	for (auto& func : mainWindowTabDrawFunctions_) {
		func();
	}
#else
	return;
#endif
}

void debugWindowInitialize() {
	DebugConsoleWindowInitialize();
	DebugWindowAddWindowFunc("Debugging Tools", drawMainWindow);
	DebugWindowAddWindowFunc("Game", DebugGameWindowDraw);
	DebugWindowAddWindowFunc("Console", DebugConsoleWindowDraw);
}

void debugWindowDrawWindows() {
	for (auto& window : windowsToDraw_) {
		drawWindowInternal(window);
	}
}

}  // namespace

void DebugSystemRender() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DebugSystemInitialize() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsClassic();
	ImGui_ImplSDL3_InitForOpenGL((SDL_Window*)WindowGet()->Handle, GraphicsGetContextPtr());
	ImGui_ImplOpenGL3_Init();
	debugWindowInitialize();
}

void DebugSystemDraw() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();
	ImGui::ShowDemoWindow();
	debugWindowDrawWindows();
}

void DebugSystemHandleEvent(void* e) {
	auto sdlEvent = static_cast<SDL_Event*>(e);
	ImGui_ImplSDL3_ProcessEvent(sdlEvent);
}

void DebugWindowAddTabFuncToMainDebugWindow(DebugWindowDrawFunc drawFunc) {
	mainWindowTabDrawFunctions_.push_back(drawFunc);
}

void DebugWindowAddWindowFunc(const char* windowName, DebugWindowDrawFunc drawFunc) {
	windowsToDraw_.push_back({windowName, drawFunc});
}
