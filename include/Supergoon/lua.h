#pragma once
#ifdef __cplusplus
extern "C" {
#endif

// Keep these here for when registering functions, lua is mad lad gay,
typedef struct lua_State* LuaState;
// Defined in lua.c
extern LuaState _luaState;

void InitializeLuaEngine(void);
void LuaRunFile(const char* path);
int LuaGetStackSize(LuaState L);
void LuaPopStack(LuaState L, int num);
void LuaClearStack(LuaState L);
// Gets the number of items on the stack, remember that the index starts at 1.
int LuaGetStack(LuaState L);
void LuaRemoveIndex(LuaState L, int index);
// Moves the tip to the index passed in, pushing everything else up.  Useful for if you pass func args first.
void LuaMoveStackTipToIndex(LuaState L, int index);
void sgCloseLua(void);
// Tables
void LuaPushTableFromGlobal(LuaState L, const char* global);
void LuaPushTableFromFile(LuaState L, const char* path);
void LuaPushTableFromStackTip(LuaState L, const char* path);
int LuaGetIntFromTablei(LuaState L, int i);
const char* LuaGetStringFromTablei(LuaState L, int i);
float LuaGetFloatFromTablei(LuaState L, int i);
void LuaPushNewTableToStack(LuaState L);
// Table must be on tip of stack.
void LuaPushTableToTable(LuaState L, const char* key);
void LuaPushValueToArrayTable(LuaState L);
void LuaPushFloatToTable(LuaState L, const char* key, float value);
void LuaPushStringToTable(LuaState L, const char* key, const char* value);
void LuaPushIntToTable(LuaState L, const char* key, int value);
void LuaPushTableFromRegistryByName(LuaState L, const char* tableName);
void LuaPushTableToStacki(LuaState L, int i);
// If a table is on the stack, you will push the table at i on the stack
// Remember lua table indexes start at 1, so this will add 1.
void LuaPushTableObjectToStacki(LuaState L, int i);
// Table must be on stack, and an array table
int LuaGetTableLength(LuaState L);
// Table must be on stack, used for key/value tables
int LuaGetTableLengthMap(LuaState L);
// Gets the table from field of the table on the tip of the stack (-1)
void LuaGetTable(LuaState L, const char* tableFieldName);
int LuaGetTablei(LuaState L, int i);
// Starts the table iteration for lua tables that are key/value pairs, returns if there is any more values left
// This pushes two values to the table for iteration, The key is -2, value is -1.  Key is used for iteration, so don't pop it off, use stopiteration when done, only pop value
void LuaStartTableKeyValueIteration(LuaState L);
// Goes to the next item in the table, returns if there is any more values left
// You must pop the value off before next iteration.
int LuaNextTableKeyValueIterate(LuaState L);
// Ends the iteration
void LuaEndTableKeyValueIteration(LuaState L);

// Ints
int LuaGetInt(LuaState L, const char* field);
int LuaGetIntFromStack(LuaState L);
int LuaGetIntFromStacki(LuaState L, int i);
// Bools
int LuaGetBool(LuaState L, const char* field);
int LuaGetBooli(LuaState L, int i);
// Floats
float LuaGetFloat(LuaState L, const char* field);
float LuaGetFloati(LuaState L, int i);
float LuaGetFloatFromStack(LuaState L);
// Gets a float from a table at position i with field name of key
float LuaGetFloatFromTableStackiKey(LuaState L, int i, const char* key);
// Strings
// i is stack location of table, const char* is key.. so not directly on stack -1.
void LuaCopyString(LuaState L, const char* name, char* location, int strlen);
// Does not pop off, please do the needful
void LuaCopyStringStack(LuaState L, int stackLocation, char* location, int strlen);
char* LuaAllocateString(LuaState L, const char* fieldName);
// String at location must be freed, allocates memory to it.
char* LuaAllocateStringStack(LuaState L, int stackLocation);
const char* LuaGetString(LuaState L, const char* name);
const char* LuaGetStringi(LuaState L, int i);
// Get light userdata at stack position i, or null if not userdata.
void* LuaGetLightUserdatai(LuaState L, int i);
// CheckStacks
int LuaIsString(LuaState L, int stackLocation);
int LuaIsFloat(LuaState L, int stackLocation);
int LuaIsInt(LuaState L, int stackLocation);
int LuaIsTable(LuaState L, int stackLocation);
int LuaIsNili(LuaState L, int stackLocation);
int LuaIsBool(LuaState L, int stackLocation);
int LuaIsLuaFunc(LuaState L, int stackLocation);
// Push to stack
void LuaPushBool(LuaState L, int boolean);
void LuaPushNil(LuaState L);
void LuaPushLightUserdata(LuaState L, void* data);
void LuaPushFloat(LuaState L, float data);
void LuaPushInt(LuaState L, int data);
void LuaPushString(LuaState L, const char* data);
// Registry
void LuaEnsureRegistryTable(LuaState L, const char* registryKey);
int LuaRegistryGetSubTableEntry(LuaState L, const char* registryKey, int subKey);
void LuaRegistrySetSubTableEntry(LuaState L, const char* registryKey, int subKey, int valueIndex);
/**
 * @brief Creates a reference in the lua registry.  It will use what is at the top of the stack.  It doesn't modify the stack
 *
 * @param L Lua state
 * @return int the ref
 */
int LuaCreateRefInLuaRegistry(LuaState L, int i);
void LuaPushRefValueInLuaRegistry(LuaState L, int refInt);
// Functions
void LuaGetLuaFuncAtIndex(LuaState L, int index);
void LuaGetLuaFunc(LuaState L, const char* field);
void RunLuaFunctionOnStack(LuaState L, int numArgs);
// Helpers
typedef enum LuaFunctionParameterTypes {
	// Used if you don't want to check this type.
	LuaFunctionParameterTypeDefault,
	LuaFunctionParameterTypePass,
	LuaFunctionParameterTypeInt,
	LuaFunctionParameterTypeNumber,
	LuaFunctionParameterTypeString,
	LuaFunctionParameterTypeFunction,
	LuaFUnctionParameterTypeUserdata,
	LuaFunctionParameterTypeTable,
	LuaFunctionParameterTypeBoolean,

} LuaFunctionParameterTypes;
// Checks to see if the stack has the specific number of arguments, with the following params.. use TypePass to skip checking that arg.
int LuaCheckFunctionCallParamsAndTypes(LuaState L, int numArgsOnStack, ...);
// Return string value of type, useful for debuggin
const char* LuaGetParamType(LuaFunctionParameterTypes paramType);
// Lua type at position i on stack
const char* LuaGetTypeStringi(LuaState L, int pos);

#ifdef __cplusplus
}
#endif
