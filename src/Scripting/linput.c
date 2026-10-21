#include <Supergoon/Input/mouse.h>
#include <Supergoon/Input/touch.h>
#include <Supergoon/Scripting/linput.h>
#include <Supergoon/lua.h>
#include <sgtools/log.h>

static int isValidMouseButton(LuaState L) {
	if (!LuaCheckFunctionCallParamsAndTypes(L, 1, LuaFunctionParameterTypeInt)) {
		return 0;
	}
	int button = LuaGetIntFromStacki(L, 1);
	return button >= MouseButtonsLeftClick && button <= MouseButtonsMiddleClick;
}

// button
static int isMouseButtonDown(LuaState L) {
	if (!isValidMouseButton(L)) {
		sgLogWarn("Bad params for is mouse button down");
		LuaPushBool(L, 0);
		return 1;
	}
	LuaPushBool(L, IsMouseButtonDown(LuaGetIntFromStacki(L, 1)));
	return 1;
}

// button
static int isMouseButtonJustPressed(LuaState L) {
	if (!isValidMouseButton(L)) {
		sgLogWarn("Bad params for is mouse button just pressed");
		LuaPushBool(L, 0);
		return 1;
	}
	LuaPushBool(L, IsMouseButtonJustPressed(LuaGetIntFromStacki(L, 1)));
	return 1;
}

// button
static int isMouseButtonJustReleased(LuaState L) {
	if (!isValidMouseButton(L)) {
		sgLogWarn("Bad params for is mouse button just released");
		LuaPushBool(L, 0);
		return 1;
	}
	LuaPushBool(L, IsMouseButtonJustReleased(LuaGetIntFromStacki(L, 1)));
	return 1;
}

static int isTouchDown(LuaState L) {
	LuaPushBool(L, IsTouchDown());
	return 1;
}

static int isTouchJustPressed(LuaState L) {
	LuaPushBool(L, IsTouchJustPressed());
	return 1;
}

static int isTouchJustReleased(LuaState L) {
	LuaPushBool(L, IsTouchJustReleased());
	return 1;
}

static int isTouchTapped(LuaState L) {
	LuaPushBool(L, IsTouchTapped());
	return 1;
}

static int isTouchTwoFingerTapped(LuaState L) {
	LuaPushBool(L, IsTouchTwoFingerTapped());
	return 1;
}

// returns x, y
static int getTouchPos(LuaState L) {
	float x, y;
	GetGameTouchPos(&x, &y);
	LuaPushFloat(L, x);
	LuaPushFloat(L, y);
	return 2;
}

// static int setWindow(LuaState L) {
// 	if (!LuaCheckFunctionCallParamsAndTypes(L, 3, LuaFunctionParameterTypeInt, LuaFunctionParameterTypeInt, LuaFunctionParameterTypeString)) {
// 		sgLogWarn("Bad params for set window");
// 		return 0;
// 	}
// 	SetWindowOptions(LuaGetIntFromStacki(L, 1), LuaGetIntFromStacki(L, 2), LuaGetStringi(L, 3));
// 	return 0;
// }

static const LuaCFuncRegister inputLib[] = {
	// {"SetWindow", setWindow},
	{"IsMouseButtonDown", isMouseButtonDown},
	{"IsMouseButtonJustPressed", isMouseButtonJustPressed},
	{"IsMouseButtonJustReleased", isMouseButtonJustReleased},
	{"IsTouchDown", isTouchDown},
	{"IsTouchJustPressed", isTouchJustPressed},
	{"IsTouchJustReleased", isTouchJustReleased},
	{"IsTouchTapped", isTouchTapped},
	{"IsTouchTwoFingerTapped", isTouchTwoFingerTapped},
	{"GetTouchPos", getTouchPos},
};

void RegisterLuaInputFunctions(void) {
	LuaRegisterFunctionsToLuaLibrary(inputLib, "Input");
}
