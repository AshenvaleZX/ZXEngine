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
	// ��ȡLua�Ǳߵ�self(��ֱ�Ӱ�GameObject��table)
	ZXEngine::GameLogic** data = (ZXEngine::GameLogic**)luaL_checkudata(L, -1, "ZXEngine.GameLogic");

	// ����һ��ָ��Transform*��ָ����Ϊ����ֵ��ͬʱ��ջ
	size_t nbytes = sizeof(ZXEngine::Transform);
	ZXEngine::Transform** t = (ZXEngine::Transform**)lua_newuserdata(L, nbytes);
	
	// ���ոմ�����ָ�븳ֵ
	*t = (*data)->GetTransform();

	// ����һ��meta table���������ظ�Lua��ָ����ܼ�����Lua�Ǳߵ���C++����ӿ�
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
	luaL_newmetatable(L, "ZXEngine.GameLogic"); /* ����Ԫ�� */
	lua_pushvalue(L, -1); /* ����Ԫ�������Ϊ��һ�д������׼������ */
	lua_setfield(L, -2, "__index"); /* mt.__index = mt ��-2λ���ϵ�mt��(��һ��newmetatable��)��__index�ֶ�����Ϊmt(�ڶ���pushvalue���Ƶ�)*/
	luaL_setfuncs(L, GameLogic_Funcs_Meta, 0); /* ע��Ԫ���� */

	luaL_newlib(L, GameLogic_Funcs);
	return 1;
}