#include <sgforge/unpack.h>
#include <stdint.h>
#include <stdio.h>
float DeltaTimeSeconds = 0;
float DeltaTimeMilliseconds = 0;
struct Directory* AssetDirectory = NULL;
#ifdef imgui
struct UIObject* RootUIObject = NULL;
#endif
