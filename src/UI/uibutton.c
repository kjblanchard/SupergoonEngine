//#include <Supergoon/Input/mouse.h>
//#include <Supergoon/Input/touch.h>
//#include <Supergoon/UI/uibutton.h>

// void UpdateUIButton(UIObject* obj) {
// 	UIButton* buttonData = (UIButton*)obj->Data;
// 	if (!buttonData) return;

// 	const float x = obj->Location.x;
// 	const float y = obj->Location.y;
// 	const float w = obj->Location.w;
// 	const float h = obj->Location.h;

// 	bool mouseHover = IsMouseOverlapRect(x, y, w, h);
// 	bool touchHover = IsTouchOverlapRect(x, y, w, h);
// 	bool isHovered = mouseHover || touchHover;
// 	bool isClicked = false;

// 	// No hover, reset and return
// 	if (!isHovered) {
// 		buttonData->MouseOverLastFrame = 0;
// 		buttonData->TouchOverLastFrame = 0;
// 		return;
// 	}

// 	// Hover event
// 	if (buttonData->ButtonHoverEvent) {
// 		bool justHovered = !buttonData->MouseOverLastFrame && isHovered;
// 		buttonData->ButtonHoverEvent(obj, justHovered);
// 	}

// 	// Mouse click handling
// 	if (mouseHover) {
// 		if (buttonData->ClickOnRelease) {
// 			if (IsMouseButtonJustReleased(MouseButtonsLeftClick) && buttonData->MouseOverLastFrame) {
// 				if (buttonData->ButtonClickEvent) {
// 					buttonData->ButtonClickEvent(obj, buttonData->ClickedLastFrame);
// 					isClicked = true;
// 				}
// 			}
// 		} else {
// 			if (IsMouseButtonDown(MouseButtonsLeftClick)) {
// 				if (buttonData->ButtonClickEvent) {
// 					buttonData->ButtonClickEvent(obj, buttonData->ClickedLastFrame);
// 					isClicked = true;
// 				}
// 			}
// 		}
// 	}

// 	// Touch click handling
// 	if (touchHover) {
// 		if (buttonData->ClickOnRelease) {
// 			if (IsTouchJustReleased() && buttonData->TouchOverLastFrame) {
// 				if (buttonData->ButtonClickEvent) {
// 					buttonData->ButtonClickEvent(obj, buttonData->ClickedLastFrame);
// 					isClicked = true;
// 				}
// 			}
// 			if (IsTouchJustPressed() || IsTouchHeldDown()) {
// 				buttonData->TouchOverLastFrame = 1;
// 			} else {
// 				buttonData->TouchOverLastFrame = 0;
// 			}
// 		} else {
// 			if ((IsTouchJustPressed() || IsTouchHeldDown()) && buttonData->ButtonClickEvent) {
// 				buttonData->ButtonClickEvent(obj, buttonData->ClickedLastFrame);
// 				isClicked = true;
// 			}
// 		}
// 	}

// 	// Save state for next frame
// 	buttonData->ClickedLastFrame = isClicked;
// 	buttonData->MouseOverLastFrame = isHovered;
// }

#include <Supergoon/Input/mouse.h>
#include <Supergoon/Input/touch.h>
#include <Supergoon/UI/uibutton.h>

