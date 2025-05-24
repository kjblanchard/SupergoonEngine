#include <SupergoonEngine/graphics.h>

#include <Supergoon/Debug/Debug.hpp>
#include <Supergoon/Debug/Graphics.hpp>
#include <Supergoon/pch.hpp>

void ShowGraphicsDebugWindow(void) {
	auto window_flags = GetDefaultWindowFlags();
	bool p_open;
	if (!ImGui::Begin("Graphics", &p_open, window_flags)) {
		ImGui::End();
		return;
	}
	ImGui::Text("Num Loaded textures %lu", _numTexturesInCache);
	ImGui::Text("Next Hole: %lu", _firstCacheHole);
	for (size_t i = 0; i < _numTexturesInCache; i++) {
		TextureCacheItem* textureCache = &_textureCache[i];
		ImGui::Text("Name: %s Num References: %d", textureCache->name, textureCache->References);
	}

	ImGui::End();
}
