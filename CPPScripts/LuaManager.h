#pragma once
#include "pubh.h"

extern "C"
{
#include "Lua/lua.h"
#include "Lua/lualib.h"
#include "Lua/lauxlib.h"
}

namespace ZXEngine
{
	class LuaManager
	{
	public:
		LuaManager();
		~LuaManager() {};

		static void Create();
		static LuaManager* GetInstance();

		lua_State* GetState();

	private:
		static LuaManager* mInstance;
		lua_State* L;
	};
}