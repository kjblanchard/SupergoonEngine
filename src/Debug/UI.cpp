#include <Supergoon/UI/ui.h>
#include <Supergoon/UI/uilayoutgroup.h>
#include <Supergoon/UI/uiobject.h>
#include <Supergoon/UI/uirect.h>
#include <Supergoon/UI/uitext.h>
#include <SupergoonEngine/ui.h>

#include <Supergoon/Debug/Debug.hpp>
#include <Supergoon/pch.hpp>
using namespace std;
extern UIObject *_rootUIObject;

static string GetWidgetTypeName(UIObjectTypes objectType) {
	switch (objectType) {
		case UIObjectTypesPanel:
			return "Panel";
		case UIObjectTypesImage:
			return "Image";
		default:
			return "NotImplemented";
	}
	return "Nothing?";
}

void DrawUIObjects(UIObject *uiObject) {
	assert(uiObject && uiObject->Name && "No name for object!!");
	if (ImGui::TreeNode(uiObject->Name)) {
		// auto panelLayerLabel = "Layer ##" + panelName;
		auto panelOffsetLabel = "Offset X ##" + string(uiObject->Name);
		auto panelOffsetYLabel = "Offset Y ##" + string(uiObject->Name);
		auto panelWLabel = "Width ##" + string(uiObject->Name);
		auto panelHLabel = "Height ##" + string(uiObject->Name);
		auto visibleLabel = "Visible ##" + string(uiObject->Name);
		auto activeLabel = "Active ##" + string(uiObject->Name);
		auto debugLabel = "DebugBox ##" + string(uiObject->Name);
		auto priorityLabel = "Priority ##" + string(uiObject->Name);
		auto superPriorityLabel = "Super Priority ##" + string(uiObject->Name);
		auto isVisible = (uiObject->Flags & UIObjectFlagVisible) != 0;
		auto isActive = (uiObject->Flags & UIObjectFlagActive) != 0;
		auto isDebug = (uiObject->Flags & UIObjectFlagDebugBox) != 0;
		auto isPriority = (uiObject->Flags & UIObjectFlagPriorityDraw) != 0;
		auto isSuperPriority = (uiObject->Flags & UIObjectFlagSuperPriorityDraw) != 0;
		ImGui::BeginDisabled();
		ImGui::Text("X: %f, Y: %f", uiObject->Location.x, uiObject->Location.y);
		ImGui::EndDisabled();
		auto text = "Type: " + GetWidgetTypeName(uiObject->Type);
		ImGui::Text("%s", text.c_str());
		if (ImGui::Checkbox(visibleLabel.c_str(), &isVisible)) {
			if (isVisible)
				uiObject->Flags |= UIObjectFlagVisible;
			else
				uiObject->Flags &= ~UIObjectFlagVisible;
			uiObject->Flags |= UIObjectFlagDirty;
		}
		ImGui::SameLine();
		if (ImGui::Checkbox(activeLabel.c_str(), &isActive)) {
			if (isActive)
				uiObject->Flags |= UIObjectFlagActive;
			else
				uiObject->Flags &= ~UIObjectFlagActive;
			uiObject->Flags |= UIObjectFlagDirty;
		}
		ImGui::SameLine();
		if (ImGui::Checkbox(debugLabel.c_str(), &isDebug)) {
			if (isDebug)
				uiObject->Flags |= UIObjectFlagDebugBox;
			else
				uiObject->Flags &= ~UIObjectFlagDebugBox;
			uiObject->Flags |= UIObjectFlagDirty;
		}
		if (ImGui::DragFloat(panelOffsetLabel.c_str(), &uiObject->XOffset, 1.0f)) {
			uiObject->Flags |= UIObjectFlagDirty;
		}
		if (ImGui::DragFloat(panelOffsetYLabel.c_str(), &uiObject->YOffset, 1.0f)) {
			uiObject->Flags |= UIObjectFlagDirty;
		}
		if (ImGui::DragFloat(panelWLabel.c_str(), &uiObject->Location.w, 1.0f)) {
			uiObject->Flags |= UIObjectFlagDirty;
		}
		if (ImGui::DragFloat(panelHLabel.c_str(), &uiObject->Location.h, 1.0f)) {
			uiObject->Flags |= UIObjectFlagDirty;
		}
		if (ImGui::Checkbox(priorityLabel.c_str(), &isPriority)) {
			if (isPriority)
				uiObject->Flags |= UIObjectFlagPriorityDraw;
			else
				uiObject->Flags &= ~UIObjectFlagPriorityDraw;
			uiObject->Flags |= UIObjectFlagDirty;
		}
		ImGui::SameLine();
		if (ImGui::Checkbox(superPriorityLabel.c_str(), &isSuperPriority)) {
			if (isSuperPriority)
				uiObject->Flags |= UIObjectFlagSuperPriorityDraw;
			else
				uiObject->Flags &= ~UIObjectFlagSuperPriorityDraw;
			uiObject->Flags |= UIObjectFlagDirty;
		}
		if (uiObject->Type == UIObjectTypesText) {
			auto textData = (UIText *)uiObject->Data;
			assert(textData && "No text!");
			auto numTextToDraw = "Num Text to Draw ##" + string(uiObject->Name);
			auto maxNumText = strlen(textData->Text);
			int data = textData->CurrentDrawnLetters;
			if (ImGui::SliderInt(numTextToDraw.c_str(), &data, 0, maxNumText)) {
				textData->NumLettersToDraw = data;
				uiObject->Flags |= UIObjectFlagDirty;
			}
			auto centeredXText = "CenteredX ##" + string(uiObject->Name);
			bool centeredX = textData->CenteredX;
			bool centeredY = textData->CenteredY;
			auto centeredYText = "CenteredY ##" + string(uiObject->Name);
			if (ImGui::Checkbox(centeredXText.c_str(), &centeredX)) {
				SetCenteredX(uiObject, centeredX);
			}
			ImGui::SameLine();
			if (ImGui::Checkbox(centeredYText.c_str(), &centeredY)) {
				SetCenteredY(uiObject, centeredY);
			}
			auto colorText = "Text Texture Color ##" + string(uiObject->Name);
			static float color[4] = {(float)textData->Color.R / 255.0f, (float)textData->Color.G / 255.0f, (float)textData->Color.B / 255.0f, (float)textData->Color.A / 255.0f};
			if (ImGui::ColorEdit4(colorText.c_str(), color, ImGuiColorEditFlags_AlphaBar)) {
				SetTextColor(uiObject,
							 (uint8_t)(color[0] * 255.0f + 0.5f),
							 (uint8_t)(color[1] * 255.0f + 0.5f),
							 (uint8_t)(color[2] * 255.0f + 0.5f),
							 (uint8_t)(color[3] * 255.0f + 0.5f));
			};
		} else if (uiObject->Type == UIObjectTypesRect) {
			auto rectData = (UIRect *)uiObject->Data;
			assert(rectData && "no rect!");
			auto colorText = "Draw Color ##" + string(uiObject->Name);
			static float color[4] = {(float)rectData->Color.R / 255.0f, (float)rectData->Color.G / 255.0f, (float)rectData->Color.B / 255.0f, (float)rectData->Color.A / 255.0f};
			if (ImGui::ColorEdit4(colorText.c_str(), color, ImGuiColorEditFlags_AlphaBar)) {
				rectData->Color.R = (uint8_t)(color[0] * 255.0f + 0.5f);
				rectData->Color.G = (uint8_t)(color[1] * 255.0f + 0.5f);
				rectData->Color.B = (uint8_t)(color[2] * 255.0f + 0.5f);
				rectData->Color.A = (uint8_t)(color[3] * 255.0f + 0.5f);
			};
		} else if (uiObject->Type == UIObjectTypesLayoutGroup) {
			auto hlgData = (UILayoutGroup *)uiObject->Data;
			assert(hlgData && "no layout group!");
			auto horizontalText = "Horizontal ##" + string(uiObject->Name);
			auto isHorizontal = (bool)hlgData->IsHorizontal;
			if (ImGui::Checkbox("Horizontal Layout", &isHorizontal)) {
				hlgData->IsHorizontal = isHorizontal;
				uiObject->Flags |= UIObjectFlagDirty;
			}
			auto spacingText = "Spacing ##" + string(uiObject->Name);
			if (ImGui::SliderInt(spacingText.c_str(), &hlgData->Spacing, -500, 500)) {
				uiObject->Flags |= UIObjectFlagDirty;
			}
		}
		ImGui::Text("**__Children__**");
		for (size_t i = 0; i < uiObject->ChildrenCount; i++) {
			DrawUIObjects(uiObject->Children[i]);
		}
		ImGui::TreePop();
	}
}

