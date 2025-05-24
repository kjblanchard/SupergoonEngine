#include <Supergoon/state.h>

#include <Supergoon/Debug/Debug.hpp>
#include <Supergoon/pch.hpp>

// void ShowPerformanceWindow(void) {
// 	auto window_flags = GetDefaultWindowFlags();
// 	bool p_open;
// 	// In your update loop or clock update:
// #define SMOOTHING_FACTOR 0.1f
// 	static float avgDeltaTime = 0.0f;
// 	avgDeltaTime = (1.0f - SMOOTHING_FACTOR) * avgDeltaTime + SMOOTHING_FACTOR * DeltaTimeSeconds;

// 	if (!ImGui::Begin("Performance", &p_open, window_flags)) {
// 		ImGui::End();
// 		return;
// 	}
// #define FRAME_HISTORY_COUNT 100
// 	static float frameTimes[FRAME_HISTORY_COUNT] = {0};
// 	static int frameIndex = 0;

// 	frameTimes[frameIndex] = DeltaTimeSeconds * 1000.0f;  // Store in ms
// 	frameIndex = (frameIndex + 1) % FRAME_HISTORY_COUNT;

// 	ImGui::Text("DeltaTime Seconds %f", DeltaTimeSeconds);
// 	ImGui::Text("DeltaTime MS %f", DeltaTimeMilliseconds);
// 	ImGui::Text("DeltaTime Seconds: %.6f (avg: %.6f)", DeltaTimeSeconds, avgDeltaTime);
// 	ImGui::Text("FPS: %.1f", 1.0f / avgDeltaTime);
// 	ImGui::PlotLines("DeltaTime (ms)", frameTimes, FRAME_HISTORY_COUNT, frameIndex, nullptr, 0.0f, 20.0f, ImVec2(0, 80));

// 	ImGui::End();
// }
// #include <algorithm>  // for std::min_element, std::max_element, std::clamp
// float Clamp(float val, float min, float max) {
// 	return (val < min) ? min : (val > max) ? max
// 										   : val;
// }

// void ShowPerformanceWindow(void) {
// 	constexpr float TARGET_FRAME_MS = 8.33f;
// 	constexpr int FRAME_HISTORY_COUNT = 100;
// 	constexpr float PLOT_MIN = 7.5f;
// 	constexpr float PLOT_MAX = 9.5f;
// 	constexpr float SMOOTHING_FACTOR = 0.1f;

// 	static float deltaHistory[FRAME_HISTORY_COUNT] = {0};
// 	static float fpsHistory[FRAME_HISTORY_COUNT] = {0};
// 	static int frameIndex = 0;
// 	static float smoothedDelta = 0.0f;

// 	// Update data
// 	float deltaMs = DeltaTimeSeconds * 1000.0f;
// 	float clampedDelta = Clamp(deltaMs, PLOT_MIN, PLOT_MAX);
// 	smoothedDelta = (1.0f - SMOOTHING_FACTOR) * smoothedDelta + SMOOTHING_FACTOR * DeltaTimeSeconds;

// 	deltaHistory[frameIndex] = clampedDelta;
// 	fpsHistory[frameIndex] = 1.0f / DeltaTimeSeconds;

// 	// Advance frame index
// 	frameIndex = (frameIndex + 1) % FRAME_HISTORY_COUNT;

// 	// Compute min/max/avg
// 	float sum = 0.0f, min = FLT_MAX, max = 0.0f;
// 	for (int i = 0; i < FRAME_HISTORY_COUNT; ++i) {
// 		float val = deltaHistory[i];
// 		sum += val;
// 		if (val < min) min = val;
// 		if (val > max) max = val;
// 	}
// 	float avg = sum / FRAME_HISTORY_COUNT;

// 	// Begin ImGui window
// 	auto window_flags = ImGuiWindowFlags_AlwaysAutoResize;
// 	bool p_open = true;
// 	if (!ImGui::Begin("Performance", &p_open, window_flags)) {
// 		ImGui::End();
// 		return;
// 	}

