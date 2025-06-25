#include <SDL3/SDL.h>
#include <Supergoon/Audio/Audio.h>
#include <Supergoon/Audio/Bgm.h>

#include <Supergoon/Debug/Audio.hpp>
#include <Supergoon/pch.hpp>
#include <regex>
#include <string>
#include <vector>

using namespace std;
#ifndef MAX_TRACKS
#define MAX_TRACKS 4
#endif

extern float _globalBgmVolume;
extern float _globalSfxVolume;
typedef struct AudioBgmAsset {
	Bgm* BgmPtr;
	float Volume;
} AudioBgmAsset;
extern AudioBgmAsset _bgmAssets[MAX_TRACKS];

std::vector<string> bgmNames;
std::vector<string> sfxNames;
static int bgmTrack = 0;
static int loops = 0;
static float sfxPlayVolume = 1.0f;
static void HelpMarker(const char* desc) {
	ImGui::TextDisabled("(?)");
	if (ImGui::BeginItemTooltip()) {
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

static void GetFiles() {
	char* fullPath = NULL;
	const char* dir = "bgm";
	SDL_asprintf(&fullPath, "%sassets/audio/%s/", SDL_GetBasePath(), dir);
	const char* pattern = "*.ogg";
	int count = 0;
	bgmNames.clear();
	sfxNames.clear();
	char** bgmFiles = SDL_GlobDirectory(fullPath, pattern, 0, &count);
	for (size_t i = 0; i < (size_t)count; i++) {
		bgmNames.push_back(std::string(bgmFiles[i]));
	}

	dir = "sfx";
	SDL_free(fullPath);
	SDL_asprintf(&fullPath, "%sassets/audio/%s/", SDL_GetBasePath(), dir);
	char** sfxFiles = SDL_GlobDirectory(fullPath, pattern, 0, &count);
	for (size_t i = 0; i < (size_t)count; i++) {
		sfxNames.push_back(std::string(sfxFiles[i]));
	}
	SDL_free(fullPath);
	SDL_free(bgmFiles);
	SDL_free(sfxFiles);
}
void InitializeAudioDebugWindow(void) {
	GetFiles();
}

void ShowAudioDebugWindow() {
	static bool p_open = true;
	static bool no_titlebar = false;
	static bool no_scrollbar = false;
	static bool no_menu = false;
	static bool no_move = false;
	static bool no_resize = false;
	static bool no_collapse = false;
	// static bool no_close = false;
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
	// if (no_close) p_open = NULL;  // Don't pass our bool* to Begin

	if (!ImGui::Begin("Sound", &p_open, window_flags)) {
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}
	auto muted = (_globalBgmVolume == 0);
	if (ImGui::Checkbox("Mute", &muted)) {
		if (muted) {
			_globalBgmVolume = 0;
			UpdatePlayingBgmVolume();
		}
	}

	if (ImGui::SliderFloat("Global Bgm Volume", &_globalBgmVolume, 0.0f, 1.0f)) {
		UpdatePlayingBgmVolume();
	}

	if (ImGui::SliderFloat("Global Sfx Volume", &_globalSfxVolume, 0.0f, 1.0f)) {
		UpdatePlayingBgmVolume();
	}
	// ImGui::SliderFloat("Global Sfx Volume", &Sound::Instance()->_globalSfxVolume, 0.0f, 1.0f);
	if (ImGui::CollapsingHeader("Bgm")) {
		for (size_t i = 0; i < MAX_TRACKS; i++) {
			ImGui::Text("Track %ld", i);
			ImGui::SameLine();
			if (_bgmAssets[i].BgmPtr) {
				double pos = getBgmCurrentPosition(_bgmAssets[i].BgmPtr);
				double duration = getBgmDuration(_bgmAssets[i].BgmPtr);
				double percent = pos / duration;
				char buf[32];
				snprintf(buf, 32, "%.1fsec/%.1fsec", (pos), duration);
				ImGui::ProgressBar(percent, ImVec2(0.f, 0.f), buf);
				// ImGui::Text("Loops Remaining %d ##%ld", _bgmAssets[i].Bgm->Loops, i);
				std::string s = _bgmAssets[i].BgmPtr->Filename;
				std::string last_element(s.substr(s.rfind("/") + 1));
				ImGui::Text("Name: %s", last_element.c_str());
				ImGui::Text("Loops Remaining %d", _bgmAssets[i].BgmPtr->Loops);
				auto volumeStr = "Volume ##" + to_string(i);
				if (ImGui::SliderFloat(volumeStr.c_str(), &_bgmAssets[i].Volume, 0, 1.0)) {
					UpdatePlayingBgmVolume();
				}
				ImGui::SameLine();
				HelpMarker("Updates the playing BGM sound only, this is multiplied by the global to get the final sound volume");
			} else {
				ImGui::ProgressBar(0, ImVec2(0.f, 0.f), "0/0sec");
			}
		}

		if (bgmNames.size() > 0) {
			static int item_current = 1;
			// Create a vector to hold the const char* pointers
			std::vector<const char*> cStrings;
			// Reserve space for performance (optional but good practice)
			cStrings.reserve(bgmNames.size());
			// Extract the const char* pointers from std::string
			for (const std::string& name : bgmNames) {
				cStrings.push_back(name.c_str());
			}
			ImGui::ListBox("BgmItemList", &item_current, cStrings.data(), cStrings.size(), 4);
			if (ImGui::Button("LoadBgm")) {
				auto song = bgmNames[item_current];
				std::regex dotRegex("\\.ogg");
				std::vector<std::string> result(std::sregex_token_iterator(song.begin(), song.end(), dotRegex, -1), std::sregex_token_iterator());
				SetBgmTrack(bgmTrack);
				LoadBgm(result[0].c_str(), 1.0, loops);
			}
			ImGui::SameLine();
			if (ImGui::Button("PlayBgm")) {
				// auto song = bgmNames[item_current];
				// std::regex dotRegex("\\.ogg");
				// std::vector<std::string> result(std::sregex_token_iterator(song.begin(), song.end(), dotRegex, -1), std::sregex_token_iterator());
				SetBgmTrack(bgmTrack);
				PlayBgm();
			}
			ImGui::SameLine();
			if (ImGui::Button("Pause")) {
				SetBgmTrack(bgmTrack);
				PauseBgm();
			}
			ImGui::SameLine();
			if (ImGui::Button("Stop")) {
				SetBgmTrack(bgmTrack);
				StopBgm();
			}
			ImGui::SameLine();
			if (ImGui::Button("Stop Fadeout")) {
				SetBgmTrack(bgmTrack);
				StopBgmFadeout();
			}
			if (ImGui::InputInt("Track", &bgmTrack)) {
			}
			ImGui::SameLine();
			HelpMarker("The stream to load into");
			if (ImGui::InputInt("Loops", &loops)) {
			}
			ImGui::SameLine();
			HelpMarker("How many times to loop at the loop points.  Loop points are loaded into the .ogg file.  -1 will loop forever");
		}
	}
	if (ImGui::CollapsingHeader("Sfx")) {
		if (sfxNames.size() > 0) {
			static int item_current = 1;
			// Create a vector to hold the const char* pointers
			std::vector<const char*> cStrings;
			// Reserve space for performance (optional but good practice)
			cStrings.reserve(sfxNames.size());
			// Extract the const char* pointers from std::string
			for (const std::string& name : sfxNames) {
				cStrings.push_back(name.c_str());
			}
			ImGui::ListBox("SfxItemList", &item_current, cStrings.data(), cStrings.size(), 4);
			ImGui::SameLine();
			HelpMarker("Click on a sound to play, and then click play to try it out.");
			if (ImGui::Button("PlaySfx")) {
				auto song = sfxNames[item_current];
				std::regex dotRegex("\\.ogg");
				std::vector<std::string> result(std::sregex_token_iterator(song.begin(), song.end(), dotRegex, -1), std::sregex_token_iterator());
				PlaySfxOneShot(result[0].c_str(), sfxPlayVolume);
			}
			ImGui::SameLine();
			ImGui::SliderFloat("Volume", &sfxPlayVolume, 0, 1.0);
		}
	}
	if (ImGui::Button("Update Sound Files")) {
		// Gather all files in assets/sound
		GetFiles();
	}
	ImGui::End();
}

void CloseAudioDebugWindow(void) {
}
