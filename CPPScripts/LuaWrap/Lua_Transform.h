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
	(*data)->position = vec3(x,y,z);
	return 0;
}

static int SetEulerAngles(lua_State* L)
{
	ZXEngine::Transform** data = (ZXEngine::Transform**)luaL_checkudata(L, -4, "ZXEngine.Transform");
	float x = (float)luaL_checknumber(L, -3);
	float y = (float)luaL_checknumber(L, -2);
	float z = (float)luaL_checknumber(L, -1);
	(*data)->rotation.SetEulerAngles(x, y, z);
	return 0;
}

static int GetPosition(lua_State* L)
{
	ZXEngine::Transform** data = (ZXEngine::Transform**)luaL_checkudata(L, -1, "ZXEngine.Transform");

	lua_newtable(L);
	lua_pushnumber(L, (*data)->position.x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, (*data)->position.y);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L, (*data)->position.z);
	lua_setfield(L, -2, "z");

	return 1;
}

static int GetForward(lua_State* L)
{
	ZXEngine::Transform** data = (ZXEngine::Transform**)luaL_checkudata(L, -1, "ZXEngine.Transform");

	lua_newtable(L);
	lua_pushnumber(L, (*data)->GetForward().x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, (*data)->GetForward().y);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L, (*data)->GetForward().z);
	lua_setfield(L, -2, "z");

	return 1;
}

static int GetRight(lua_State* L)
{
	ZXEngine::Transform** data = (ZXEngine::Transform**)luaL_checkudata(L, -1, "ZXEngine.Transform");

	lua_newtable(L);
	lua_pushnumber(L, (*data)->GetRight().x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, (*data)->GetRight().y);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L, (*data)->GetRight().z);
	lua_setfield(L, -2, "z");

	return 1;
}

static int GetUp(lua_State* L)
{
	ZXEngine::Transform** data = (ZXEngine::Transform**)luaL_checkudata(L, -1, "ZXEngine.Transform");

	lua_newtable(L);
	lua_pushnumber(L, (*data)->GetUp().x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, (*data)->GetUp().y);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L, (*data)->GetUp().z);
	lua_setfield(L, -2, "z");

	return 1;
}

static const luaL_Reg Transform_Funcs[] = {
	{NULL, NULL}
};

static const luaL_Reg Transform_Funcs_Meta[] = {
	{"SetPosition", SetPosition},
	{"SetEulerAngles", SetEulerAngles},
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