#pragma once
#include "../Input/InputManager.h"

extern "C"
{
#include "../External/Lua/lua.h"
#include "../External/Lua/lualib.h"
#include "../External/Lua/lauxlib.h"
}

static int InputManager_ShowCursor(lua_State* L)
{
	bool show = lua_toboolean(L, -1);
	ZXEngine::InputManager::GetInstance()->ShowCursor(show);
	return 0;
}

static int InputManager_GetTouchCount(lua_State* L)
{
	lua_pushinteger(L, ZXEngine::InputManager::GetInstance()->GetTouchCount());
	return 1;
}

static int InputManager_GetTouch(lua_State* L)
{
	uint32_t index = lua_tointeger(L, -1);
	ZXEngine::Touch touch = ZXEngine::InputManager::GetInstance()->GetTouch(index);
	lua_newtable(L);
	lua_pushinteger(L, touch.id);
	lua_setfield(L, -2, "id");
	lua_pushnumber(L, touch.x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, touch.y);
	lua_setfield(L, -2, "y");
	lua_pushinteger(L, (int)touch.phase);
	lua_setfield(L, -2, "phase");
	return 1;
}

static const luaL_Reg InputManager_Funcs[] = 
{
	{ "ShowCursor",    InputManager_ShowCursor    },
	{ "GetTouchCount", InputManager_GetTouchCount },
	{ "GetTouch",	   InputManager_GetTouch      },
	{ NULL, NULL }
};

LUAMOD_API int luaopen_InputManager(lua_State* L) 
{
	luaL_newlib(L, InputManager_Funcs);
	return 1;
}