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
		static void PrintTable(lua_State* state, int tableIndex, const char* msg);
		static void PrintLuaState(lua_State* state, const char* msg);

		lua_State* GetState();
		void CallFunction(const char* table, const char* func, const char* msg, bool self = true);
		void CallGlobalFunction(const char* func, const char* msg);

	private:
		static LuaManager* mInstance;
		lua_State* L;
	};
}