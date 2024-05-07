#pragma once
#include "../Component/GameLogic.h"

extern "C"
{
#include "../External/Lua/lua.h"
#include "../External/Lua/lualib.h"
#include "../External/Lua/lauxlib.h"
}

static int GameLogic_GetScript(lua_State* L)
{
	ZXEngine::GameLogic** self = (ZXEngine::GameLogic**)luaL_checkudata(L, -1, "ZXEngine.GameLogic");

	lua_getglobal(L, "AllGameLogic");
	lua_pushnumber(L, (*self)->luaID);
	lua_gettable(L, -2);

	return 1;
}

static const luaL_Reg GameLogic_Funcs[] =
{
	{ NULL, NULL }
};

static const luaL_Reg GameLogic_Funcs_Meta[] =
{
	{ "GetScript", GameLogic_GetScript },
	{ NULL, NULL }
};

LUAMOD_API int luaopen_GameLogic(lua_State* L)
{
	luaL_newmetatable(L, "ZXEngine.GameLogic");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, GameLogic_Funcs_Meta, 0);

	luaL_newlib(L, GameLogic_Funcs);
	return 1;
}