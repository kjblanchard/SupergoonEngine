#include <Supergoon/map.h>
#include <imgui.h>

#include <DebugMapTab.hpp>

using namespace std;

extern "C" {
extern Tilemap* currentMap;
extern Tileset** tilesets;
extern size_t tilesetCount;
}

void DebugMapTabDraw() {
	if (ImGui::CollapsingHeader("Map")) {
		ImGui::Text("Current map is %s", currentMap ? currentMap->BaseFilename : "No map");
		ImGui::Text("Num CachedTilesets: %d", tilesetCount);
		if (ImGui::CollapsingHeader("Loaded Tilesets")) {
			for (auto i = 0; i < tilesetCount; ++i) {
				auto tileset = tilesets[i];
				ImGui::Text("%s", tileset->Name);
			}
		}
	}
}
