#pragma once
#include "../Debug.h"

extern "C"
{
#include "../Lua/lua.h"
#include "../Lua/lualib.h"
#include "../Lua/lauxlib.h"
}

static int Log(lua_State* L)
{
	const char* s = lua_tostring(L, -1);
	Debug::Log(s);
	return 0;
}

static int LogWarning(lua_State* L)
{
	const char* s = lua_tostring(L, -1);
	Debug::LogWarning(s);
	return 0;
}

static int LogError(lua_State* L)
{
	const char* s = lua_tostring(L, -1);
	Debug::LogError(s);
	return 0;
}

static const luaL_Reg Debug_Funcs[] = {
	{"Log", Log},
	{"LogWarning",LogWarning},
	{"LogError", LogError},
	{NULL, NULL}
};

LUAMOD_API int luaopen_Debug(lua_State* L) {
	luaL_newlib(L, Debug_Funcs);
	return 1;
}