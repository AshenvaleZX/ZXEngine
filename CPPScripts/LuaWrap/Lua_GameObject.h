#pragma once
#include "../Debug.h"
#include "../GameObject.h"

extern "C"
{
#include "../External/Lua/lua.h"
#include "../External/Lua/lualib.h"
#include "../External/Lua/lauxlib.h"
}

static int GetComponent(lua_State* L)
{
	ZXEngine::GameObject** data = (ZXEngine::GameObject**)luaL_checkudata(L, -2, "ZXEngine.GameObject");
	string type = lua_tostring(L, -1);

	if (type == "UITextRenderer")
	{
		size_t nbytes = sizeof(ZXEngine::UITextRenderer);
		ZXEngine::UITextRenderer** t = (ZXEngine::UITextRenderer**)lua_newuserdata(L, nbytes);
		*t = (*data)->GetComponent<ZXEngine::UITextRenderer>();
		luaL_getmetatable(L, "ZXEngine.UITextRenderer");
		lua_setmetatable(L, -2);
	}
	else if (type == "Transform")
	{
		size_t nbytes = sizeof(ZXEngine::Transform);
		ZXEngine::Transform** t = (ZXEngine::Transform**)lua_newuserdata(L, nbytes);
		*t = (*data)->GetComponent<ZXEngine::Transform>();
		luaL_getmetatable(L, "ZXEngine.Transform");
		lua_setmetatable(L, -2);
	}
	else if (type == "Animator")
	{
		size_t nbytes = sizeof(ZXEngine::Animator);
		ZXEngine::Animator** t = (ZXEngine::Animator**)lua_newuserdata(L, nbytes);
		*t = (*data)->GetComponent<ZXEngine::Animator>();
		luaL_getmetatable(L, "ZXEngine.Animator");
		lua_setmetatable(L, -2);
	}
	else
	{ 
		ZXEngine::Debug::LogError("Not find lua wrap component: " + type);
	}

	return 1;
}

static const luaL_Reg GameObject_Funcs[] = {
	{NULL, NULL}
};

static const luaL_Reg GameObject_Funcs_Meta[] = {
	{"GetComponent", GetComponent},
	{NULL, NULL}
};

LUAMOD_API int luaopen_GameObject(lua_State* L) {
	luaL_newmetatable(L, "ZXEngine.GameObject"); /* ����Ԫ�� */
	lua_pushvalue(L, -1); /* ����Ԫ�������Ϊ��һ�д������׼������ */
	lua_setfield(L, -2, "__index"); /* mt.__index = mt ��-2λ���ϵ�mt��(��һ��newmetatable��)��__index�ֶ�����Ϊmt(�ڶ���pushvalue���Ƶ�)*/
	luaL_setfuncs(L, GameObject_Funcs_Meta, 0); /* ע��Ԫ���� */

	luaL_newlib(L, GameObject_Funcs);
	return 1;
}