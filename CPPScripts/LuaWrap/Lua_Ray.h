#pragma once
#include "../PhysZ/Ray.h"

extern "C"
{
#include "../External/Lua/lua.h"
#include "../External/Lua/lualib.h"
#include "../External/Lua/lauxlib.h"
}

static int Ray_GetOrigin(lua_State* L)
{
	ZXEngine::PhysZ::Ray** ray = (ZXEngine::PhysZ::Ray**)luaL_checkudata(L, -1, "ZXEngine.Ray");

	const ZXEngine::Vector3& data = (*ray)->mOrigin;
	lua_newtable(L);
	lua_pushnumber(L, data.x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, data.y);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L, data.z);
	lua_setfield(L, -2, "z");

	return 1;
}

static int Ray_GetDirection(lua_State* L)
{
	ZXEngine::PhysZ::Ray** ray = (ZXEngine::PhysZ::Ray**)luaL_checkudata(L, -1, "ZXEngine.Ray");

	const ZXEngine::Vector3& data = (*ray)->mDirection;
	lua_newtable(L);
	lua_pushnumber(L, data.x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, data.y);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L, data.z);
	lua_setfield(L, -2, "z");

	return 1;
}

static const luaL_Reg Ray_Funcs[] =
{
	{ NULL, NULL }
};

static const luaL_Reg Ray_Funcs_Meta[] =
{
	{ "GetOrigin",    Ray_GetOrigin    },
	{ "GetDirection", Ray_GetDirection },
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