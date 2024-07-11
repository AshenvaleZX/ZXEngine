#pragma once
#include "../Component/RectTransform.h"

extern "C"
{
#include "../External/Lua/lua.h"
#include "../External/Lua/lualib.h"
#include "../External/Lua/lauxlib.h"
}

static int RectTransform_GetLocalScale(lua_State* L)
{
	ZXEngine::RectTransform** transform = (ZXEngine::RectTransform**)luaL_checkudata(L, -1, "ZXEngine.RectTransform");

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

static int RectTransform_GetLocalPosition(lua_State* L)
{
	ZXEngine::RectTransform** transform = (ZXEngine::RectTransform**)luaL_checkudata(L, -1, "ZXEngine.RectTransform");

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

static int RectTransform_GetLocalEulerAngles(lua_State* L)
{
	ZXEngine::RectTransform** transform = (ZXEngine::RectTransform**)luaL_checkudata(L, -1, "ZXEngine.RectTransform");

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

static int RectTransform_GetLocalRotation(lua_State* L)
{
	ZXEngine::RectTransform** transform = (ZXEngine::RectTransform**)luaL_checkudata(L, -1, "ZXEngine.RectTransform");

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

static int RectTransform_SetLocalScale(lua_State* L)
{
	int argc = lua_gettop(L);

	if (argc == 2)
	{
		ZXEngine::RectTransform** transform = (ZXEngine::RectTransform**)luaL_checkudata(L, -2, "ZXEngine.RectTransform");
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
		ZXEngine::RectTransform** transform = (ZXEngine::RectTransform**)luaL_checkudata(L, -4, "ZXEngine.RectTransform");
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

static int RectTransform_SetLocalPosition(lua_State* L)
{
	int argc = lua_gettop(L);

	if (argc == 2)
	{
		ZXEngine::RectTransform** transform = (ZXEngine::RectTransform**)luaL_checkudata(L, -2, "ZXEngine.RectTransform");
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
		ZXEngine::RectTransform** transform = (ZXEngine::RectTransform**)luaL_checkudata(L, -4, "ZXEngine.RectTransform");
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

static int RectTransform_SetLocalEulerAngles(lua_State* L)
{
	int argc = lua_gettop(L);

	if (argc == 2)
	{
		ZXEngine::RectTransform** transform = (ZXEngine::RectTransform**)luaL_checkudata(L, -2, "ZXEngine.RectTransform");
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
		ZXEngine::RectTransform** transform = (ZXEngine::RectTransform**)luaL_checkudata(L, -4, "ZXEngine.RectTransform");
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

static int RectTransform_SetLocalRotation(lua_State* L)
{
	int argc = lua_gettop(L);

	if (argc == 2)
	{
		ZXEngine::RectTransform** transform = (ZXEngine::RectTransform**)luaL_checkudata(L, -2, "ZXEngine.RectTransform");
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
		ZXEngine::RectTransform** transform = (ZXEngine::RectTransform**)luaL_checkudata(L, -5, "ZXEngine.RectTransform");
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

static int RectTransform_GetPosition(lua_State* L)
{
	ZXEngine::RectTransform** transform = (ZXEngine::RectTransform**)luaL_checkudata(L, -1, "ZXEngine.RectTransform");

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

static int RectTransform_GetEulerAngles(lua_State* L)
{
	ZXEngine::RectTransform** transform = (ZXEngine::RectTransform**)luaL_checkudata(L, -1, "ZXEngine.RectTransform");

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

static int RectTransform_GetRotation(lua_State* L)
{
	ZXEngine::RectTransform** transform = (ZXEngine::RectTransform**)luaL_checkudata(L, -1, "ZXEngine.RectTransform");

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

static int RectTransform_SetPosition(lua_State* L)
{
	int argc = lua_gettop(L);

	if (argc == 2)
	{
		ZXEngine::RectTransform** transform = (ZXEngine::RectTransform**)luaL_checkudata(L, -2, "ZXEngine.RectTransform");
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
		ZXEngine::RectTransform** transform = (ZXEngine::RectTransform**)luaL_checkudata(L, -4, "ZXEngine.RectTransform");
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

static int RectTransform_SetEulerAngles(lua_State* L)
{
	int argc = lua_gettop(L);

	if (argc == 2)
	{
		ZXEngine::RectTransform** transform = (ZXEngine::RectTransform**)luaL_checkudata(L, -2, "ZXEngine.RectTransform");
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
		ZXEngine::RectTransform** transform = (ZXEngine::RectTransform**)luaL_checkudata(L, -4, "ZXEngine.RectTransform");
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

static int RectTransform_SetRotation(lua_State* L)
{
	int argc = lua_gettop(L);

	if (argc == 2)
	{
		ZXEngine::RectTransform** transform = (ZXEngine::RectTransform**)luaL_checkudata(L, -2, "ZXEngine.RectTransform");
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
		ZXEngine::RectTransform** transform = (ZXEngine::RectTransform**)luaL_checkudata(L, -5, "ZXEngine.RectTransform");
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

static int RectTransform_GetForward(lua_State* L)
{
	ZXEngine::RectTransform** transform = (ZXEngine::RectTransform**)luaL_checkudata(L, -1, "ZXEngine.RectTransform");

	ZXEngine::Vector3 forward = (*transform)->GetForward();
	lua_newtable(L);
	lua_pushnumber(L, forward.x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, forward.y);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L, forward.z);
	lua_setfield(L, -2, "z");

	return 1;
}

static int RectTransform_GetRight(lua_State* L)
{
	ZXEngine::RectTransform** transform = (ZXEngine::RectTransform**)luaL_checkudata(L, -1, "ZXEngine.RectTransform");

	ZXEngine::Vector3 right = (*transform)->GetRight();
	lua_newtable(L);
	lua_pushnumber(L, right.x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, right.y);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L, right.z);
	lua_setfield(L, -2, "z");

	return 1;
}

static int RectTransform_GetUp(lua_State* L)
{
	ZXEngine::RectTransform** transform = (ZXEngine::RectTransform**)luaL_checkudata(L, -1, "ZXEngine.RectTransform");

	ZXEngine::Vector3 up = (*transform)->GetUp();
	lua_newtable(L);
	lua_pushnumber(L, up.x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, up.y);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L, up.z);
	lua_setfield(L, -2, "z");

	return 1;
}

static int RectTransform_SetWidth(lua_State* L)
{
	ZXEngine::RectTransform** transform = (ZXEngine::RectTransform**)luaL_checkudata(L, -2, "ZXEngine.RectTransform");

	float width = (float)luaL_checknumber(L, -1);
	(*transform)->SetWidth(width);

	return 0;
}

static int RectTransform_GetWidth(lua_State* L)
{
	ZXEngine::RectTransform** transform = (ZXEngine::RectTransform**)luaL_checkudata(L, -1, "ZXEngine.RectTransform");

	float width = (*transform)->GetWidth();
	lua_pushnumber(L, width);

	return 1;
}

static int RectTransform_SetHeight(lua_State* L)
{
	ZXEngine::RectTransform** transform = (ZXEngine::RectTransform**)luaL_checkudata(L, -2, "ZXEngine.RectTransform");

	float height = (float)luaL_checknumber(L, -1);
	(*transform)->SetHeight(height);

	return 0;
}

static int RectTransform_GetHeight(lua_State* L)
{
	ZXEngine::RectTransform** transform = (ZXEngine::RectTransform**)luaL_checkudata(L, -1, "ZXEngine.RectTransform");

	float height = (*transform)->GetHeight();
	lua_pushnumber(L, height);

	return 1;
}

static int RectTransform_SetSize(lua_State* L)
{
	ZXEngine::RectTransform** transform = (ZXEngine::RectTransform**)luaL_checkudata(L, -3, "ZXEngine.RectTransform");

	float width = (float)luaL_checknumber(L, -2);
	float height = (float)luaL_checknumber(L, -1);
	(*transform)->SetSize(width, height);

	return 0;
}

static int RectTransform_SetVerticalAnchor(lua_State* L)
{
	ZXEngine::RectTransform** transform = (ZXEngine::RectTransform**)luaL_checkudata(L, -2, "ZXEngine.RectTransform");

	int anchorV = (int)luaL_checkinteger(L, -1);
	(*transform)->SetVerticalAnchor((ZXEngine::UIAnchorVertical)anchorV);

	return 0;
}

static int RectTransform_SetHorizontalAnchor(lua_State* L)
{
	ZXEngine::RectTransform** transform = (ZXEngine::RectTransform**)luaL_checkudata(L, -2, "ZXEngine.RectTransform");

	int anchorH = (int)luaL_checkinteger(L, -1);
	(*transform)->SetHorizontalAnchor((ZXEngine::UIAnchorHorizontal)anchorH);

	return 0;
}

static int RectTransform_SetLocalRectPosition(lua_State* L)
{
	int argc = lua_gettop(L);

	if (argc == 2)
	{
		ZXEngine::RectTransform** transform = (ZXEngine::RectTransform**)luaL_checkudata(L, -2, "ZXEngine.RectTransform");
		lua_getfield(L, -1, "x");
		float x = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, -1, "y");
		float y = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		(*transform)->SetLocalRectPosition(ZXEngine::Vector2(x, y));
	}
	else if (argc == 4)
	{
		ZXEngine::RectTransform** transform = (ZXEngine::RectTransform**)luaL_checkudata(L, -4, "ZXEngine.RectTransform");
		float x = (float)luaL_checknumber(L, -3);
		float y = (float)luaL_checknumber(L, -2);
		float z = (float)luaL_checknumber(L, -1);
		(*transform)->SetLocalRectPosition(x, y, z);
	}
	else
	{
		ZXEngine::Debug::LogError("No matched lua warp function to call: SetLocalRectPosition");
	}

	return 0;
}

static const luaL_Reg RectTransform_Funcs[] =
{
	{ NULL, NULL }
};

static const luaL_Reg RectTransform_Funcs_Meta[] =
{
	{ "GetLocalScale",        RectTransform_GetLocalScale        },
	{ "GetLocalPosition",     RectTransform_GetLocalPosition     },
	{ "GetLocalEulerAngles",  RectTransform_GetLocalEulerAngles  },
	{ "GetLocalRotation",     RectTransform_GetLocalRotation     },
	{ "SetLocalScale",        RectTransform_SetLocalScale        },
	{ "SetLocalPosition",     RectTransform_SetLocalPosition     },
	{ "SetLocalEulerAngles",  RectTransform_SetLocalEulerAngles  },
	{ "SetLocalRotation",     RectTransform_SetLocalRotation     },
	{ "GetPosition",          RectTransform_GetPosition          },
	{ "GetEulerAngles",       RectTransform_GetEulerAngles       },
	{ "GetRotation",          RectTransform_GetRotation          },
	{ "SetPosition",          RectTransform_SetPosition          },
	{ "SetEulerAngles",       RectTransform_SetEulerAngles       },
	{ "SetRotation",          RectTransform_SetRotation          },
	{ "GetForward",           RectTransform_GetForward           },
	{ "GetRight",             RectTransform_GetRight             },
	{ "GetUp",                RectTransform_GetUp                },
	{ "SetWidth",             RectTransform_SetWidth             },
	{ "GetWidth",             RectTransform_GetWidth             },
	{ "SetHeight",            RectTransform_SetHeight            },
	{ "GetHeight",            RectTransform_GetHeight            },
	{ "SetSize",              RectTransform_SetSize              },
	{ "SetVerticalAnchor",    RectTransform_SetVerticalAnchor    },
	{ "SetHorizontalAnchor",  RectTransform_SetHorizontalAnchor  },
	{ "SetLocalRectPosition", RectTransform_SetLocalRectPosition },
	{ NULL, NULL }
};

LUAMOD_API int luaopen_RectTransform(lua_State* L)
{
	luaL_newmetatable(L, "ZXEngine.RectTransform");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, RectTransform_Funcs_Meta, 0);

	luaL_newlib(L, RectTransform_Funcs);
	return 1;
}