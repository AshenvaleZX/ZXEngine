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
	luaL_newmetatable(L, "ZXEngine.UITextRenderer"); /* 创建元表 */
	lua_pushvalue(L, -1); /* 复制元表，这个是为下一行代码调用准备参数 */
	lua_setfield(L, -2, "__index"); /* mt.__index = mt 把-2位置上的mt表(第一行newmetatable的)的__index字段设置为mt(第二行pushvalue复制的)*/
	luaL_setfuncs(L, UITextRenderer_Funcs_Meta, 0); /* 注册元方法 */

	luaL_newlib(L, UITextRenderer_Funcs);
	return 1;
}