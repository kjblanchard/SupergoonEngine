#include <SDL3/SDL_events.h>
#include <Supergoon/Graphics/graphics.h>
#include <Supergoon/Input/mouse.h>
#include <Supergoon/Platform/sdl/sdlWindow.h>
#include <Supergoon/window.h>
#include <imconfig.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#ifdef sdlrendererbackend
#include <SDL3/SDL_render.h>

#include <imgui_impl_sdlrenderer3.h>
#else
#include <imgui_impl_opengl3.h>
#endif

#include <DebugConsoleWindow.hpp>
#include <DebugEngineTab.hpp>
#include <DebugGameWindow.hpp>
#include <DebugMapTab.hpp>
#include <DebugUIWindow.hpp>
#include <DebugWindow.hpp>
#include <functional>
#include <string>
#include <vector>

using namespace std;

struct debugWindow {
	string WindowName;
	function<void()> DrawFunction;
	int ExtraFlags;
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
	ImGuiWindowFlags window_flags = windowToDraw.ExtraFlags;
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
	// The game image fills the content region; a scrollbar would shrink it and
	// make the integer game scale flip back and forth.
	DebugWindowAddWindowFuncWithFlags("Game", DebugGameWindowDraw, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	DebugWindowAddWindowFunc("Console", DebugConsoleWindowDraw);
	DebugWindowAddWindowFunc("UI", DebugUIWindowDraw);
	DebugWindowAddTabFuncToMainDebugWindow(DebugEngineTabDraw);
	DebugWindowAddTabFuncToMainDebugWindow(DebugMapTabDraw);
}

void debugWindowDrawWindows() {
	for (auto& window : windowsToDraw_) {
		drawWindowInternal(window);
	}
}

}  // namespace

void DebugSystemRender() {
	ImGui::Render();
#ifdef sdlrendererbackend
	// Caller has already bound the window as the render target at scale 1, which
	// this backend requires: it restores viewport and clip, but not the target.
	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(),
										  static_cast<SDL_Renderer*>(GraphicsGetContextPtr()));
#else
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
}

void DebugSystemInitialize() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsClassic();
#ifdef sdlrendererbackend
	auto renderer = static_cast<SDL_Renderer*>(GraphicsGetContextPtr());
	ImGui_ImplSDL3_InitForSDLRenderer((SDL_Window*)WindowGet()->Handle, renderer);
	ImGui_ImplSDLRenderer3_Init(renderer);
#else
	ImGui_ImplSDL3_InitForOpenGL((SDL_Window*)WindowGet()->Handle, GraphicsGetContextPtr());
	ImGui_ImplOpenGL3_Init();
#endif
	debugWindowInitialize();
}

void DebugSystemDraw() {
#ifdef sdlrendererbackend
	ImGui_ImplSDLRenderer3_NewFrame();
#else
	ImGui_ImplOpenGL3_NewFrame();
#endif
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();
	// Game window re-sets this if it draws, so a closed/collapsed one reads as not hovered
	MouseSetGameViewRect(0, 0, 0, 0, 0);
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
	DebugWindowAddWindowFuncWithFlags(windowName, drawFunc, 0);
}

void DebugWindowAddWindowFuncWithFlags(const char* windowName, DebugWindowDrawFunc drawFunc, int extraFlags) {
	windowsToDraw_.push_back({windowName, drawFunc, extraFlags});
}