void ShowUIDebugWindow() {
	if (!_rootUIObject) {
		return;
	}
	auto window_flags = GetDefaultWindowFlags();
	bool p_open;
	if (!ImGui::Begin("UI", &p_open, window_flags)) {
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	if (ImGui::CollapsingHeader("UI Elements")) {
		DrawUIObjects(_rootUIObject);
	}

	// if (ImGui::CollapsingHeader("UI Animators")) {
	// 	// We will need to loop over all panels animators
	// 	for (auto &value : UI::UIInstance->Children) {
	// 		for (auto &&animator : value->Animators) {
	// 			if (ImGui::TreeNode(animator->Name.c_str())) {
	// 				auto name = std::string("Play ##") + animator->Name;
	// 				auto valueText = animator->IsPlaying() ? "True" : "False";
	// 				// auto playingText = std::string("Is Playing ") + valueText + "## " + animator->Name;
	// 				auto playingText = std::string("Is Playing ") + valueText;
	// 				ImGui::Text("%s", playingText.c_str());
	// 				ImGui::SameLine();
	// 				auto percentText = std::string("Percent: ") + std::to_string(animator->SequenceToPlay->Percent());
	// 				ImGui::Text("%s", percentText.c_str());

	// 				if (ImGui::Button(name.c_str())) {
	// 					animator->Play();
	// 				}
	// 				ImGui::SameLine();
	// 				auto stop = std::string("Stop ##") + animator->Name;
	// 				if (ImGui::Button(stop.c_str())) {
	// 					animator->Stop();
	// 				}
	// 				ImGui::SameLine();
	// 				auto pause = std::string("Pause ##") + animator->Name;
	// 				if (ImGui::Button(pause.c_str())) {
	// 					animator->Pause();
	// 				}
	// 				ImGui::TreePop();
	// 			}
	// 		}
	// 	}
	// }
	ImGui::End();
}
