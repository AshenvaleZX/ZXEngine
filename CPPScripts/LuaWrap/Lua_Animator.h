#pragma once
#include "../Component/Animator.h"

extern "C"
{
#include "../External/Lua/lua.h"
#include "../External/Lua/lualib.h"
#include "../External/Lua/lauxlib.h"
}

static int Animator_Play(lua_State* L)
{
	ZXEngine::Animator** animator = (ZXEngine::Animator**)luaL_checkudata(L, -2, "ZXEngine.Animator");

	const char* animationName = lua_tostring(L, -1);

	(*animator)->Play(animationName);

	return 0;
}

static int Animator_Switch(lua_State* L)
{
	int argc = lua_gettop(L);

	if (argc == 2)
	{
		ZXEngine::Animator** animator = (ZXEngine::Animator**)luaL_checkudata(L, -2, "ZXEngine.Animator");

		const char* animationName = lua_tostring(L, -1);

		(*animator)->Switch(animationName);
	}
	else if (argc == 3)
	{
		ZXEngine::Animator** animator = (ZXEngine::Animator**)luaL_checkudata(L, -3, "ZXEngine.Animator");

		const char* animationName = lua_tostring(L, -2);

		float time = (float)lua_tonumber(L, -1);

		(*animator)->Switch(animationName, time);
	}
	else
	{
		ZXEngine::Debug::LogError("No matched lua warp function to call: ZXEngine::Animator::Switch");
	}

	return 0;
}

static const luaL_Reg Animator_Funcs[] = 
{
	{ NULL, NULL }
};

static const luaL_Reg Animator_Funcs_Meta[] = 
{
	{ "Play",   Animator_Play   },
	{ "Switch", Animator_Switch },
	{ NULL, NULL }
};

LUAMOD_API int luaopen_Animator(lua_State* L) 
{
	luaL_newmetatable(L, "ZXEngine.Animator");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, Animator_Funcs_Meta, 0);

	luaL_newlib(L, Animator_Funcs);
	return 1;
}