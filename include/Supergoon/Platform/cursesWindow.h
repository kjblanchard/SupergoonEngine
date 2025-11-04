#pragma once
#ifdef __cplusplus
extern "C" {
#endif

// Matches the ncurses color like BLACK, etc
typedef enum CursesWindowTextColorPairs {
	CursesWindowTextColorPairDefault = 0,
	CursesWindowTextColorPairBlack,
	CursesWindowTextColorPairRed,
	CursesWindowTextColorPairGreen,
	CursesWindowTextColorPairYello,
	CursesWindowTextColorPairBlue,
	CursesWindowTextColorPairMagenta,
	CursesWindowTextColorPairCyan,
	CursesWindowTextColorPairWhite,
	CursesWindowTextColorPairMax,
} CursesWindowTextColorPairs;

typedef enum CursesWindowTextStyles {
	CursesWindowTextStylesDefault = 0,
	CursesWindowTextStylesBold,
	CursesWindowTextStylesMax,
} CursesWindowTextStyles;

void SetScreenDirty(int dirty);
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
