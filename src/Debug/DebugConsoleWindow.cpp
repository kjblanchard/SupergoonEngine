#include <SDL3/SDL_messagebox.h>
#include <Supergoon/Primitives/Color.h>
#include <imgui.h>
#include <sgtools/log.h>

#include <DebugConsoleWindow.hpp>
#include <deque>
#include <string>
using namespace std;

static const int MAX_MESSAGES = 999;
static bool scrollToBottom = false;
static bool autoScroll = true;
std::deque<std::pair<int, std::string>> consoleMessages;

static void engineLogFunc(const char* time, const char* message, int logLevel) {
	if (consoleMessages.size() >= MAX_MESSAGES) {
		consoleMessages.pop_front();
	}
	consoleMessages.push_back({logLevel, time + std::string(" ") + std::string(message)});
	scrollToBottom = true;
	if (logLevel == sgLogLevelCritical) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Critical error", message, NULL);
	}
}

void DebugConsoleWindowInitialize() {
	sgSetDebugFunction(engineLogFunc);
}

void DebugConsoleWindowDraw() {
	const char* logLevels[] = {"Error", "Warn", "Info", "Debug"};
	const int logLevelValues[] = {sgLogLevelError, sgLogLevelWarn, sgLogLevelInfo, sgLogLevelDebug};
	static int currentLogLevel = 1;
	if (ImGui::Combo("Log Level", &currentLogLevel, logLevels, IM_ARRAYSIZE(logLevels))) {
		sgSetLogLevel(logLevelValues[currentLogLevel]);
	}
	ImGui::Checkbox("Autoscroll", &autoScroll);
	ImGui::SameLine();
	if (ImGui::Button("Clear Logs")) {
		consoleMessages.clear();
	}
	ImGui::BeginChild("##log", ImVec2(0.0f, 0.0f), ImGuiChildFlags_Borders, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);
	for (auto message : consoleMessages) {
		Color color = {255, 255, 255, 255};
		std::string typeText = "Info - ";
		if (message.first == sgLogLevelWarn) {
			color.B = 0;
			typeText = "Warning - ";
		} else if (message.first == sgLogLevelError) {
			typeText = "Error - ";
			color.G = 0;
			color.B = 0;
		}
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(color.R, color.G, color.B, 255));
		ImGui::Text("%s %s", typeText.c_str(), message.second.c_str());
		ImGui::PopStyleColor();
	}
	if (autoScroll && scrollToBottom) {
		ImGui::SetScrollHereY(1.0);
	}
	scrollToBottom = false;
	ImGui::EndChild();
}
