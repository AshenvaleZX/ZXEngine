#pragma once
#include "../Component/Animator.h"

extern "C"
{
#include "../External/Lua/lua.h"
#include "../External/Lua/lualib.h"
#include "../External/Lua/lauxlib.h"
}

static int Play(lua_State* L)
{
	ZXEngine::Animator** animator = (ZXEngine::Animator**)luaL_checkudata(L, -2, "ZXEngine.Animator");

	const char* animationName = lua_tostring(L, -1);

	(*animator)->Play(animationName);

	return 0;
}

static const luaL_Reg Animator_Funcs[] = {
	{NULL, NULL}
};

static const luaL_Reg Animator_Funcs_Meta[] = {
	{"Play", Play},
	{NULL, NULL}
};

LUAMOD_API int luaopen_Animator(lua_State* L) {
	luaL_newmetatable(L, "ZXEngine.Animator"); /* 创建元表 */
	lua_pushvalue(L, -1); /* 复制元表，这个是为下一行代码调用准备参数 */
	lua_setfield(L, -2, "__index"); /* mt.__index = mt 把-2位置上的mt表(第一行newmetatable的)的__index字段设置为mt(第二行pushvalue复制的)*/
	luaL_setfuncs(L, Animator_Funcs_Meta, 0); /* 注册元方法 */

	luaL_newlib(L, Animator_Funcs);
	return 1;
}