#include <Supergoon/Primitives/Point.h>
#include <SupergoonEngine/TUI/panel.h>
#include <SupergoonEngine/TUI/text.h>
#include <SupergoonEngine/tools.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

typedef struct TUIText {
	Point Offset;
	char* Text;
	size_t StringSize;
	WINDOW* TextSubWindow;
} TUIText;

void* TextCreate(void* parentPanel, int xOffset, int yOffset, const char* displayText) {
	WINDOW* cursesWindow = PanelGetCursesWindow(parentPanel);
	int parentW, parentH;
	getmaxyx(cursesWindow, parentH, parentW);
	TUIText* text = malloc(sizeof(*text));
	text->StringSize = 0;
	text->Offset.X = xOffset;
	text->Offset.Y = yOffset;
	text->Text = NULL;
	// offset by 1/1 and reduce the size by 2, to account for borders
	// create subwindow so that we can use \n in the lines properly
	text->TextSubWindow = derwin(cursesWindow, parentH - 2, parentW - 2, 1, 1);
	scrollok(text->TextSubWindow, true);
	TextUpdateText(text, displayText);
	return text;
}

void TextUpdateText(void* textPtr, const char* newText) {
	TUIText* text = (TUIText*)textPtr;
	if (!text) {
		return;
	}
	if (strlen(newText) >= text->StringSize) {
		if (text->Text) {
			free(text->Text);
		}
		text->Text = strdup(newText);
		text->StringSize = strlen(text->Text);
	} else {
		text->Text = strncpy(text->Text, newText, text->StringSize);
	}
}

void TextDraw(void* parentPanel, void* textPtr) {
	TUIText* text = (TUIText*)textPtr;
	if (!text || !text->TextSubWindow) {
		return;
	}
	mvwprintw(
		text->TextSubWindow,
		text->Offset.Y,
		text->Offset.X,
		"%s",
		text->Text);
}
