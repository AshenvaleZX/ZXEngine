#pragma once
#include "../Time.h"

extern "C"
{
#include "../Lua/lua.h"
#include "../Lua/lualib.h"
#include "../Lua/lauxlib.h"
}

static int GetDeltaTime(lua_State* L)
{
	lua_pushnumber(L, ZXEngine::Time::deltaTime);
	return 1;
}

static const luaL_Reg Time_Funcs[] = {
	{"GetDeltaTime", GetDeltaTime},
	{NULL, NULL}
};

LUAMOD_API int luaopen_Time(lua_State* L) {
	luaL_newlib(L, Time_Funcs);
	return 1;
}