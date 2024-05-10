#pragma once
#include "../Component/ZCamera.h"
#include "../Math/Vector2.h"
#include "../PhysZ/Ray.h"

extern "C"
{
#include "../External/Lua/lua.h"
#include "../External/Lua/lualib.h"
#include "../External/Lua/lauxlib.h"
}

static int Camera_WorldToScreenPoint(lua_State* L)
{
	ZXEngine::Vector2 screenPos;

	int argc = lua_gettop(L);
	if (argc == 2)
	{
		ZXEngine::Camera** camera = (ZXEngine::Camera**)luaL_checkudata(L, -2, "ZXEngine.Camera");

		lua_getfield(L, -1, "x");
		float x = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, -1, "y");
		float y = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, -1, "z");
		float z = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);

		ZXEngine::Vector3 pos = { x, y, z };

		screenPos = (*camera)->WorldToScreenPoint(pos);
	}
	else if (argc == 4)
	{
		ZXEngine::Camera** camera = (ZXEngine::Camera**)luaL_checkudata(L, -4, "ZXEngine.Camera");

		float x = (float)luaL_checknumber(L, -3);
		float y = (float)luaL_checknumber(L, -2);
		float z = (float)luaL_checknumber(L, -1);

		ZXEngine::Vector3 pos = { x, y, z };

		screenPos = (*camera)->WorldToScreenPoint(pos);
	}
	else
	{
		luaL_error(L, "Wrong number of arguments in 'WorldToScreenPoint' function!");
		return 0;
	}

	lua_newtable(L);
	lua_pushnumber(L, screenPos.x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, screenPos.y);
	lua_setfield(L, -2, "y");

	return 1;
}

static int Camera_ScreenPointToRay(lua_State* L)
{
	ZXEngine::Camera** camera = (ZXEngine::Camera**)luaL_checkudata(L, -2, "ZXEngine.Camera");

	lua_getfield(L, -1, "x");
	float x = (float)luaL_checknumber(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, -1, "y");
	float y = (float)luaL_checknumber(L, -1);
	lua_pop(L, 1);

	ZXEngine::Vector2 pos = { x, y };

	ZXEngine::PhysZ::Ray ray = (*camera)->ScreenPointToRay(pos);

	ZXEngine::PhysZ::Ray* pRay = new ZXEngine::PhysZ::Ray(ray);

	size_t nbytes = sizeof(ZXEngine::PhysZ::Ray);
	ZXEngine::PhysZ::Ray** t = (ZXEngine::PhysZ::Ray**)lua_newuserdata(L, nbytes);
	*t = pRay;
	luaL_getmetatable(L, "ZXEngine.Ray");
	lua_setmetatable(L, -2);

	return 1;
}

static const luaL_Reg Camera_Funcs[] =
{
	{ NULL, NULL }
};

static const luaL_Reg Camera_Funcs_Meta[] =
{
	{ "WorldToScreenPoint", Camera_WorldToScreenPoint },
	{ "ScreenPointToRay",   Camera_ScreenPointToRay   },
	{ NULL, NULL }
};

LUAMOD_API int luaopen_Camera(lua_State* L)
{
	luaL_newmetatable(L, "ZXEngine.Camera");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, Camera_Funcs_Meta, 0);

	luaL_newlib(L, Camera_Funcs);
	return 1;
}