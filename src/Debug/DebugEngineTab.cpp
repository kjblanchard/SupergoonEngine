#include <Supergoon/map.h>
#include <Supergoon/state.h>
#include <imgui.h>

#include <DebugEngineTab.hpp>

using namespace std;

extern "C" {
extern uint64_t frametime;
extern int _vsyncEnabled;
int getRefreshRate(void);
}

struct timeframe {
	float Average = 0;
	float Max = 0;
	float Min = 0;
	float Current;
	float Previous[5];
};

namespace {
timeframe Second;
timeframe TenSecond;

void updateTimeframe(timeframe& t, float fps, float interval) {
	t.Current += DeltaTimeSeconds;
	if (interval != 0.0f && t.Current > interval) {
		memcpy(t.Previous, t.Previous + 1, sizeof(float) * 4);
		t.Previous[4] = fps;
		auto average = 0.0f;
		int count = 0;
		t.Max = t.Min = 0;
		for (int i = 0; i < 5; ++i) {
			auto previousFPS = t.Previous[i];
			if (previousFPS == 0) {
				continue;
			}
			average += previousFPS;
			t.Min = t.Min > 0 ? previousFPS < t.Min ? previousFPS : t.Min : previousFPS;
			t.Max = previousFPS > t.Max ? previousFPS : t.Max;
			++count;
		}
		t.Average = average / (float)count;
		t.Current -= interval;
	}
}
}  // namespace

void DebugEngineTabDraw() {
	if (ImGui::CollapsingHeader("Engine")) {
		auto refreshRate = getRefreshRate();

		float fps = 1000000000.0f / (float)frametime;
		updateTimeframe(Second, fps, 1.0f);
		updateTimeframe(TenSecond, fps, 10.0f);
		ImGui::Text("Vsync: %s", _vsyncEnabled ? "true" : "false");
		ImGui::Text("Refresh Rate: %d", refreshRate);
		ImGui::Text("FPS: %.0f, Min: %.0f, Max: %.0f", Second.Average, Second.Min, Second.Max);
		ImGui::Text("10Sec FPS: %.2f, Min: %.0f, Max: %.0f", TenSecond.Average, TenSecond.Min, TenSecond.Max);
	}
}
