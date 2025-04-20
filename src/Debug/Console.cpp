#include <Supergoon/Primitives/Color.h>
#include <Supergoon/log.h>

#include <Supergoon/Debug/Debug.hpp>
#include <Supergoon/pch.hpp>
#include <deque>

static const int MAX_MESSAGES = 999;
static bool scrollToBottom = false;
static bool autoScroll = true;
static bool showDebugMessages = false;
static bool showInfoMessages = false;
static bool showWarnMessages = true;
static bool showErrorMessages = true;
std::deque<std::pair<int, std::string>> consoleMessages;

static void engineLogFunc(const char *time, const char *message, int logLevel) {
	if (consoleMessages.size() >= MAX_MESSAGES) {
		consoleMessages.pop_front();
	}
	consoleMessages.push_back({logLevel, time + std::string(" ") + std::string(message)});
	scrollToBottom = true;
}

void ShowContentDebugWindow(void) {
	auto window_flags = GetDefaultWindowFlags();
	bool p_open;
	if (!ImGui::Begin("Console", &p_open, window_flags)) {
		ImGui::End();
		return;
	}
	ImGui::Checkbox("Autoscroll", &autoScroll);
	ImGui::SameLine();
	ImGui::Checkbox("Debug", &showDebugMessages);
	ImGui::SameLine();
	ImGui::Checkbox("Info", &showInfoMessages);
	ImGui::SameLine();
	ImGui::Checkbox("Warn", &showWarnMessages);
	ImGui::SameLine();
	ImGui::Checkbox("Error", &showErrorMessages);
	ImGui::SameLine();
	if (ImGui::Button("Clear Logs")) {
		consoleMessages.clear();
	}
	ImGui::SameLine();
	if (ImGui::Button("Test Message")) {
		sgLogDebug("Hello debug test!");
	}
	ImGui::BeginChild("##log", ImVec2(0.0f, 0.0f), ImGuiChildFlags_Borders, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);
	for (auto message : consoleMessages) {
		auto messageLogLevel = message.first;
		if ((messageLogLevel == Log_LDebug && !showDebugMessages) ||
			(messageLogLevel == Log_LInfo && !showInfoMessages) ||
			(messageLogLevel == Log_LWarn && !showWarnMessages) ||
			(messageLogLevel == Log_LError && !showErrorMessages)) {
			continue;
		}

		sgColor color = {255, 255, 255, 255};
		std::string typeText = "Info - ";
		if (message.first == Log_LWarn) {
			color.B = 0;
			typeText = "Warning - ";
		} else if (message.first == Log_LError) {
			typeText = "Error - ";
			color.G = 0;
			color.B = 0;
		} else if (message.first == Log_LDebug) {
			typeText = "Debug - ";
			color.R = color.G = color.B = 150;
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
	ImGui::End();
}

void InitializeContentDebugWindow(void) {
	sgSetDebugFunction(engineLogFunc);
}
