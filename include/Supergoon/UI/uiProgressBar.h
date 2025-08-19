#pragma once
#include <Supergoon/Primitives/Color.h>
#include <Supergoon/UI/uiobject.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UIProgressBarData {
	sgColor Color;
	int BarPercent;
} UIProgressBarData;

void DrawProgressBar(UIObject* uiobject);
#ifdef __cplusplus
}
#endif
