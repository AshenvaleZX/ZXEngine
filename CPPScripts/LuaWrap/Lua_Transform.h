#pragma once
#include "../Component/Transform.h"

extern "C"
{
#include "../External/Lua/lua.h"
#include "../External/Lua/lualib.h"
#include "../External/Lua/lauxlib.h"
}

static int Transform_GetLocalScale(lua_State* L)
{
	ZXEngine::Transform** transform = (ZXEngine::Transform**)luaL_checkudata(L, -1, "ZXEngine.Transform");

	ZXEngine::Vector3 data = (*transform)->GetLocalScale();
	lua_newtable(L);
	lua_pushnumber(L, data.x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, data.y);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L, data.z);
	lua_setfield(L, -2, "z");

	return 1;
}

static int Transform_GetLocalPosition(lua_State* L)
{
	ZXEngine::Transform** transform = (ZXEngine::Transform**)luaL_checkudata(L, -1, "ZXEngine.Transform");

	ZXEngine::Vector3 data = (*transform)->GetLocalPosition();
	lua_newtable(L);
	lua_pushnumber(L, data.x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, data.y);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L, data.z);
	lua_setfield(L, -2, "z");

	return 1;
}

static int Transform_GetLocalEulerAngles(lua_State* L)
{
	ZXEngine::Transform** transform = (ZXEngine::Transform**)luaL_checkudata(L, -1, "ZXEngine.Transform");

	ZXEngine::Vector3 data = (*transform)->GetLocalEulerAngles();
	lua_newtable(L);
	lua_pushnumber(L, data.x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, data.y);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L, data.z);
	lua_setfield(L, -2, "z");

	return 1;
}

static int Transform_GetLocalRotation(lua_State* L)
{
	ZXEngine::Transform** transform = (ZXEngine::Transform**)luaL_checkudata(L, -1, "ZXEngine.Transform");

	ZXEngine::Quaternion data = (*transform)->GetLocalRotation();
	lua_newtable(L);
	lua_pushnumber(L, data.x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, data.y);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L, data.z);
	lua_setfield(L, -2, "z");
	lua_pushnumber(L, data.w);
	lua_setfield(L, -2, "w");

	return 1;
}

static int Transform_SetLocalScale(lua_State* L)
{
	int argc = lua_gettop(L);
	if (argc == 2)
	{
		ZXEngine::Transform** transform = (ZXEngine::Transform**)luaL_checkudata(L, -2, "ZXEngine.Transform");
		lua_getfield(L, -1, "x");
		float x = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, -1, "y");
		float y = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, -1, "z");
		float z = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		(*transform)->SetLocalScale(x, y, z);
	}
	else if (argc == 4)
	{
		ZXEngine::Transform** transform = (ZXEngine::Transform**)luaL_checkudata(L, -4, "ZXEngine.Transform");
		float x = (float)luaL_checknumber(L, -3);
		float y = (float)luaL_checknumber(L, -2);
		float z = (float)luaL_checknumber(L, -1);
		(*transform)->SetLocalScale(x, y, z);
	}
	else
	{
		ZXEngine::Debug::LogError("No matched lua warp function to call: SetLocalScale");
	}
	return 0;
}

static int Transform_SetLocalPosition(lua_State* L)
{
	int argc = lua_gettop(L);
	if (argc == 2)
	{
		ZXEngine::Transform** transform = (ZXEngine::Transform**)luaL_checkudata(L, -2, "ZXEngine.Transform");
		lua_getfield(L, -1, "x");
		float x = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, -1, "y");
		float y = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, -1, "z");
		float z = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		(*transform)->SetLocalPosition(x, y, z);
	}
	else if (argc == 4)
	{
		ZXEngine::Transform** transform = (ZXEngine::Transform**)luaL_checkudata(L, -4, "ZXEngine.Transform");
		float x = (float)luaL_checknumber(L, -3);
		float y = (float)luaL_checknumber(L, -2);
		float z = (float)luaL_checknumber(L, -1);
		(*transform)->SetLocalPosition(x, y, z);
	}
	else
	{
		ZXEngine::Debug::LogError("No matched lua warp function to call: SetLocalPosition");
	}
	return 0;
}

