#pragma once
#include "../GlobalData.h"

extern "C"
{
#include "../External/Lua/lua.h"
#include "../External/Lua/lualib.h"
#include "../External/Lua/lauxlib.h"
}

static int GetGlobalData(lua_State* L)
{
	const char* key = luaL_checkstring(L, 2);

	if (strcmp(key, "srcWidth") == 0)
	{
		lua_pushnumber(L, ZXEngine::GlobalData::srcWidth);
	}
	else if (strcmp(key, "srcHeight") == 0)
	{
		lua_pushnumber(L, ZXEngine::GlobalData::srcHeight);
	}
	else
	{
		lua_pushnil(L);
	}

	return 1;
}

void luaopen_GlobalData(lua_State* L)
{
	lua_newtable(L);
	lua_newtable(L);
	lua_pushstring(L, "__index");
	lua_pushcfunction(L, GetGlobalData);

	lua_settable(L, -3);
	lua_setmetatable(L, -2);
	lua_setglobal(L, "GlobalData");
}