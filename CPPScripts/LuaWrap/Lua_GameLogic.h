#pragma once
#include "../GameLogic.h"
#include "../Transform.h"

extern "C"
{
#include "../Lua/lua.h"
#include "../Lua/lualib.h"
#include "../Lua/lauxlib.h"
}

static int GetTransform(lua_State* L)
{
	// 获取Lua那边的self(即直接绑定GameObject的table)
	ZXEngine::GameLogic** data = (ZXEngine::GameLogic**)luaL_checkudata(L, -1, "ZXEngine.GameLogic");

	// 创建一个指向Transform*的指针作为返回值，同时入栈
	size_t nbytes = sizeof(ZXEngine::Transform);
	ZXEngine::Transform** t = (ZXEngine::Transform**)lua_newuserdata(L, nbytes);
	
	// 给刚刚创建的指针赋值
	*t = (*data)->GetTransform();

	// 设置一下meta table，这样返回给Lua的指针才能继续在Lua那边调用C++引擎接口
	luaL_getmetatable(L, "ZXEngine.Transform");
	lua_setmetatable(L, -2);

	return 1;
}

static const luaL_Reg GameLogic_Funcs[] = {
	{NULL, NULL}
};

static const luaL_Reg GameLogic_Funcs_Meta[] = {
	{"GetTransform", GetTransform},
	{NULL, NULL}
};

LUAMOD_API int luaopen_GameLogic(lua_State* L) {
	luaL_newmetatable(L, "ZXEngine.GameLogic"); /* 创建元表 */
	lua_pushvalue(L, -1); /* 复制元表，这个是为下一行代码调用准备参数 */
	lua_setfield(L, -2, "__index"); /* mt.__index = mt 把-2位置上的mt表(第一行newmetatable的)的__index字段设置为mt(第二行pushvalue复制的)*/
	luaL_setfuncs(L, GameLogic_Funcs_Meta, 0); /* 注册元方法 */

	luaL_newlib(L, GameLogic_Funcs);
	return 1;
}