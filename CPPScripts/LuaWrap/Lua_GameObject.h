#pragma once
#include "../Debug.h"
#include "../GameObject.h"

extern "C"
{
#include "../External/Lua/lua.h"
#include "../External/Lua/lualib.h"
#include "../External/Lua/lauxlib.h"
}

static int GameObject_Create(lua_State* L)
{
	string path = lua_tostring(L, -1);
	ZXEngine::GameObject* go = ZXEngine::GameObject::Create(path);

	if (go == nullptr)
	{
		lua_pushnil(L);
		return 1;
	}

	size_t nbytes = sizeof(ZXEngine::GameObject);
	ZXEngine::GameObject** t = (ZXEngine::GameObject**)lua_newuserdata(L, nbytes);
	*t = go;
	luaL_getmetatable(L, "ZXEngine.GameObject");
	lua_setmetatable(L, -2);

	return 1;
}

static int GameObject_CreateInstance(lua_State* L)
{
	ZXEngine::PrefabStruct* prefab = (ZXEngine::PrefabStruct*)lua_touserdata(L, -1);
	ZXEngine::GameObject* go = ZXEngine::GameObject::CreateInstance(prefab);

	if (go == nullptr)
	{
		lua_pushnil(L);
		return 1;
	}

	size_t nbytes = sizeof(ZXEngine::GameObject);
	ZXEngine::GameObject** t = (ZXEngine::GameObject**)lua_newuserdata(L, nbytes);
	*t = go;
	luaL_getmetatable(L, "ZXEngine.GameObject");
	lua_setmetatable(L, -2);

	return 1;
}

static int GameObject_AsyncCreate(lua_State* L)
{
	string path = lua_tostring(L, -1);
	ZXEngine::GameObject::AsyncCreate(path);
	return 0;
}

static int GameObject_Find(lua_State* L)
{
	string path = lua_tostring(L, -1);
	ZXEngine::GameObject* go = ZXEngine::GameObject::Find(path);
	if (go == nullptr)
	{
		lua_pushnil(L);
		return 1;
	}

	size_t nbytes = sizeof(ZXEngine::GameObject);
	ZXEngine::GameObject** t = (ZXEngine::GameObject**)lua_newuserdata(L, nbytes);
	*t = go;
	luaL_getmetatable(L, "ZXEngine.GameObject");
	lua_setmetatable(L, -2);

	return 1;
}

static int GameObject_SetActive(lua_State* L)
{
	ZXEngine::GameObject** self = (ZXEngine::GameObject**)luaL_checkudata(L, -2, "ZXEngine.GameObject");
	bool active = lua_toboolean(L, -1);

	(*self)->SetActive(active);

	return 0;
}

static int GameObject_AddChild(lua_State* L)
{
	ZXEngine::GameObject** self = (ZXEngine::GameObject**)luaL_checkudata(L, -2, "ZXEngine.GameObject");
	ZXEngine::GameObject** child = (ZXEngine::GameObject**)luaL_checkudata(L, -1, "ZXEngine.GameObject");

	(*self)->AddChild(*child);

	return 0;
}

static int GameObject_RemoveChild(lua_State* L)
{
	ZXEngine::GameObject** self = (ZXEngine::GameObject**)luaL_checkudata(L, -2, "ZXEngine.GameObject");
	ZXEngine::GameObject** child = (ZXEngine::GameObject**)luaL_checkudata(L, -1, "ZXEngine.GameObject");

	(*self)->RemoveChild(*child);

	return 0;
}

static int GameObject_SetParent(lua_State* L)
{
	ZXEngine::GameObject** self = (ZXEngine::GameObject**)luaL_checkudata(L, -2, "ZXEngine.GameObject");
	ZXEngine::GameObject** parent = (ZXEngine::GameObject**)luaL_checkudata(L, -1, "ZXEngine.GameObject");

	(*self)->SetParent(*parent);

	return 0;
}

