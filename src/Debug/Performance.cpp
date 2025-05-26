#include <SDL3/SDL.h>
#include <Supergoon/state.h>
#include <SupergoonEngine/window.h>

#include <Supergoon/Debug/Debug.hpp>
#include <Supergoon/pch.hpp>

void ShowPerformanceWindow(void) {
	constexpr int FRAME_HISTORY_COUNT = 100;
	static float frameTimes[FRAME_HISTORY_COUNT] = {0};
	static float fpsValues[FRAME_HISTORY_COUNT] = {0};
	static int frameIndex = 0;
	static int currentVsync = 0;
	// SDL_GetRenderVSync(_renderer, &currentVsync);
	// bool vSyncEnabled = isVsync ? true : false;

	// Update values
	float deltaMs = DeltaTimeSeconds * 1000.0f;
	if (deltaMs < 1.0f || deltaMs > 100.0f) {
		deltaMs = frameIndex > 0 ? frameTimes[(frameIndex - 1 + FRAME_HISTORY_COUNT) % FRAME_HISTORY_COUNT] : 8.33f;
	}
	float fps = 1.0f / DeltaTimeSeconds;

	frameTimes[frameIndex] = deltaMs;
	fpsValues[frameIndex] = fps;
	frameIndex = (frameIndex + 1) % FRAME_HISTORY_COUNT;

	// Stats calc
	float minDelta = FLT_MAX, maxDelta = 0.0f, sumDelta = 0.0f;
	float minFps = FLT_MAX, maxFps = 0.0f, sumFps = 0.0f;

	for (int i = 0; i < FRAME_HISTORY_COUNT; ++i) {
		float d = frameTimes[i];
		float f = fpsValues[i];
		sumDelta += d;
		sumFps += f;
		if (d < minDelta) minDelta = d;
		if (d > maxDelta) maxDelta = d;
		if (f < minFps) minFps = f;
		if (f > maxFps) maxFps = f;
	}

	float avgDelta = sumDelta / FRAME_HISTORY_COUNT;
	float avgFps = sumFps / FRAME_HISTORY_COUNT;

	// Display
	ImGui::Begin("Performance");
	ImGui::Text("Delta Time (ms) : Min: %.2f | Max: %.2f | Avg: %.2f", minDelta, maxDelta, avgDelta);
	ImGui::Text("FPS             : Min: %.1f | Max: %.1f | Avg: %.1f", minFps, maxFps, avgFps);
	ImGui::Text("VSync Modes:");
	if (ImGui::RadioButton("VSync OFF", currentVsync == 0)) {
		currentVsync = 0;
		SDL_SetRenderVSync(_renderer, 0);  // 0 disables vsync
	}
	if (ImGui::RadioButton("VSync Type -1 (Adaptive)", currentVsync == 1)) {
		currentVsync = 1;
		SDL_SetRenderVSync(_renderer, -1);
	}
	if (ImGui::RadioButton("VSync Type 1", currentVsync == 2)) {
		currentVsync = 2;
		SDL_SetRenderVSync(_renderer, 1);
	}
	if (ImGui::RadioButton("VSync Type 2", currentVsync == 3)) {
		currentVsync = 3;
		SDL_SetRenderVSync(_renderer, 2);
	}
	static int frameCapChoice = 1;

	// Frame cap radio buttons
	ImGui::Text("Frame Cap:");
	if (ImGui::RadioButton("30 FPS", frameCapChoice == 0)) {
		TARGET_FPS = 30;
		frameCapChoice = 0;
	}
	if (ImGui::RadioButton("60 FPS", frameCapChoice == 1)) {
		TARGET_FPS = 60;
		frameCapChoice = 1;
	}
	if (ImGui::RadioButton("120 FPS", frameCapChoice == 2)) {
		TARGET_FPS = 120;
		frameCapChoice = 2;
	}

	if (ImGui::RadioButton("144 FPS", frameCapChoice == 3)) {
		TARGET_FPS = 144;
		frameCapChoice = 3;
	}
	if (ImGui::RadioButton("Unlimited", frameCapChoice == 4)) {
		TARGET_FPS = 999;
		frameCapChoice = 4;
	}
	ImGui::End();
}