static int Transform_SetLocalEulerAngles(lua_State* L)
{
	int argc = lua_gettop(L);
	if (argc == 2)
	{
		ZXEngine::Transform** transform = (ZXEngine::Transform**)luaL_checkudata(L, -2, "ZXEngine.Transform");
		lua_getfield(L, -1, "x");
		float x = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, -1, "y");
		float y = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, -1, "z");
		float z = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		(*transform)->SetLocalEulerAngles(x, y, z);
	}
	else if (argc == 4)
	{
		ZXEngine::Transform** transform = (ZXEngine::Transform**)luaL_checkudata(L, -4, "ZXEngine.Transform");
		float x = (float)luaL_checknumber(L, -3);
		float y = (float)luaL_checknumber(L, -2);
		float z = (float)luaL_checknumber(L, -1);
		(*transform)->SetLocalEulerAngles(x, y, z);
	}
	else
	{
		ZXEngine::Debug::LogError("No matched lua warp function to call: SetLocalEulerAngles");
	}
	return 0;
}

static int Transform_SetLocalRotation(lua_State* L)
{
	int argc = lua_gettop(L);
	if (argc == 2)
	{
		ZXEngine::Transform** transform = (ZXEngine::Transform**)luaL_checkudata(L, -2, "ZXEngine.Transform");
		lua_getfield(L, -1, "x");
		float x = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, -1, "y");
		float y = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, -1, "z");
		float z = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, -1, "w");
		float w = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		(*transform)->SetLocalRotation(ZXEngine::Quaternion(x, y, z, w));
	}
	else if (argc == 5)
	{
		ZXEngine::Transform** transform = (ZXEngine::Transform**)luaL_checkudata(L, -5, "ZXEngine.Transform");
		float x = (float)luaL_checknumber(L, -4);
		float y = (float)luaL_checknumber(L, -3);
		float z = (float)luaL_checknumber(L, -2);
		float w = (float)luaL_checknumber(L, -1);
		(*transform)->SetLocalRotation(ZXEngine::Quaternion(x, y, z, w));
	}
	else
	{
		ZXEngine::Debug::LogError("No matched lua warp function to call: SetLocalRotation");
	}
	return 0;
}

static int Transform_GetPosition(lua_State* L)
{
	ZXEngine::Transform** transform = (ZXEngine::Transform**)luaL_checkudata(L, -1, "ZXEngine.Transform");

	ZXEngine::Vector3 data = (*transform)->GetPosition();
	lua_newtable(L);
	lua_pushnumber(L, data.x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, data.y);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L, data.z);
	lua_setfield(L, -2, "z");

	return 1;
}

static int Transform_GetEulerAngles(lua_State* L)
{
	ZXEngine::Transform** transform = (ZXEngine::Transform**)luaL_checkudata(L, -1, "ZXEngine.Transform");

	ZXEngine::Vector3 data = (*transform)->GetEulerAngles();
	lua_newtable(L);
	lua_pushnumber(L, data.x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, data.y);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L, data.z);
	lua_setfield(L, -2, "z");

	return 1;
}

static int Transform_GetRotation(lua_State* L)
{
	ZXEngine::Transform** transform = (ZXEngine::Transform**)luaL_checkudata(L, -1, "ZXEngine.Transform");

	ZXEngine::Quaternion data = (*transform)->GetRotation();
	lua_newtable(L);
	lua_pushnumber(L, data.x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, data.y);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L, data.z);
	lua_setfield(L, -2, "z");
	lua_pushnumber(L, data.w);
	lua_setfield(L, -2, "w");

	return 1;
}

static int Transform_SetPosition(lua_State* L)
{
	int argc = lua_gettop(L);
	if (argc == 2)
	{
		ZXEngine::Transform** transform = (ZXEngine::Transform**)luaL_checkudata(L, -2, "ZXEngine.Transform");
		lua_getfield(L, -1, "x");
		float x = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, -1, "y");
		float y = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, -1, "z");
		float z = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		(*transform)->SetPosition(x, y, z);
	}
	else if (argc == 4)
	{
		ZXEngine::Transform** transform = (ZXEngine::Transform**)luaL_checkudata(L, -4, "ZXEngine.Transform");
		float x = (float)luaL_checknumber(L, -3);
		float y = (float)luaL_checknumber(L, -2);
		float z = (float)luaL_checknumber(L, -1);
		(*transform)->SetPosition(x, y, z);
	}
	else
	{
		ZXEngine::Debug::LogError("No matched lua warp function to call: SetPosition");
	}
	return 0;
}