// 	// // Display text info
// 	// ImGui::Text("DeltaTime Seconds: %.6f", DeltaTimeSeconds);
// 	// ImGui::Text("DeltaTime MS     : %.3f", deltaMs);
// 	// ImGui::Text("Smoothed FPS     : %.1f", 1.0f / smoothedDelta);
// 	// ImGui::Text("Delta (ms)       - Min: %.2f | Max: %.2f | Avg: %.2f", min, max, avg);

// 	// // Plot delta time
// 	// ImVec2 plotSize = ImVec2(0, 80);
// 	// ImVec2 plotPos = ImGui::GetCursorScreenPos();
// 	// ImGui::PlotLines("Frame Time (ms)", deltaHistory, FRAME_HISTORY_COUNT, frameIndex, nullptr, PLOT_MIN, PLOT_MAX, plotSize);

// 	// // Draw target line at 8.33ms (120 FPS)
// 	// ImDrawList* drawList = ImGui::GetWindowDrawList();
// 	// float norm = (TARGET_FRAME_MS - PLOT_MIN) / (PLOT_MAX - PLOT_MIN);
// 	// float y = plotPos.y + plotSize.y * (1.0f - norm);
// 	// drawList->AddLine(ImVec2(plotPos.x, y), ImVec2(plotPos.x + plotSize.x, y), IM_COL32(255, 0, 0, 255), 1.0f);
// 	// drawList->AddText(ImVec2(plotPos.x + 4, y - 14), IM_COL32(255, 0, 0, 255), "120 FPS");

// 	// FPS histogram
// 	// ImGui::PlotHistogram("FPS", fpsHistory, FRAME_HISTORY_COUNT, frameIndex, nullptr, 0.0f, 144.0f, plotSize);

// 	ImGui::Text("DeltaTime Seconds: %.6f", DeltaTimeSeconds);
// 	ImGui::Text("DeltaTime MS     : %.3f", deltaMs);
// 	ImGui::Text("FPS              : %.1f", 1.0f / DeltaTimeSeconds);
// 	ImGui::Text("Delta (ms)       - Min: %.2f | Max: %.2f | Avg: %.2f", min, max, avg);
// 	ImGui::PlotHistogram("FPS", fpsHistory, FRAME_HISTORY_COUNT, frameIndex, nullptr, 0.0f, 144.0f, ImVec2(0, 80));

// 	ImGui::End();
// }
// void ShowPerformanceWindow(void) {
// 	constexpr int FRAME_HISTORY_COUNT = 100;
// 	static float fpsHistory[FRAME_HISTORY_COUNT] = {0};
// 	static int frameIndex = 0;

// 	// Update FPS history
// 	float fps = 1.0f / DeltaTimeSeconds;
// 	fpsHistory[frameIndex] = fps;
// 	frameIndex = (frameIndex + 1) % FRAME_HISTORY_COUNT;

// 	// Calculate stats
// 	float min = FLT_MAX, max = 0.0f, sum = 0.0f;
// 	for (int i = 0; i < FRAME_HISTORY_COUNT; ++i) {
// 		float val = fpsHistory[i];
// 		sum += val;
// 		if (val < min) min = val;
// 		if (val > max) max = val;
// 	}
// 	float avg = sum / FRAME_HISTORY_COUNT;

// 	// Show ImGui window
// 	ImGui::Begin("Performance");

// 	ImGui::Text("FPS: (Min: %.1f | Max: %.1f | Avg: %.1f)", min, max, avg);
// 	ImGui::Text("Delta (ms)       - Min: %.2f | Max: %.2f | Avg: %.2f", min, max, avg);
// 	ImGui::PlotHistogram("FPS History", fpsHistory, FRAME_HISTORY_COUNT, frameIndex, nullptr, 0.0f, 144.0f, ImVec2(0, 80));

// 	ImGui::End();
// }

void ShowPerformanceWindow(void) {
	constexpr int FRAME_HISTORY_COUNT = 100;
	static float frameTimes[FRAME_HISTORY_COUNT] = {0};
	static float fpsValues[FRAME_HISTORY_COUNT] = {0};
	static int frameIndex = 0;

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

	ImGui::End();
}
