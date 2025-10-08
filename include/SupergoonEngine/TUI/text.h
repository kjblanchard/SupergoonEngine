#pragma once
#ifdef __cplusplus
extern "C" {
#endif
void* TextCreate(void* parentPanel, int xOffset, int yOffset, const char* displayText);
void TextDraw(void* parentPanel, void* textPtr);
void TextUpdateText(void* textPtr, const char* newText);

#ifdef __cplusplus
}
#endif
