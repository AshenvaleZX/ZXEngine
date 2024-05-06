#pragma once
#include "../Component/Physics/Collider.h"

extern "C"
{
#include "../External/Lua/lua.h"
#include "../External/Lua/lualib.h"
#include "../External/Lua/lauxlib.h"
}

static int Collider_IntersectRay(lua_State* L)
{
	ZXEngine::Collider** collider = (ZXEngine::Collider**)luaL_checkudata(L, -2, "ZXEngine.Collider");

	ZXEngine::PhysZ::Ray** ray = (ZXEngine::PhysZ::Ray**)luaL_checkudata(L, -1, "ZXEngine.Ray");

	bool res = (*collider)->IntersectRay(**ray);

	lua_pushboolean(L, res);

	return 1;
}

static const luaL_Reg Collider_Funcs[] =
{
	{ NULL, NULL }
};

static const luaL_Reg Collider_Funcs_Meta[] =
{
	{ "IntersectRay", Collider_IntersectRay },
	{ NULL, NULL }
};

LUAMOD_API int luaopen_Collider(lua_State* L)
{
	luaL_newmetatable(L, "ZXEngine.Collider");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, Collider_Funcs_Meta, 0);

	luaL_newlib(L, Collider_Funcs);
	return 1;
}