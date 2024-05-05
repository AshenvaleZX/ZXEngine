#pragma once
#include "../PhysZ/Ray.h"

extern "C"
{
#include "../External/Lua/lua.h"
#include "../External/Lua/lualib.h"
#include "../External/Lua/lauxlib.h"
}

static const luaL_Reg Ray_Funcs[] =
{
	{ NULL, NULL }
};

static const luaL_Reg Ray_Funcs_Meta[] =
{
	{ NULL, NULL }
};

LUAMOD_API int luaopen_Ray(lua_State* L)
{
	luaL_newmetatable(L, "ZXEngine.Ray");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, Ray_Funcs_Meta, 0);

	luaL_newlib(L, Ray_Funcs);
	return 1;
}