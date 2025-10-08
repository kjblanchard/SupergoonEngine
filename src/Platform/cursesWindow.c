#include <SupergoonEngine/Platform/cursesWindow.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

int TARGET_FPS = 60;
static int _windowWidth = 0;
static int _windowHeight = 0;
static char* _windowName = NULL;

void SetScreenDirty(int dirty) {
}

void SetWindowOptionsImpl(int width, int height, const char* name) {
	if (_windowName) {
		free(_windowName);
	}
	_windowName = strdup(name);
}
void SetScalingOptionsImpl(int worldWidth, int worldHeight) {
}
void CreateWindowImpl(void) {
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);
	nodelay(stdscr, TRUE);
	getmaxyx(stdscr, _windowHeight, _windowWidth);
	box(stdscr, 0, 0);
	SetWindowOptionsImpl(_windowWidth, _windowHeight, "Escape the Fate v0.1");
	int title_x = (_windowWidth - strlen(_windowName)) / 2;
	mvprintw(0, title_x, "%s", _windowName);
	WINDOW* win = newwin(10, 30, 2, 4);		// height, width, y, x
	box(win, 0, 0);							// draw a border around it
	mvwprintw(win, 0, 2, "[ My Window ]");	// print title at top border
	wrefresh(stdscr);
	getch(); /* Wait for user input */
	return;
}
void DrawStartImpl(void) {
}
void DrawEndImpl(void) {
}
int WindowHeightImpl(void) {
	return _windowHeight;
}
int WindowWidthImpl(void) {
	return _windowWidth;
}
void CloseWindowImpl(void) {
	free(_windowName);
	endwin();
}
