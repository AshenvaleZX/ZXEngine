#pragma once
#include "../Time.h"

extern "C"
{
#include "../External/Lua/lua.h"
#include "../External/Lua/lualib.h"
#include "../External/Lua/lauxlib.h"
}

static int GetDeltaTime(lua_State* L)
{
	lua_pushnumber(L, ZXEngine::Time::deltaTime);
	return 1;
}

static int GetFixedDeltaTime(lua_State* L)
{
	lua_pushnumber(L, ZXEngine::Time::fixedDeltaTime);
	return 1;
}

static const luaL_Reg Time_Funcs[] = {
	{"GetDeltaTime", GetDeltaTime},
	{"GetFixedDeltaTime", GetFixedDeltaTime},
	{NULL, NULL}
};

LUAMOD_API int luaopen_Time(lua_State* L) {
	luaL_newlib(L, Time_Funcs);
	return 1;
}