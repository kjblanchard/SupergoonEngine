#pragma once
#ifdef __cplusplus
extern "C" {
#endif

// Keep these here for when registering functions, lua is mad lad gay,
typedef struct lua_State lua_State;
// Defined in lua.c
extern lua_State* _luaState;

void InitializeLuaEngine(void);
void LuaRunFile(const char* path);
int LuaGetStackSize(void);
void LuaPushTableFromFile(const char* path);
int LuaGetInt(const char* field);
int LuaGetIntFromStack(void);
int LuaGetIntFromStacki(int i);
int LuaGetBooli(int i);

int LuaGetIntFromTablei(int i);
float LuaGetFloat(const char* field);
float LuaGetFloatFromStack(void);
void LuaPushTableToStack(const char* tableFieldName);
// If a table is on the stack, you will push the table at i on the stack
// Remember lua table indexes start at 1, so this will add 1.
void LuaPushTableObjectToStacki(int i);
// Table must be on stack, and an array table
int LuaGetTableLength(void);
// Table must be on stack, used for key/value tables
int LuaGetTableLengthMap(void);
int LuaGetTablei(int i);
void* LuaGetLightUserdatai(int i);
// i is stack location of table, const char* is key.. so not directly on stack -1.
float LuaGetFloatFromTableStacki(int i, const char* key);
void LuaCopyString(const char* name, char* location, int strlen);
// Does not pop off, please do the needful
void LuaCopyStringStack(int stackLocation, char* location, int strlen);
// String at location must be freed, allocates memory to it.
char* LuaAllocateStringStack(int stackLocation);
const char* LuaGetString(const char* name);
const char* LuaGetStringi(int i);
void LuaPopStack(int num);
void LuaClearStack(void);
// Starts the table iteration for lua tables that are key/value pairs, returns if there is any more values left
// This pushes two values to the table for iteration, The key is -2, value is -1.  Key is used for iteration, so don't pop it off, use stopiteration when done, only pop value
void LuaStartTableKeyValueIteration(void);
// Goes to the next item in the table, returns if there is any more values left
// You must pop the value off before next iteration.
int LuaNextTableKeyValueIterate(void);
int LuaIsString(int stackLocation);
int LuaIsFloat(int stackLocation);
int LuaIsInt(int stackLocation);
int LuaIsTable(int stackLocation);
int LuaIsNili(int stackLocation);
int LuaIsBool(int stackLocation);
void LuaPushNil();
void LuaPushLightUserdata(void* data);
// Ends the iteration
void LuaEndTableKeyValueIteration(void);
void sgCloseLua(void);
#ifdef __cplusplus
}
#endif
