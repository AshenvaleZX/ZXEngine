#pragma once
#include "../Component/UIButton.h"

extern "C"
{
#include "../External/Lua/lua.h"
#include "../External/Lua/lualib.h"
#include "../External/Lua/lauxlib.h"
}

static int UIButton_SetClickCallBack(lua_State* L)
{
	ZXEngine::UIButton** self = (ZXEngine::UIButton**)luaL_checkudata(L, -3, "ZXEngine.UIButton");

	(*self)->UnregisterCallBack();

	// -1: table
	// -2: function
	// luaL_checktype检查后会弹出，所以反着来，先检查第二个参数，再检查第一个参数
	luaL_checktype(L, -1, LUA_TTABLE);
	(*self)->mClickCallBack.table = luaL_ref(L, LUA_REGISTRYINDEX);

	luaL_checktype(L, -1, LUA_TFUNCTION);
	(*self)->mClickCallBack.func = luaL_ref(L, LUA_REGISTRYINDEX);

	return 0;
}

static const luaL_Reg UIButton_Funcs[] =
{
	{ NULL, NULL }
};

static const luaL_Reg UIButton_Funcs_Meta[] =
{
	{ "SetClickCallBack", UIButton_SetClickCallBack },
	{ NULL, NULL }
};

LUAMOD_API int luaopen_UIButton(lua_State* L)
{
	luaL_newmetatable(L, "ZXEngine.UIButton");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, UIButton_Funcs_Meta, 0);

	luaL_newlib(L, UIButton_Funcs);
	return 1;
}