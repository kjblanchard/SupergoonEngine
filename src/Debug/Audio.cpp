#include <SDL3/SDL.h>
#include <Supergoon/Audio/Audio.h>
#include <Supergoon/Audio/Bgm.h>

#include <Supergoon/Debug/Audio.hpp>
#include <regex>
#include <string>
#include <vector>

using namespace std;
#ifndef MAX_SLOTS
#define MAX_SLOTS 4
#endif

extern float _globalBgmVolume;
typedef struct AudioBgmAsset {
	sgBgm* Bgm;
	float Volume;
} AudioBgmAsset;
extern AudioBgmAsset _bgmAssets[MAX_SLOTS];

std::vector<string> bgmNames;
std::vector<string> sfxNames;
// static float sfxPlayVolume = 1.0f;
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
	SDL_asprintf(&fullPath, "%sassets/audio/%s/", SDL_GetBasePath(), dir);
	char** sfxFiles = SDL_GlobDirectory(fullPath, pattern, 0, &count);
	for (size_t i = 0; i < (size_t)count; i++) {
		sfxNames.push_back(std::string(sfxFiles[i]));
	}
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
	// ImGui::SliderFloat("Global Sfx Volume", &Sound::Instance()->_globalSfxVolume, 0.0f, 1.0f);
	if (ImGui::CollapsingHeader("Bgm")) {
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
			ImGui::SameLine();
			HelpMarker("Click on a song to play, and then click play to try it out.");
			if (ImGui::Button("PlayBgm")) {
				auto song = bgmNames[item_current];
				std::regex dotRegex("\\.ogg");
				std::vector<std::string> result(std::sregex_token_iterator(song.begin(), song.end(), dotRegex, -1), std::sregex_token_iterator());
				SetBgmSlot(0);
				LoadBgm(result[0].c_str(), 1.0, 1);
				PlayBgm();
			}
			// ImGui::SameLine();
			// if (ImGui::Button("Pause")) {
			// 	sound->PauseBgm();
			// }
			// ImGui::SameLine();
			// if (ImGui::Button("Stop")) {
			// 	sound->StopBgm();
			// }
			// ImGui::SameLine();
			// if (ImGui::Button("Stop Fadeout")) {
			// 	sound->StopBgmFadeout();
			// }
			if (ImGui::SliderFloat("Bgm Playing Volume", &_bgmAssets[item_current].Volume, 0, 1.0)) {
				UpdatePlayingBgmVolume();
			}
			ImGui::SameLine();
			HelpMarker("Updates the playing BGM sound only, this is multiplied by the global to get the final sound volume");
		}
	}
	// if (ImGui::CollapsingHeader("Sfx")) {
	// 	if (sfxNames.size() > 0) {
	// 		static int item_current = 1;
	// 		// Create a vector to hold the const char* pointers
	// 		std::vector<const char*> cStrings;
	// 		// Reserve space for performance (optional but good practice)
	// 		cStrings.reserve(sfxNames.size());
	// 		// Extract the const char* pointers from std::string
	// 		for (const std::string& name : sfxNames) {
	// 			cStrings.push_back(name.c_str());
	// 		}
	// 		ImGui::ListBox("SfxItemList", &item_current, cStrings.data(), cStrings.size(), 4);
	// 		ImGui::SameLine();
	// 		HelpMarker("Click on a sound to play, and then click play to try it out.");
	// 		if (ImGui::Button("PlaySfx")) {
	// 			auto song = sfxNames[item_current];
	// 			std::regex dotRegex("\\.ogg");
	// 			std::vector<std::string> result(std::sregex_token_iterator(song.begin(), song.end(), dotRegex, -1), std::sregex_token_iterator());
	// 			auto sfx = ContentRegistry::CreateContent<Sfx>(result[0]);
	// 			ContentRegistry::LoadContent(*sfx);
	// 			sound->PlaySfx(sfx.get(), sfxPlayVolume);
	// 		}
	// 		ImGui::SameLine();
	// 		ImGui::SliderFloat("Volume", &sfxPlayVolume, 0, 1.0);
	// 	}
	// }
	if (ImGui::Button("Update Sound Files")) {
		// Gather all files in assets/sound
		GetFiles();
	}
	ImGui::End();
}
