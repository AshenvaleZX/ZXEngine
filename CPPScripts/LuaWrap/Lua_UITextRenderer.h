#pragma once
#include "../UITextRenderer.h"

extern "C"
{
#include "../Lua/lua.h"
#include "../Lua/lualib.h"
#include "../Lua/lauxlib.h"
}

static int SetText(lua_State* L)
{
	ZXEngine::UITextRenderer** data = (ZXEngine::UITextRenderer**)luaL_checkudata(L, -2, "ZXEngine.UITextRenderer");
	string text = lua_tostring(L, -1);
	(*data)->SetContent(text);
	return 0;
}

static const luaL_Reg UITextRenderer_Funcs[] = {
	{NULL, NULL}
};

static const luaL_Reg UITextRenderer_Funcs_Meta[] = {
	{"SetText", SetText},
	{NULL, NULL}
};

LUAMOD_API int luaopen_UITextRenderer(lua_State* L) {
	luaL_newmetatable(L, "ZXEngine.UITextRenderer"); /* ����Ԫ�� */
	lua_pushvalue(L, -1); /* ����Ԫ�������Ϊ��һ�д������׼������ */
	lua_setfield(L, -2, "__index"); /* mt.__index = mt ��-2λ���ϵ�mt��(��һ��newmetatable��)��__index�ֶ�����Ϊmt(�ڶ���pushvalue���Ƶ�)*/
	luaL_setfuncs(L, UITextRenderer_Funcs_Meta, 0); /* ע��Ԫ���� */

	luaL_newlib(L, UITextRenderer_Funcs);
	return 1;
}