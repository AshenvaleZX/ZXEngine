#pragma once
#include "../Component/MeshRenderer.h"
#include "../Material.h"

extern "C"
{
#include "../External/Lua/lua.h"
#include "../External/Lua/lualib.h"
#include "../External/Lua/lauxlib.h"
}

static int MeshRenderer_SetEnable(lua_State* L)
{
	ZXEngine::MeshRenderer** self = (ZXEngine::MeshRenderer**)luaL_checkudata(L, -1, "ZXEngine.MeshRenderer");

	bool enable = lua_toboolean(L, -2);
	(*self)->mEnabled = enable;

	return 0;
}

static int MeshRenderer_GetMaterial(lua_State* L)
{
	ZXEngine::MeshRenderer** self = (ZXEngine::MeshRenderer**)luaL_checkudata(L, -1, "ZXEngine.MeshRenderer");
	ZXEngine::Material* mat = (*self)->mMatetrial;

	size_t nbytes = sizeof(ZXEngine::Material);
	ZXEngine::Material** t = (ZXEngine::Material**)lua_newuserdata(L, nbytes);
	*t = mat;
	luaL_getmetatable(L, "ZXEngine.Material");
	lua_setmetatable(L, -2);
	
	return 1;
}

static const luaL_Reg MeshRenderer_Funcs[] =
{
	{ NULL, NULL }
};

static const luaL_Reg MeshRenderer_Funcs_Meta[] =
{
	{ "SetEnable",   MeshRenderer_SetEnable   },
	{ "GetMaterial", MeshRenderer_GetMaterial },
	{ NULL, NULL }
};

LUAMOD_API int luaopen_MeshRenderer(lua_State* L)
{
	luaL_newmetatable(L, "ZXEngine.MeshRenderer");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, MeshRenderer_Funcs_Meta, 0);

	luaL_newlib(L, MeshRenderer_Funcs);
	return 1;
}