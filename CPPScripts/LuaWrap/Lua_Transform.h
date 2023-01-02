#pragma once
#include "../Transform.h"

extern "C"
{
#include "../Lua/lua.h"
#include "../Lua/lualib.h"
#include "../Lua/lauxlib.h"
}

static int SetPosition(lua_State* L)
{
	// ����Lua��self����(��Transform)����Ϊ�ǵ�һ�������������ǵ�һ����ջ�ģ����滹��3�������������������-4
	ZXEngine::Transform** data = (ZXEngine::Transform**)luaL_checkudata(L, -4, "ZXEngine.Transform");
	float x = (float)luaL_checknumber(L, -3);
	float y = (float)luaL_checknumber(L, -2);
	float z = (float)luaL_checknumber(L, -1);
	(*data)->SetPosition(x, y, z);
	return 0;
}

static int SetEulerAngles(lua_State* L)
{
	ZXEngine::Transform** data = (ZXEngine::Transform**)luaL_checkudata(L, -4, "ZXEngine.Transform");
	float x = (float)luaL_checknumber(L, -3);
	float y = (float)luaL_checknumber(L, -2);
	float z = (float)luaL_checknumber(L, -1);
	(*data)->SetEulerAngles(x, y, z);
	return 0;
}

static int GetEulerAngles(lua_State* L)
{
	ZXEngine::Transform** data = (ZXEngine::Transform**)luaL_checkudata(L, -1, "ZXEngine.Transform");

	ZXEngine::Vector3 eulerAngles = (*data)->GetEulerAngles();
	lua_newtable(L);
	lua_pushnumber(L, eulerAngles.x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, eulerAngles.y);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L, eulerAngles.z);
	lua_setfield(L, -2, "z");

	return 1;
}

static int GetPosition(lua_State* L)
{
	ZXEngine::Transform** data = (ZXEngine::Transform**)luaL_checkudata(L, -1, "ZXEngine.Transform");

	ZXEngine::Vector3 position = (*data)->GetPosition();
	lua_newtable(L);
	lua_pushnumber(L, position.x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, position.y);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L, position.z);
	lua_setfield(L, -2, "z");

	return 1;
}

static int GetForward(lua_State* L)
{
	ZXEngine::Transform** data = (ZXEngine::Transform**)luaL_checkudata(L, -1, "ZXEngine.Transform");

	ZXEngine::Vector3 forward = (*data)->GetForward();
	lua_newtable(L);
	lua_pushnumber(L, forward.x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, forward.y);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L, forward.z);
	lua_setfield(L, -2, "z");

	return 1;
}

static int GetRight(lua_State* L)
{
	ZXEngine::Transform** data = (ZXEngine::Transform**)luaL_checkudata(L, -1, "ZXEngine.Transform");

	ZXEngine::Vector3 right = (*data)->GetRight();
	lua_newtable(L);
	lua_pushnumber(L, right.x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, right.y);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L, right.z);
	lua_setfield(L, -2, "z");

	return 1;
}

static int GetUp(lua_State* L)
{
	ZXEngine::Transform** data = (ZXEngine::Transform**)luaL_checkudata(L, -1, "ZXEngine.Transform");

	ZXEngine::Vector3 up = (*data)->GetUp();
	lua_newtable(L);
	lua_pushnumber(L, up.x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, up.y);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L, up.z);
	lua_setfield(L, -2, "z");

	return 1;
}

static const luaL_Reg Transform_Funcs[] = {
	{NULL, NULL}
};

static const luaL_Reg Transform_Funcs_Meta[] = {
	{"SetPosition", SetPosition},
	{"SetEulerAngles", SetEulerAngles},
	{"GetEulerAngles", GetEulerAngles},
	{"GetPosition", GetPosition},
	{"GetForward", GetForward},
	{"GetRight", GetRight},
	{"GetUp", GetUp},
	{NULL, NULL}
};

LUAMOD_API int luaopen_Transform(lua_State* L) {
	luaL_newmetatable(L, "ZXEngine.Transform"); /* ����Ԫ�� */
	lua_pushvalue(L, -1); /* ����Ԫ�������Ϊ��һ�д������׼������ */
	lua_setfield(L, -2, "__index"); /* mt.__index = mt ��-2λ���ϵ�mt��(��һ��newmetatable��)��__index�ֶ�����Ϊmt(�ڶ���pushvalue���Ƶ�)*/
	luaL_setfuncs(L, Transform_Funcs_Meta, 0); /* ע��Ԫ���� */

	luaL_newlib(L, Transform_Funcs);
	return 1;
}