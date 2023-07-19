#pragma once
#include "pubh.h"

extern "C"
{
#include "External/Lua/lua.h"
#include "External/Lua/lualib.h"
#include "External/Lua/lauxlib.h"
}

namespace ZXEngine
{
	class LuaManager
	{
	public:
		static void Create();
		static LuaManager* GetInstance();
		static void PrintTable(lua_State* state, int tableIndex, const char* msg);
		static void PrintLuaState(lua_State* state, const char* msg);

	private:
		static LuaManager* mInstance;

	public:
		LuaManager();
		~LuaManager() {};

		lua_State* GetState();
		void InitLuaState();
		void RestartLuaState();
		void CallFunction(const char* table, const char* func, const char* msg, bool self = true);
		void CallGlobalFunction(const char* func, const char* msg);

	private:
		lua_State* L;
	};
}