static int Transform_SetEulerAngles(lua_State* L)
{
	int argc = lua_gettop(L);
	if (argc == 2)
	{
		ZXEngine::Transform** transform = (ZXEngine::Transform**)luaL_checkudata(L, -2, "ZXEngine.Transform");
		lua_getfield(L, -1, "x");
		float x = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, -1, "y");
		float y = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, -1, "z");
		float z = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		(*transform)->SetEulerAngles(x, y, z);
	}
	else if (argc == 4)
	{
		ZXEngine::Transform** transform = (ZXEngine::Transform**)luaL_checkudata(L, -4, "ZXEngine.Transform");
		float x = (float)luaL_checknumber(L, -3);
		float y = (float)luaL_checknumber(L, -2);
		float z = (float)luaL_checknumber(L, -1);
		(*transform)->SetEulerAngles(x, y, z);
	}
	else
	{
		ZXEngine::Debug::LogError("No matched lua warp function to call: SetEulerAngles");
	}
	return 0;
}

static int Transform_SetRotation(lua_State* L)
{
	int argc = lua_gettop(L);
	if (argc == 2)
	{
		ZXEngine::Transform** transform = (ZXEngine::Transform**)luaL_checkudata(L, -2, "ZXEngine.Transform");
		lua_getfield(L, -1, "x");
		float x = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, -1, "y");
		float y = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, -1, "z");
		float z = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, -1, "w");
		float w = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		(*transform)->SetRotation(ZXEngine::Quaternion(x, y, z, w));
	}
	else if (argc == 5)
	{
		ZXEngine::Transform** transform = (ZXEngine::Transform**)luaL_checkudata(L, -5, "ZXEngine.Transform");
		float x = (float)luaL_checknumber(L, -4);
		float y = (float)luaL_checknumber(L, -3);
		float z = (float)luaL_checknumber(L, -2);
		float w = (float)luaL_checknumber(L, -1);
		(*transform)->SetRotation(ZXEngine::Quaternion(x, y, z, w));
	}
	else
	{
		ZXEngine::Debug::LogError("No matched lua warp function to call: SetRotation");
	}
	return 0;
}

static int Transform_GetForward(lua_State* L)
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

static int Transform_GetRight(lua_State* L)
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

static int Transform_GetUp(lua_State* L)
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

static const luaL_Reg Transform_Funcs[] = 
{
	{ NULL, NULL }
};

static const luaL_Reg Transform_Funcs_Meta[] = 
{
	{ "GetLocalScale",       Transform_GetLocalScale       },
	{ "GetLocalPosition",    Transform_GetLocalPosition    },
	{ "GetLocalEulerAngles", Transform_GetLocalEulerAngles },
	{ "GetLocalRotation",    Transform_GetLocalRotation    },
	{ "SetLocalScale",       Transform_SetLocalScale       },
	{ "SetLocalPosition",    Transform_SetLocalPosition    },
	{ "SetLocalEulerAngles", Transform_SetLocalEulerAngles },
	{ "SetLocalRotation",    Transform_SetLocalRotation    },
	{ "GetPosition",         Transform_GetPosition         },
	{ "GetEulerAngles",      Transform_GetEulerAngles      },
	{ "GetRotation",         Transform_GetRotation         },
	{ "SetPosition",         Transform_SetPosition         },
	{ "SetEulerAngles",      Transform_SetEulerAngles      },
	{ "SetRotation",         Transform_SetRotation         },
	{ "GetForward",          Transform_GetForward          },
	{ "GetRight",            Transform_GetRight            },
	{ "GetUp",               Transform_GetUp               },
	{ NULL, NULL }
};

LUAMOD_API int luaopen_Transform(lua_State* L) 
{
	luaL_newmetatable(L, "ZXEngine.Transform");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, Transform_Funcs_Meta, 0);

	luaL_newlib(L, Transform_Funcs);
	return 1;
}