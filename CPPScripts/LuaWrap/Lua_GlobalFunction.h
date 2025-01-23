#pragma once
#include "../LuaManager.h"

extern "C"
{
#include "../External/Lua/lua.h"
#include "../External/Lua/lualib.h"
#include "../External/Lua/lauxlib.h"
}

static int GlobalFunction_CustomRequire(lua_State* L)
{
	const char* modname = lua_tostring(L, -1);
	ZXEngine::LuaManager::GetInstance()->CustomRequire(modname);
	return 0;
}

static const luaL_Reg GlobalFunction[] =
{
	{ "CustomRequire", GlobalFunction_CustomRequire },
	{ NULL, NULL }
};

LUAMOD_API int luaopen_GlobalFunction(lua_State* L)
{
	for (const luaL_Reg* func = GlobalFunction; func->name != NULL; ++func)
	{
		lua_register(L, func->name, func->func);
	}
	return 1;
}