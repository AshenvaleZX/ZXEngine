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
	else if (strcmp(key, "Platform") == 0)
	{
#if defined(ZX_PLATFORM_ANDROID)
		lua_pushinteger(L, (int)ZXEngine::PlatformType::Android);
#elif defined(ZX_PLATFORM_LINUX)
		lua_pushinteger(L, (int)ZXEngine::PlatformType::Linux);
#elif defined(ZX_PLATFORM_MACOS)
		lua_pushinteger(L, (int)ZXEngine::PlatformType::MacOS);
#elif defined(ZX_PLATFORM_WINDOWS)
		lua_pushinteger(L, (int)ZXEngine::PlatformType::Windows);
#endif
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