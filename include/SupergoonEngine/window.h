/**
 * @file window.h
 * @author your name (you@domain.com)
 * @brief Engine include for non public variables, these could likely live in functions if need be but are accessed in a critical manner in each draw or update loop.
 * @version 0.1
 * @date 2025-05-07
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once
typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_Texture Texture;
#ifdef __cplusplus
extern "C" {
#endif

extern Texture* _imguiGameTexture;
extern int _gameImagePosX;
extern int _gameImagePosY;
extern float _gameImageScale;
extern int _gameImageWidth;
extern int _gameImageHeight;
extern SDL_Renderer* _renderer;
extern int _logicalWidth;
extern int _logicalHeight;
void handleWindowEvents();
#ifdef __cplusplus
}
#endif
