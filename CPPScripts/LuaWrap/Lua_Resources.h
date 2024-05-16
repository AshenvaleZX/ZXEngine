#pragma once
#include "../Resources.h"

extern "C"
{
#include "../External/Lua/lua.h"
#include "../External/Lua/lualib.h"
#include "../External/Lua/lauxlib.h"
}

static int Resources_GetAssetsPath(lua_State* L)
{
	lua_pushstring(L, ZXEngine::Resources::GetAssetsPath().c_str());
	return 1;
}

static int Resources_LoadPrefab(lua_State* L)
{
	string path = lua_tostring(L, -1);
	ZXEngine::PrefabStruct* prefab = ZXEngine::Resources::LoadPrefab(path);

	if (prefab == nullptr)
	{
		lua_pushnil(L);
		return 1;
	}

	lua_pushlightuserdata(L, prefab);
	return 1;
}

static int Resources_ReleasePrefab(lua_State* L)
{
	ZXEngine::PrefabStruct* prefab = (ZXEngine::PrefabStruct*)lua_touserdata(L, -1);
	delete prefab;
	return 0;
}

static const luaL_Reg Resources_Funcs[] = 
{
	{ "GetAssetsPath", Resources_GetAssetsPath },
	{ "LoadPrefab",    Resources_LoadPrefab    },
	{ "ReleasePrefab", Resources_ReleasePrefab },
	{ NULL, NULL }
};

LUAMOD_API int luaopen_Resources(lua_State* L) 
{
	luaL_newlib(L, Resources_Funcs);
	return 1;
}