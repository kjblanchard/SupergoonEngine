#pragma once
#ifdef __cplusplus
extern "C" {
#endif

void SetWindowOptionsImpl(int width, int height, const char* name);
void SetScalingOptionsImpl(int worldWidth, int worldHeight);
void CreateWindowImpl(void);
void DrawStartImpl(void);
void DrawEndImpl(void);
int WindowHeightImpl(void);
int WindowWidthImpl(void);
void CloseWindowImpl(void);
#ifdef __cplusplus
}
#endif
