#pragma once
#include "../Material.h"

extern "C"
{
#include "../External/Lua/lua.h"
#include "../External/Lua/lualib.h"
#include "../External/Lua/lauxlib.h"
}

static int Material_SetBool(lua_State* L)
{
	ZXEngine::Material** self = (ZXEngine::Material**)luaL_checkudata(L, -3, "ZXEngine.Material");

	string name = lua_tostring(L, -2);
	bool value = lua_toboolean(L, -1);
	(*self)->SetScalar(name, value);

	return 0;
}

static int Material_SetFloat(lua_State* L)
{
	ZXEngine::Material** self = (ZXEngine::Material**)luaL_checkudata(L, -3, "ZXEngine.Material");

	string name = lua_tostring(L, -2);
	float value = (float)lua_tonumber(L, -1);
	(*self)->SetScalar(name, value);

	return 0;
}

static int Material_SetInt(lua_State* L)
{
	ZXEngine::Material** self = (ZXEngine::Material**)luaL_checkudata(L, -3, "ZXEngine.Material");

	string name = lua_tostring(L, -2);
	int value = (int)lua_tointeger(L, -1);
	(*self)->SetScalar(name, value);

	return 0;
}

static int Material_SetVector2(lua_State* L)
{
	int argc = lua_gettop(L);

	if (argc == 3)
	{
		ZXEngine::Material** self = (ZXEngine::Material**)luaL_checkudata(L, -3, "ZXEngine.Material");

		string name = lua_tostring(L, -2);

		ZXEngine::Vector2 value;
		lua_getfield(L, -1, "x");
		value.x = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, -1, "y");
		value.y = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);

		(*self)->SetVector(name, value);
	}
	else if (argc == 4)
	{
		ZXEngine::Material** self = (ZXEngine::Material**)luaL_checkudata(L, -4, "ZXEngine.Material");

		string name = lua_tostring(L, -3);
		ZXEngine::Vector2 value = ZXEngine::Vector2((float)lua_tonumber(L, -2), (float)lua_tonumber(L, -1));

		(*self)->SetVector(name, value);
	}
	else
	{
		luaL_error(L, "Invalid argument count for Material:SetVector2");
	}

	return 0;
}

static int Material_SetVector3(lua_State* L)
{
	int argc = lua_gettop(L);

	if (argc == 3)
	{
		ZXEngine::Material** self = (ZXEngine::Material**)luaL_checkudata(L, -3, "ZXEngine.Material");

		string name = lua_tostring(L, -2);

		ZXEngine::Vector3 value;
		lua_getfield(L, -1, "x");
		value.x = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, -1, "y");
		value.y = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, -1, "z");
		value.z = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);

		(*self)->SetVector(name, value);
	}
	else if (argc == 5)
	{
		ZXEngine::Material** self = (ZXEngine::Material**)luaL_checkudata(L, -5, "ZXEngine.Material");

		string name = lua_tostring(L, -4);
		ZXEngine::Vector3 value = ZXEngine::Vector3((float)lua_tonumber(L, -3), (float)lua_tonumber(L, -2), (float)lua_tonumber(L, -1));

		(*self)->SetVector(name, value);
	}
	else
	{
		luaL_error(L, "Invalid argument count for Material:SetVector3");
	}

	return 0;
}

static int Material_SetVector4(lua_State* L)
{
	int argc = lua_gettop(L);

	if (argc == 3)
	{
		ZXEngine::Material** self = (ZXEngine::Material**)luaL_checkudata(L, -3, "ZXEngine.Material");

		string name = lua_tostring(L, -2);

		ZXEngine::Vector4 value;
		lua_getfield(L, -1, "x");
		value.x = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, -1, "y");
		value.y = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, -1, "z");
		value.z = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, -1, "w");
		value.w = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);

		(*self)->SetVector(name, value);
	}
	else if (argc == 6)
	{
		ZXEngine::Material** self = (ZXEngine::Material**)luaL_checkudata(L, -6, "ZXEngine.Material");

		string name = lua_tostring(L, -5);
		ZXEngine::Vector4 value = ZXEngine::Vector4((float)lua_tonumber(L, -4), (float)lua_tonumber(L, -3), (float)lua_tonumber(L, -2), (float)lua_tonumber(L, -1));

		(*self)->SetVector(name, value);
	}
	else
	{
		luaL_error(L, "Invalid argument count for Material:SetVector4");
	}

	return 0;
}

static const luaL_Reg Material_Funcs[] =
{
	{ NULL, NULL }
};

static const luaL_Reg Material_Funcs_Meta[] =
{
	{ "SetBool",    Material_SetBool    },
	{ "SetFloat",   Material_SetFloat   },
	{ "SetInt",     Material_SetInt     },
	{ "SetVector2", Material_SetVector2 },
	{ "SetVector3", Material_SetVector3 },
	{ "SetVector4", Material_SetVector4 },
	{ NULL, NULL }
};

LUAMOD_API int luaopen_Material(lua_State* L)
{
	luaL_newmetatable(L, "ZXEngine.Material");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, Material_Funcs_Meta, 0);

	luaL_newlib(L, Material_Funcs);
	return 1;
}