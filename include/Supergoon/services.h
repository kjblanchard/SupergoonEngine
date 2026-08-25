/**
* @file services.h
* @author Kevin B (kevin@supergoon.com)
* @brief Service integrations
* @version 0.1
* @date 2026-08-09
*
* @copyright Copyright (c) 2026
*
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void InitializeServiceSystem(void);
void UpdateServiceSystem(void);
void ShutdownServiceSystem(void);

#ifdef steamed
void ShowSteamKeyboard(int x, int y, int w, int h);
#endif

#ifdef __cplusplus
}
#endif
