#pragma once
#ifdef __cplusplus
extern "C" {
#endif
int IsScreenFading();
void FadeinScreen(float fadeinTimeInSeconds);
void FadeoutScreen(float fadeoutTimeInSeconds);
#ifdef __cplusplus
}
#endif
