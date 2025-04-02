#include <Supergoon/UI/ui.h>
#include <Supergoon/UI/uiobject.h>
#include <SupergoonEngine/ui.h>

#include <Supergoon/Debug/Debug.hpp>
#include <Supergoon/pch.hpp>
using namespace std;
extern UIObject *_rootUIObject;

static string GetWidgetTypeName(UIObjectTypes objectType) {
	switch (objectType) {
		case Panel:
			return "Panel";
		default:
			return "NotImplemented";
	}
	return "Nothing?";
}

void DrawUIObjects(UIObject *uiObject) {
	if (ImGui::TreeNode(uiObject->Name)) {
		// auto panelLayerLabel = "Layer ##" + panelName;
		auto panelOffsetLabel = "Offset X ##" + string(uiObject->Name);
		auto panelOffsetYLabel = "Offset Y ##" + string(uiObject->Name);
		auto panelWLabel = "Width ##" + string(uiObject->Name);
		auto panelHLabel = "Height ##" + string(uiObject->Name);
		auto visibleLabel = "Visible ##" + string(uiObject->Name);
		auto activeLabel = "Active ##" + string(uiObject->Name);
		auto debugLabel = "DebugBox ##" + string(uiObject->Name);
		auto isVisible = (uiObject->Flags & UIObjectFlagVisible) != 0;
		auto isActive = (uiObject->Flags & UIObjectFlagActive) != 0;
		auto isDebug = (uiObject->Flags & UIObjectFlagDebugBox) != 0;
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
		// if (ImGui::DragInt(panelLayerLabel.c_str(), &uiObject->_layer, 1, 0, 100)) {
		// 	uiObject->SetDirty();
		// }
		// if (uiObject->WidgetType == (int)BuiltinWidgetTypes::Image) {
		// 	auto imageObject = dynamic_cast<UIImage *>(uiObject);
		// 	std::string childWLabel = "Child W##" + panelName;
		// 	std::string childHLabel = "Child H##" + panelName;
		// 	if (ImGui::DragFloat(childWLabel.c_str(), &imageObject->Bounds.W, 1.0f)) {
		// 		uiObject->SetDirty();
		// 	}
		// 	if (ImGui::DragFloat(childHLabel.c_str(), &imageObject->Bounds.H, 1.0f)) {
		// 		uiObject->SetDirty();
		// 	}
		// } else if (uiObject->WidgetType == (int)BuiltinWidgetTypes::Text) {
		// 	assert((UIText *)uiObject);
		// 	auto textUIObject = (UIText *)uiObject;
		// 	std::string childW_label = "Width##" + panelName;
		// 	std::string childH_label = "Height##" + panelName;
		// 	std::string childXBounds = "TextBoundsX##" + panelName;
		// 	std::string childYBounds = "TextBoundsY##" + panelName;
		// 	std::string childWordWrapLabel = "WordWrap##" + panelName;
		// 	std::string childCenterLabel = "Center##" + panelName;
		// 	std::string childCenterLabelY = "CenterY##" + panelName;
		// 	std::string childLettersToDraw = "Letters To Draw##" + panelName;
		// 	std::string childDebugBoxCheckbox = "DebugBox##" + panelName;
		// 	if (ImGui::DragFloat(childW_label.c_str(), &uiObject->Bounds.W, 1.0f, 0.0f, FLT_MAX)) {
		// 		uiObject->SetDirty();
		// 	}
		// 	if (ImGui::DragFloat(childH_label.c_str(), &uiObject->Bounds.H, 1.0f, 0.0f, FLT_MAX)) {
		// 		uiObject->SetDirty();
		// 	}
		// 	if (ImGui::Checkbox(childWordWrapLabel.c_str(), &textUIObject->WordWrap)) {
		// 		uiObject->SetDirty();
		// 	};
		// 	ImGui::SameLine();
		// 	if (ImGui::Checkbox(childCenterLabel.c_str(), &textUIObject->CenterTextX)) {
		// 		uiObject->SetDirty();
		// 	};
		// 	ImGui::SameLine();
		// 	if (ImGui::Checkbox(childCenterLabelY.c_str(), &textUIObject->_centerTextY)) {
		// 		uiObject->SetDirty();
		// 	};
		// 	ImGui::SameLine();
		// 	ImGui::Checkbox(childDebugBoxCheckbox.c_str(), &shouldDrawDebugBox);

		// 	if (shouldDrawDebugBox) {
		// 		Graphics::Instance()->DrawRect(textUIObject->Bounds, {255, 0, 0, 255});
		// 	}
		// 	if (ImGui::DragInt(childLettersToDraw.c_str(), &textUIObject->_currentLetters, 1, 0, textUIObject->TextPtr->_text.length())) {
		// 		uiObject->SetDirty();
		// 	}
		// } else if (uiObject->WidgetType == (int)BuiltinWidgetTypes::HorizontalLayoutGroup) {
		// 	assert((UIHorizontalLayoutGroup *)uiObject);
		// 	auto horiGroup = (UIHorizontalLayoutGroup *)uiObject;
		// 	std::string childSpaceLabel = "Space Y##" + panelName;
		// 	if (ImGui::DragFloat(childSpaceLabel.c_str(), &horiGroup->XSpaceBetweenElements, 1.0f, -255, 255)) {
		// 		uiObject->SetDirty();
		// 	}
		// 	// ImGui::End();
		// } else if (uiObject->WidgetType == (int)BuiltinWidgetTypes::VerticalLayoutGroup) {
		// 	assert((UIVerticalLayoutGroup *)uiObject);
		// 	auto horiGroup = (UIVerticalLayoutGroup *)uiObject;
		// 	std::string childSpaceLabel = "Space Y##" + panelName;
		// 	if (ImGui::DragFloat(childSpaceLabel.c_str(), &horiGroup->YSpaceBetweenElements, 1.0f, -255, 255)) {
		// 		uiObject->SetDirty();
		// 	}
		// } else if (uiObject->WidgetType == (int)BuiltinWidgetTypes::ProgressBar) {
		// 	assert((UIProgressBar *)uiObject);
		// 	auto progressBar = (UIProgressBar *)uiObject;
		// 	std::string barOffsetXLabel = "BarOffsetX##" + panelName;
		// 	std::string barOffsetYLabel = "BarOffsetY##" + panelName;
		// 	std::string barXLabel = "BarWidth##" + panelName;
		// 	std::string barYLabel = "BarHeight##" + panelName;
		// 	std::string barPercentLabel = "BarPercent##" + panelName;
		// if (ImGui::DragFloat(barOffsetXLabel.c_str(), &progressBar->BarOffset.X, 1.0f, 0.0f)) {
		// 	progressBar->SetDirty();
		// }
		// if (ImGui::DragFloat(barOffsetYLabel.c_str(), &progressBar->BarOffset.Y, 1.0f, 0.0f)) {
		// 	progressBar->SetDirty();
		// }
		// if (ImGui::DragFloat(barXLabel.c_str(), &progressBar->BarSize.X, 1.0f, 0.0f, 200)) {
		// 	progressBar->SetDirty();
		// }
		// if (ImGui::DragFloat(barYLabel.c_str(), &progressBar->BarSize.Y, 1.0f, 0.0f, 200)) {
		// 	progressBar->SetDirty();
		// }
		// if (ImGui::DragInt(barPercentLabel.c_str(), &progressBar->BarPercent, 1.0f, 0.0f)) {
		// 	progressBar->SetDirty();
		// }
		// }

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
