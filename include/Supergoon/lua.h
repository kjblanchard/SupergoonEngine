#pragma once
#ifdef __cplusplus
extern "C" {
#endif

void InitializeLuaEngine(void);
void LuaRunFile(const char* path);
void LuaPushTableFromFile(const char* path);
int LuaGetInt(const char* field);
int LuaGetIntFromStack(void);
int LuaGetIntFromTablei(int i);
void LuaPushTableToStack(const char* tableFieldName);
// If a table is on the stack, you will push the table at i on the stack
// Remember lua table indexes start at 1, so this will add 1.
void LuaPushTableObjectToStacki(int i);
// Table must be on stack
int LuaGetTableLength(void);
void LuaCopyString(const char* name, char* location, int strlen);
const char* LuaGetString(const char* name);
void LuaPopStack(int num);
void LuaClearStack(void);
void sgCloseLua(void);
#ifdef __cplusplus
}
#endif
