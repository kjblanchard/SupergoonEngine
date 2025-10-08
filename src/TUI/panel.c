#include <Supergoon/Primitives/Point.h>
#include <Supergoon/log.h>
#include <SupergoonEngine/TUI/panel.h>
#include <SupergoonEngine/tools.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct PanelChildObject {
	void* PanelObject;
	TUIDrawCallback DrawFunc;
	struct PanelChildObject* NextChild;

} PanelChildObject;

typedef enum TUIPanelFlags {
	TUIObjectFlagDirty = 1 << 0,
	TUIObjectFlagBorder = 1 << 1,
	TUIObjectFlagDisplayNameInBorder = 1 << 2,
} TUIPanelFlags;

typedef struct Panel {
	Point Location;
	Point Size;
	char* Name;
	uint32_t Flags;
	WINDOW* CursesWindow;
	PanelChildObject* NextChild;
} Panel;

static Panel* newPanel(int x, int y, int w, int h, const char* name, bool border) {
	Panel* panel = malloc(sizeof(*panel));
	panel->Location.X = x;
	panel->Location.Y = y;
	panel->Size.X = w;
	panel->Size.Y = h;
	panel->Flags = 0;
	panel->Name = NULL;
	PanelAddBorder(panel, border);
	PanelAddName(panel, name);
	panel->CursesWindow = newwin(h, w, y, x);
	if (!panel->CursesWindow) {
		sgLogWarn("For some reason, cannot create curses window");
	}
	panel->NextChild = NULL;
	return panel;
}

static Panel* getPanel(void* panelPtr) {
	Panel* panel = (Panel*)panelPtr;
	if (!panel) {
		sgLogWarn("Bad panel passed in");
		return NULL;
	}
	if (!panel->CursesWindow) {
		sgLogWarn("Invalid Panel, No curses window created for passed panel!");
		return NULL;
	}
	return panel;
}

static void drawPanelBorder(Panel* panel) {
	box(panel->CursesWindow, 0, 0);
	if (HAS_ANY_FLAGS(panel->Flags, TUIObjectFlagDisplayNameInBorder)) {
		mvwprintw(panel->CursesWindow, 0, 2, panel->Name);
	}
}

void* PanelGetCursesWindow(void* panelPtr) {
	Panel* panel = getPanel(panelPtr);
	return panel ? panel->CursesWindow : NULL;
}

void* PanelCreate(int width, int height, int x, int y) {
	Panel* panel = newPanel(x, y, width, height, "", false);
	return panel;
}

void PanelUpdateSize(void* panel, int w, int h) {
}

void PanelUpdateLocation(void* panel, int x, int y) {
}

void PanelDestroy(void* panelPtr) {
	Panel* panel = getPanel(panelPtr);
	if (!panel) {
		sgLogDebug("Not freeing panel");
	}
	werase(panel->CursesWindow);
	// Clear all children
	wrefresh(panel->CursesWindow);
	delwin(panel->CursesWindow);
	free(panel->Name);
	free(panel);
}

void PanelAddBorder(void* panelPtr, int shouldBorder) {
	Panel* panel = getPanel(panelPtr);
	if (!panel) {
		return;
	}
	if (shouldBorder) {
		SET_FLAG(panel->Flags, TUIObjectFlagBorder | TUIObjectFlagDirty);

	} else {
		SET_FLAG(panel->Flags, TUIObjectFlagDirty);
		CLEAR_FLAG(panel->Flags, TUIObjectFlagBorder);
		CLEAR_FLAG(panel->Flags, TUIObjectFlagDisplayNameInBorder);
	}
}

void PanelAddName(void* panelPtr, const char* name) {
	Panel* panel = getPanel(panelPtr);
	if (!panel) {
		return;
	}
	if (strcmp(name, "") != 0) {
		if (panel->Name) {
			free(panel->Name);
		}
		panel->Name = strdup(name);
		SET_FLAG(panel->Flags, TUIObjectFlagDirty);
		if (HAS_ANY_FLAGS(panel->Flags, TUIObjectFlagBorder)) {
			SET_FLAG(panel->Flags, TUIObjectFlagDisplayNameInBorder);
		}
	}
}

void PanelAddChild(void* parentPanelPtr, void* childPtr, TUIDrawCallback func) {
	Panel* panel = getPanel(parentPanelPtr);
	if (!panel) {
		return;
	}
	PanelChildObject* newChild = malloc(sizeof(*newChild));
	newChild->PanelObject = childPtr;
	newChild->DrawFunc = func;
	newChild->NextChild = NULL;
	PanelChildObject* lastChild = panel->NextChild;
	if (lastChild) {
		while (lastChild->NextChild) {
			lastChild = lastChild->NextChild;
		}
		lastChild->NextChild = newChild;
	} else {
		panel->NextChild = newChild;
	}
}

void PanelDraw(void* panelPtr) {
	Panel* panel = getPanel(panelPtr);
	if (!panel) {
		sgLogWarn("Cannot draw panel, invalid panel!");
		return;
	}
	werase(panel->CursesWindow);
	if (HAS_ANY_FLAGS(panel->Flags, TUIObjectFlagBorder)) {
		drawPanelBorder(panel);
	}
	// Draw all children
	PanelChildObject* nextChild = panel->NextChild;
	while (nextChild) {
		if (nextChild->DrawFunc) {
			nextChild->DrawFunc(panel, nextChild->PanelObject);
		}
		// nextChild = panel->NextChild->NextChild;
		nextChild = nextChild->NextChild;
	}
	wrefresh(panel->CursesWindow);
}
