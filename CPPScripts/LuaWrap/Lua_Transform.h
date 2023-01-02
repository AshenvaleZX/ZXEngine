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
	// 接收Lua的self参数(即Transform)，因为是第一个参数，所以是第一个入栈的，后面还有3个参数，这个参数就在-4
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
	luaL_newmetatable(L, "ZXEngine.Transform"); /* 创建元表 */
	lua_pushvalue(L, -1); /* 复制元表，这个是为下一行代码调用准备参数 */
	lua_setfield(L, -2, "__index"); /* mt.__index = mt 把-2位置上的mt表(第一行newmetatable的)的__index字段设置为mt(第二行pushvalue复制的)*/
	luaL_setfuncs(L, Transform_Funcs_Meta, 0); /* 注册元方法 */

	luaL_newlib(L, Transform_Funcs);
	return 1;
}