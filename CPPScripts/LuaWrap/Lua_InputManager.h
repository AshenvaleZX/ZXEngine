#pragma once
#include "../Input/InputManager.h"

extern "C"
{
#include "../Lua/lua.h"
#include "../Lua/lualib.h"
#include "../Lua/lauxlib.h"
}

static int ShowCursor(lua_State* L)
{
	bool show = lua_toboolean(L, -1);
	ZXEngine::InputManager::GetInstance()->ShowCursor(show);
	return 0;
}

static const luaL_Reg InputManager_Funcs[] = {
	{"ShowCursor", ShowCursor},
	{NULL, NULL}
};

LUAMOD_API int luaopen_InputManager(lua_State* L) {
	luaL_newlib(L, InputManager_Funcs);
	return 1;
}