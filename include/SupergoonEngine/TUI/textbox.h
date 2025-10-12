#pragma once
#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Creates an initial Textbox, with the display text inside the box.
 *
 * @param parentPanel Panel, which will draw the border, etc.. this just gets a subwindow inside of that panel and fills it.k
 * @param xOffset additional offset when used, will offset 1 for the border initially
 * @param yOffset additional offset when used, will offset 1 for the border
 * @param displayText the display text to use initially
 * @return void* a textbox void ptr
 */
void* TextboxCreate(void* parentPanel, int xOffset, int yOffset, const char* displayText);
/**
 * @brief updates the full text, useful if you want to clear it, etc
 *
 * @param textPtr
 * @param newText
 */
void TextboxUpdateText(void* textPtr, const char* newText);
/**
 * @brief Writes new text to the textbox.
 *
 * @param textPtr
 * @param newText
 */
void TextboxAddLineTextWithColor(void* textPtr, const char* newText, int colorPair);
void TextboxAddText(void* textPtr, const char* newText);
void TextboxAddStyle(void* textPtr, int style);
void TextboxAddColor(void* textPtr, int colorPair);

void TextboxDraw(void* parentPanel, void* textPtr);

#ifdef __cplusplus
}
#endif
