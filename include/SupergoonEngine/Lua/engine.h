#pragma once
#ifdef __cplusplus
extern "C" {
#endif
void RegisterLuaEngineFunctions(void);
// Updates the lua gamestate objects so they can be used from lua without traversing to C, since most objects will use them.
void PushGamestateToLua(void);

#ifdef __cplusplus
}
#endif
