
#include <Supergoon/Debug/Audio.hpp>
#include <Supergoon/Debug/Console.hpp>
#include <Supergoon/Debug/Debug.hpp>
#include <Supergoon/Debug/Game.hpp>
#include <Supergoon/Debug/Graphics.hpp>
#include <Supergoon/Debug/Tween.hpp>
#include <Supergoon/Debug/UI.hpp>
#include <Supergoon/pch.hpp>

void ShowWidgets() {
	ShowContentDebugWindow();
	ShowAudioDebugWindow();
	ShowTweenDebugWindow();
	ShowGameDebugWindow();
	ShowUIDebugWindow();
	ShowGraphicsDebugWindow();
}

int GetDefaultWindowFlags() {
	static bool no_titlebar = false;
	static bool no_scrollbar = false;
	static bool no_menu = false;
	static bool no_move = false;
	static bool no_resize = false;
	static bool no_collapse = false;
	static bool no_nav = false;
	static bool no_background = false;
	static bool no_bring_to_front = false;
	static bool no_docking = false;
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
	if (no_docking) window_flags |= ImGuiWindowFlags_NoDocking;
	if (unsaved_document) window_flags |= ImGuiWindowFlags_UnsavedDocument;
	return window_flags;
}

void InitializeDebug() {
	InitializeContentDebugWindow();
	InitializeAudioDebugWindow();
}
