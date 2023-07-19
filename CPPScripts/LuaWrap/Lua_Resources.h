#pragma once
#include "../Resources.h"

extern "C"
{
#include "../External/Lua/lua.h"
#include "../External/Lua/lualib.h"
#include "../External/Lua/lauxlib.h"
}

static int GetAssetsPath(lua_State* L)
{
	lua_pushstring(L, ZXEngine::Resources::GetAssetsPath().c_str());
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