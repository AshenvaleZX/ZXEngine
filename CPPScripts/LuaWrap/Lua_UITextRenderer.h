#pragma once
#include "../Component/UITextRenderer.h"

extern "C"
{
#include "../External/Lua/lua.h"
#include "../External/Lua/lualib.h"
#include "../External/Lua/lauxlib.h"
}

static int UITextRenderer_SetText(lua_State* L)
{
	ZXEngine::UITextRenderer** data = (ZXEngine::UITextRenderer**)luaL_checkudata(L, -2, "ZXEngine.UITextRenderer");
	string text = lua_tostring(L, -1);
	(*data)->SetContent(text);
	return 0;
}

static const luaL_Reg UITextRenderer_Funcs[] = 
{
	{ NULL, NULL }
};

static const luaL_Reg UITextRenderer_Funcs_Meta[] = 
{
	{ "SetText", UITextRenderer_SetText },
	{ NULL, NULL }
};

LUAMOD_API int luaopen_UITextRenderer(lua_State* L) 
{
	luaL_newmetatable(L, "ZXEngine.UITextRenderer");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, UITextRenderer_Funcs_Meta, 0);

	luaL_newlib(L, UITextRenderer_Funcs);
	return 1;
}