#include <SDL3/SDL_messagebox.h>
#include <Supergoon/Primitives/Color.h>
#include <Supergoon/UI/image.h>
#include <Supergoon/UI/object.h>
#include <Supergoon/sprite.h>
#include <Supergoon/state.h>
#include <imgui.h>
#include <sgtools/log.h>

#include <DebugUIWindow.hpp>
using namespace std;

namespace {

void drawUIObject(UIObject* o);

void drawUIObjectList(UIObject* o) {
	while (o) {
		drawUIObject(o);
		o = o->Sibling;
	}
}

void drawUIImage(UIImageData* data) {
	if (!data) {
		return;
	}
	int color[4] = {
		data->Sprite->DrawColor.R,
		data->Sprite->DrawColor.G,
		data->Sprite->DrawColor.B,
		data->Sprite->DrawColor.A};

	if (ImGui::SliderInt4("Color", color, 0, 255)) {
		data->Sprite->DrawColor.R = (uint8_t)color[0];
		data->Sprite->DrawColor.G = (uint8_t)color[1];
		data->Sprite->DrawColor.B = (uint8_t)color[2];
		data->Sprite->DrawColor.A = (uint8_t)color[3];
	}
}

void drawType(const char* type, void* data) {
	if (strcmp(type, "Image") == 0) {
		drawUIImage((UIImageData*)data);
	}
}

void drawUIObject(UIObject* o) {
	ImGui::PushID((int)o->Id);
	if (ImGui::TreeNode(o->Name)) {
		ImGui::SliderFloat("X", &o->Rect.x, -200, 200);
		ImGui::SliderFloat("AbsX", &o->AbsolutePos.X, -200, 200);
		ImGui::SliderFloat("Y", &o->Rect.y, -200, 200);
		ImGui::SliderFloat("AbsY", &o->AbsolutePos.Y, -200, 200);
		ImGui::SliderFloat("W", &o->Rect.w, 0, 200);
		ImGui::SliderFloat("H", &o->Rect.h, 0, 200);
		bool debug = o->Debug;
		if (ImGui::Checkbox("DebugBox", &debug)) {
			o->Debug = debug;
		}
		ImGui::SameLine();
		bool visible = o->Visible;
		if (ImGui::Checkbox("Visible", &visible)) {
			o->Visible = visible;
		}
		if (ImGui::InputScalar("Priority", ImGuiDataType_U32, &o->Priority)) {
			UIObjectUpdatePriority(o, o->Priority);
		}
		// Draw the type info
		if (o->Type && o->Type->Name) {
			ImGui::Text("Type: %s", o->Type->Name);
		}
		drawUIObjectList(o->Child);
		ImGui::TreePop();
	}
	ImGui::PopID();
}

}  // namespace

void DebugUIWindowDraw() {
	if (ImGui::CollapsingHeader("UI")) {
		if (!RootUIObject) {
			return;
		}
		drawUIObject(RootUIObject);
	}
}