static int GameObject_GetComponent(lua_State* L)
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
	else if (type == "RectTransform")
	{
		size_t nbytes = sizeof(ZXEngine::RectTransform);
		ZXEngine::RectTransform** t = (ZXEngine::RectTransform**)lua_newuserdata(L, nbytes);
		*t = (*data)->GetComponent<ZXEngine::RectTransform>();
		luaL_getmetatable(L, "ZXEngine.RectTransform");
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
	else if (type == "RigidBody")
	{
		size_t nbytes = sizeof(ZXEngine::ZRigidBody);
		ZXEngine::ZRigidBody** t = (ZXEngine::ZRigidBody**)lua_newuserdata(L, nbytes);
		*t = (*data)->GetComponent<ZXEngine::ZRigidBody>();
		luaL_getmetatable(L, "ZXEngine.RigidBody");
		lua_setmetatable(L, -2);
	}
	else if (type == "Camera")
	{
		size_t nbytes = sizeof(ZXEngine::Camera);
		ZXEngine::Camera** t = (ZXEngine::Camera**)lua_newuserdata(L, nbytes);
		*t = (*data)->GetComponent<ZXEngine::Camera>();
		luaL_getmetatable(L, "ZXEngine.Camera");
		lua_setmetatable(L, -2);
	}
	else if (type == "Collider")
	{
		size_t nbytes = sizeof(ZXEngine::Collider);
		ZXEngine::Collider** t = (ZXEngine::Collider**)lua_newuserdata(L, nbytes);
		*t = (*data)->GetComponent<ZXEngine::Collider>();
		luaL_getmetatable(L, "ZXEngine.Collider");
		lua_setmetatable(L, -2);
	}
	else if (type == "MeshRenderer")
	{
		size_t nbytes = sizeof(ZXEngine::MeshRenderer);
		ZXEngine::MeshRenderer** t = (ZXEngine::MeshRenderer**)lua_newuserdata(L, nbytes);
		*t = (*data)->GetComponent<ZXEngine::MeshRenderer>();
		luaL_getmetatable(L, "ZXEngine.MeshRenderer");
		lua_setmetatable(L, -2);
	}
	else if (type == "GameLogic")
	{
		size_t nbytes = sizeof(ZXEngine::GameLogic);
		ZXEngine::GameLogic** t = (ZXEngine::GameLogic**)lua_newuserdata(L, nbytes);
		*t = (*data)->GetComponent<ZXEngine::GameLogic>();
		luaL_getmetatable(L, "ZXEngine.GameLogic");
		lua_setmetatable(L, -2);
	}
	else if (type == "UIButton")
	{
		size_t nbytes = sizeof(ZXEngine::UIButton);
		ZXEngine::UIButton** t = (ZXEngine::UIButton**)lua_newuserdata(L, nbytes);
		*t = (*data)->GetComponent<ZXEngine::UIButton>();
		luaL_getmetatable(L, "ZXEngine.UIButton");
		lua_setmetatable(L, -2);
	}
	else
	{ 
		ZXEngine::Debug::LogError("Not find lua wrap component: " + type);
	}

	return 1;
}

static int GameObject_SetName(lua_State* L)
{
	ZXEngine::GameObject** self = (ZXEngine::GameObject**)luaL_checkudata(L, -2, "ZXEngine.GameObject");
	string name = lua_tostring(L, -1);

	(*self)->name = name;

	return 0;
}

static int GameObject_FindChild(lua_State* L)
{
	ZXEngine::GameObject** self = (ZXEngine::GameObject**)luaL_checkudata(L, -2, "ZXEngine.GameObject");
	string path = lua_tostring(L, -1);

	ZXEngine::GameObject* go = (*self)->FindChildren(path);
	if (go == nullptr)
	{
		lua_pushnil(L);
		return 1;
	}

	size_t nbytes = sizeof(ZXEngine::GameObject);
	ZXEngine::GameObject** t = (ZXEngine::GameObject**)lua_newuserdata(L, nbytes);
	*t = go;
	luaL_getmetatable(L, "ZXEngine.GameObject");
	lua_setmetatable(L, -2);

	return 1;
}

static int GameObject_IdenticalTo(lua_State* L)
{
	ZXEngine::GameObject** self = (ZXEngine::GameObject**)luaL_checkudata(L, -2, "ZXEngine.GameObject");
	ZXEngine::GameObject** other = (ZXEngine::GameObject**)luaL_checkudata(L, -1, "ZXEngine.GameObject");

	lua_pushboolean(L, *self == *other);

	return 1;
}

static int GameObject_Destroy(lua_State* L)
{
	ZXEngine::GameObject** self = (ZXEngine::GameObject**)luaL_checkudata(L, -1, "ZXEngine.GameObject");
	delete (*self);
	return 0;
}

static const luaL_Reg GameObject_Funcs[] = 
{
	{ "Create",         GameObject_Create         },
	{ "CreateInstance", GameObject_CreateInstance },
	{ "AsyncCreate",    GameObject_AsyncCreate    },
	{ "Find",           GameObject_Find           },
	{ NULL, NULL }
};

static const luaL_Reg GameObject_Funcs_Meta[] = 
{
	{ "SetActive",    GameObject_SetActive    },
	{ "AddChild",     GameObject_AddChild     },
	{ "RemoveChild",  GameObject_RemoveChild  },
	{ "SetParent",    GameObject_SetParent    },
	{ "GetComponent", GameObject_GetComponent },
	{ "SetName",      GameObject_SetName      },
	{ "FindChild",    GameObject_FindChild    },
	{ "IdenticalTo",  GameObject_IdenticalTo  },
	{ "Destroy",      GameObject_Destroy      },
	{ NULL, NULL }
};

LUAMOD_API int luaopen_GameObject(lua_State* L) 
{
	// 创建元表
	luaL_newmetatable(L, "ZXEngine.GameObject");
	// 复制元表，为下一行代码调用准备参数
	lua_pushvalue(L, -1);
	// mt.__index = mt 把-2位置上的mt表(第一行newmetatable的)的__index字段设置为mt(第二行pushvalue复制的)
	lua_setfield(L, -2, "__index");
	// 注册元方法
	luaL_setfuncs(L, GameObject_Funcs_Meta, 0);

	luaL_newlib(L, GameObject_Funcs);
	return 1;
}