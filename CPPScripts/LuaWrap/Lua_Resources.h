#pragma once
#include "../Resources.h"

extern "C"
{
#include "../Lua/lua.h"
#include "../Lua/lualib.h"
#include "../Lua/lauxlib.h"
}

static int GetAssetsPath(lua_State* L)
{
	lua_pushstring(L, ZXEngine::Resources::GetAssetsPath());
	return 1;
}

static const luaL_Reg Resources_Funcs[] = {
	{"GetAssetsPath", GetAssetsPath},
	{NULL, NULL}
};

LUAMOD_API int luaopen_Resources(lua_State* L) {
	luaL_newlib(L, Resources_Funcs);
	return 1;
}