//void UpdateUIButton(UIObject* obj) {
//	UIButton* buttonData = (UIButton*)obj->Data;
//	if (!buttonData) return;
//
//	const float x = obj->Location.x;
//	const float y = obj->Location.y;
//	const float w = obj->Location.w;
//	const float h = obj->Location.h;
//
//#if IS_MOBILE
//	bool hover = IsTouchOverlapRect(x, y, w, h);
//#else
//	bool hover = IsMouseOverlapRect(x, y, w, h);
//#endif
//
//	bool isClicked = false;
//
//	// Reset if not hovering
//	if (!hover) {
//#if IS_MOBILE
//		buttonData->TouchOverLastFrame = 0;
//#else
//		buttonData->MouseOverLastFrame = 0;
//#endif
//		return;
//	}
//
//	// Hover event
//	if (buttonData->ButtonHoverEvent) {
//#if IS_MOBILE
//		bool justHovered = !buttonData->TouchOverLastFrame && hover;
//#else
//		bool justHovered = !buttonData->MouseOverLastFrame && hover;
//#endif
//		buttonData->ButtonHoverEvent(obj, justHovered);
//	}
//
//#if IS_MOBILE
//    // Touch input logic
//    if (buttonData->ClickOnRelease) {
//        // Trigger click on release only if still hovering
//        if (IsTouchJustReleased()) {
////            if (IsTouchJustReleased() && buttonData->TouchOverLastFrame) {
//            if (buttonData->ButtonClickEvent) {
//                buttonData->ButtonClickEvent(obj, !buttonData->ClickedLastFrame);
//                isClicked = true;
//            }
//        }
//
//        // Track whether we were hovering while finger was down
//        if (IsTouchJustPressed() || IsTouchHeldDown()) {
//            buttonData->TouchOverLastFrame = 1;
//        } else if (IsTouchJustReleased()) {
//            buttonData->TouchOverLastFrame = 0; // Reset only after release
//        }
//    } else {
//        // Immediate click on press or hold
//        if ((IsTouchJustPressed() || IsTouchHeldDown()) && buttonData->ButtonClickEvent) {
//            buttonData->ButtonClickEvent(obj, !buttonData->ClickedLastFrame);
//            isClicked = true;
//        }
//    }
//
//#else
//	// Mouse input logic (always click on press)
//	if (IsMouseButtonDown(MouseButtonsLeftClick) && buttonData->ButtonClickEvent) {
//		buttonData->ButtonClickEvent(obj, !buttonData->ClickedLastFrame);
//		isClicked = true;
//	}
//#endif
//
//	// Save frame state
//	buttonData->ClickedLastFrame = isClicked;
//
//#if IS_MOBILE
//	buttonData->TouchOverLastFrame = hover;
//#else
//	buttonData->MouseOverLastFrame = hover;
//#endif
//}
//
//#include <Supergoon/Input/mouse.h>
//#include <Supergoon/Input/touch.h>
//#include <Supergoon/UI/uibutton.h>

void UpdateUIButton(UIObject* obj) {
    UIButton* buttonData = (UIButton*)obj->Data;
    if (!buttonData) return;

    const float x = obj->Location.x;
    const float y = obj->Location.y;
    const float w = obj->Location.w;
    const float h = obj->Location.h;

#if IS_MOBILE
    bool hover = IsTouchOverlapRect(x, y, w, h);
#else
    bool hover = IsMouseOverlapRect(x, y, w, h);
#endif

    bool isClicked = false;

    // Hover event
    if (buttonData->ButtonHoverEvent) {
#if IS_MOBILE
        bool justHovered = !buttonData->TouchOverLastFrame && hover;
#else
        bool justHovered = !buttonData->MouseOverLastFrame && hover;
#endif
        buttonData->ButtonHoverEvent(obj, justHovered);
    }

#if IS_MOBILE
    // Touch input logic
    if (buttonData->ClickOnRelease) {
        // Trigger click if touch was released after being down on the button
        if (IsTouchJustReleased() && buttonData->TouchOverLastFrame) {
            if (buttonData->ButtonClickEvent) {
                buttonData->ButtonClickEvent(obj, !buttonData->ClickedLastFrame);
                isClicked = true;
            }
        }

        // Track hover state while touch is held or just pressed
        if (IsTouchJustPressed() || IsTouchHeldDown()) {
            buttonData->TouchOverLastFrame = hover;
        } else if (IsTouchJustReleased()) {
            buttonData->TouchOverLastFrame = 0;
        }
    } else {
        // Immediate click while pressing or holding, only if hovering
        if ((IsTouchJustPressed() || IsTouchHeldDown()) && hover && buttonData->ButtonClickEvent) {
            buttonData->ButtonClickEvent(obj, !buttonData->ClickedLastFrame);
            isClicked = true;
        }
    }
#else
    // Mouse input logic (always click on press if hovering)
    if (IsMouseButtonDown(MouseButtonsLeftClick) && hover && buttonData->ButtonClickEvent) {
        buttonData->ButtonClickEvent(obj, !buttonData->ClickedLastFrame);
        isClicked = true;
    }
#endif

    // Save frame state
    buttonData->ClickedLastFrame = isClicked;

#if IS_MOBILE
    buttonData->TouchOverLastFrame = hover;
#else
    buttonData->MouseOverLastFrame = hover;
#endif
}
