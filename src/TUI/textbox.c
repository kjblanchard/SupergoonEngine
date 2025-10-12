#include <Supergoon/Primitives/Point.h>
#include <SupergoonEngine/Platform/cursesWindow.h>
#include <SupergoonEngine/TUI/panel.h>
#include <SupergoonEngine/TUI/textbox.h>
#include <SupergoonEngine/tools.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

typedef struct TUITextBox {
	// Point Offset;
	WINDOW* TextSubWindow;
} TUITextBox;

void* TextboxCreate(void* parentPanel, int xOffset, int yOffset, const char* displayText) {
	WINDOW* cursesWindow = PanelGetCursesWindow(parentPanel);
	int parentW, parentH;
	getmaxyx(cursesWindow, parentH, parentW);
	TUITextBox* text = malloc(sizeof(*text));
	// text->Offset.X = xOffset;
	// text->Offset.Y = yOffset;
	// offset by 1/1 and reduce the size by 2, to account for borders
	// create subwindow so that we can use \n in the lines properly
	text->TextSubWindow = derwin(cursesWindow, parentH - 2, parentW - 2, 1, 1);
	scrollok(text->TextSubWindow, true);
	idlok(text->TextSubWindow, true);
	TextboxUpdateText(text, displayText);
	return text;
}

void TextboxUpdateText(void* textPtr, const char* newText) {
	TUITextBox* text = (TUITextBox*)textPtr;
	if (!text || !text->TextSubWindow) {
		return;
	}
	wclear(text->TextSubWindow);
	// go to the beginning, and print it
	mvwprintw(
		text->TextSubWindow,
		// text->Offset.Y,
		// text->Offset.X,
		0, 0, "%s",
		newText);
}

void TextboxAddLineTextWithColor(void* textPtr, const char* newText, int colorPair) {
	TUITextBox* text = (TUITextBox*)textPtr;
	if (!text || !text->TextSubWindow) {
		return;
	}
	wattron(text->TextSubWindow, COLOR_PAIR(colorPair));
	wprintw(text->TextSubWindow, "%s", newText);
	wattroff(text->TextSubWindow, COLOR_PAIR(colorPair));
	wrefresh(text->TextSubWindow);
}

void TextboxAddText(void* textPtr, const char* newText) {
	TUITextBox* text = (TUITextBox*)textPtr;
	if (!text || !text->TextSubWindow) {
		return;
	}
	wprintw(text->TextSubWindow, "%s", newText);
	wrefresh(text->TextSubWindow);
}

void TextboxAddColor(void* textPtr, int colorPair) {
	TUITextBox* text = (TUITextBox*)textPtr;
	if (!text || !text->TextSubWindow) {
		return;
	}
	wattron(text->TextSubWindow, COLOR_PAIR(colorPair));
}

void TextboxAddStyle(void* textPtr, int style) {
	TUITextBox* text = (TUITextBox*)textPtr;
	if (!text || !text->TextSubWindow) {
		return;
	}
	switch (style) {
		case CursesWindowTextStylesBold:
			wattron(text->TextSubWindow, A_BOLD);
			break;
		case CursesWindowTextStylesDefault:
			wattrset(text->TextSubWindow, A_NORMAL);
		default:
			break;
	}
}

void TextboxDraw(void* parentPanel, void* textPtr) {}
