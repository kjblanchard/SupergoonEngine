#include <steam/steam_api.h>

extern "C" void InitializeServiceSystemImpl(void) {
	SteamAPI_Init();
}

extern "C" void UpdateServiceSystemImpl(void) {
	SteamAPI_RunCallbacks();
}

extern "C" void ShutdownServiceSystemImpl(void) {
	SteamAPI_Shutdown();
}

extern "C" void ShowSteamKeyboard(int x, int y, int w, int h) {
	SteamUtils()->ShowFloatingGamepadTextInput(
		k_EFloatingGamepadTextInputModeModeSingleLine, x, y, w, h);
}
