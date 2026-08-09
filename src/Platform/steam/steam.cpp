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
