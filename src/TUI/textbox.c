#include <Supergoon/Primitives/Point.h>
#include <SupergoonEngine/TUI/panel.h>
#include <SupergoonEngine/TUI/textbox.h>
#include <SupergoonEngine/tools.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

typedef struct TUITextBox {
	Point Offset;
	// char* Text;
	// size_t StringSize;
	WINDOW* TextSubWindow;
} TUITextBox;

void* TextboxCreate(void* parentPanel, int xOffset, int yOffset, const char* displayText) {
	WINDOW* cursesWindow = PanelGetCursesWindow(parentPanel);
	int parentW, parentH;
	getmaxyx(cursesWindow, parentH, parentW);
	TUITextBox* text = malloc(sizeof(*text));
	// text->StringSize = 0;
	text->Offset.X = xOffset;
	text->Offset.Y = yOffset;
	// text->Text = NULL;
	// offset by 1/1 and reduce the size by 2, to account for borders
	// create subwindow so that we can use \n in the lines properly
	text->TextSubWindow = derwin(cursesWindow, parentH - 2, parentW - 2, 1, 1);
	scrollok(text->TextSubWindow, true);
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
		text->Offset.Y,
		text->Offset.X,
		"%s",
		newText);
}

void TextboxAddText(void* textPtr, const char* newText, int colorPair) {
	TUITextBox* text = (TUITextBox*)textPtr;
	if (!text || !text->TextSubWindow) {
		return;
	}
	wattron(text->TextSubWindow, COLOR_PAIR(colorPair));
	wprintw(text->TextSubWindow, "%s", newText);
	wattroff(text->TextSubWindow, COLOR_PAIR(colorPair));
}

void TextboxDraw(void* parentPanel, void* textPtr) {
	// TUITextBox* text = (TUITextBox*)textPtr;
	// if (!text || !text->TextSubWindow) {
	// 	return;
	// }
	// mvwprintw(
	// 	text->TextSubWindow,
	// 	text->Offset.Y,
	// 	text->Offset.X,
	// 	"%s",
	// 	textPtr.Text);
}
